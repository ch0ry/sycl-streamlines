#include "hdf5_field_sycl.h"
#include <dpct/dpct.hpp>
#include <dpct/dpl_utils.hpp>
#include <oneapi/dpl/algorithm>
#include <oneapi/dpl/execution>
#include <sycl/sycl.hpp>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <math.h>
#include <type_traits>
#include <vector>

// Replacement for old thrust experimental pinned_allocator, compatibility for
// newer CUDA versions
template <typename T> struct pinned_allocator {
  using value_type = T;

  T *allocate(std::size_t n) {
    T *ptr = nullptr;
    /*
    DPCT1048:0: The original value cudaHostAllocDefault is not meaningful in the
    migrated code and was removed or replaced with 0. You may need to check the
    migrated code.
    */
    ptr = (T *)sycl::malloc_host(n * sizeof(T), dpct::get_in_order_queue());
    return ptr;
  }
  void deallocate(T *ptr, std::size_t) {
    sycl::free(ptr, dpct::get_in_order_queue());
  }
};

// -------------------------------------------------------------------------

/*
DPCT1011:82: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

std::ostream &operator<<(std::ostream &out, const sycl::float3 &p) {
  return (out << '(' << p.x() << ", " << p.y() << ", " << p.z() << ')');
}
} // namespace dpct_operator_overloading

// -------------------------------------------------------------------------

struct integrator_rk4 {
  sycl::float3 p; // position
  float t;        // time

  template <typename Field>
  void step(const Field &field, const float dt, const sycl::stream &stream_ct1,
            dpct::image_accessor_ext<sycl::float4, 3> m_texture) {
    if (sycl::isnan(t))
      return;

    const float dt_half = 0.5 * dt;

    sycl::float3 k1, k2, k3, k4;

    if (!field.get(p, k1, m_texture))
      goto outside;

    if (!field.get(p + dt_half * k1, k2, m_texture))
      goto outside;

    if (!field.get(p + dt_half * k2, k3, m_texture))
      goto outside;

    if (!field.get(p + dt * k3, k4, m_texture))
      goto outside;

    p += dt / 6.0f * (k1 + k2 + k3 + k4);
    t += dt;

    return;

  outside:
    /*
    DPCT1015:3: Output needs adjustment.
    */
    stream_ct1 << "Out of bounds at (%.3f, %.3f, %.3f)\n";
    /*
    DPCT1017:83: The sycl::nan call is used instead of the nanf call. These two
    calls do not provide exactly the same functionality. Check the potential
    precision and/or performance issues for the generated code.
    */
    t = sycl::nan(0u);
  }
};

// -------------------------------------------------------------------------

struct seed_generator {
  unsigned int num_seeds;

  integrator_rk4 operator()(unsigned int i) const {
    const float radius = 0.1;
    const float alpha = 2.0f * M_PI * (float)i / (float)num_seeds;

    integrator_rk4 intg;

    intg.t = 0.0;
    intg.p = sycl::float3{0.5f + radius * sycl::cos((float)alpha), 0.01,
                          0.5f + radius * sycl::sin((float)alpha)};

    return intg;
  }
};

// -------------------------------------------------------------------------

void save_as_vtk(std::vector<integrator_rk4> houtput, unsigned int num_seeds,
                 unsigned int num_steps, const std::string &filename) {
  std::vector<int> offset, connectivity;
  std::vector<float> coord, itime;

  unsigned int num_points = 0;

  // unpack all the streamline points into separate arrays
  // (discarding any invalid points)
  auto iter = houtput.begin();

  for (unsigned int seed = 0; seed < num_seeds; ++seed, ++iter) {
    offset.push_back(connectivity.size());

    auto iiter = iter;

    for (unsigned int step = 0; step < num_steps; ++step, iiter += num_seeds) {
      auto &si = *iiter;

      if (isnan(si.t))
        break;

      coord.push_back(si.p.x());
      coord.push_back(si.p.y());
      coord.push_back(si.p.z());
      itime.push_back(si.t);

      connectivity.push_back(num_points);

      ++num_points;
    }
  }

  // write to VTP file
  std::ofstream out(filename);

  out << "<?xml version=\"1.0\"?>\n"
      << "<VTKFile type=\"PolyData\" version=\"0.1\" "
         "byte_order=\"LittleEndian\">"
      << "<PolyData>" << "<Piece " << "NumberOfPoints=\"" << num_points << "\" "
      << "NumberOfVerts=\"0\" " << "NumberOfLines=\"" << num_seeds << "\" "
      << "NumberOfStris=\"0\" " << "NumberOfPolys=\"0\">" << "<Points>"
      << "<DataArray " << "type=\"Float32\" " << "NumberOfComponents=\"3\" "
      << "format=\"ascii\">\n";

  for (auto c : coord)
    out << c << ' ';

  out << "</DataArray>" << "</Points>";

  out << "<Lines>" << "<DataArray Name=\"connectivity\" "
      << "type=\"Int32\" format=\"ascii\">\n";

  for (auto c : connectivity)
    out << c << '\n';

  out << "</DataArray>" << "<DataArray Name=\"offsets\" "
      << "type=\"Int32\" format=\"ascii\">\n";

  for (auto o : offset)
    out << o << '\n';

  out << "</DataArray>" << "</Lines>" << "<PointData Scalars=\"time\">"
      << "<DataArray Name=\"time\" type=\"Float32\" format=\"ascii\">\n";

  for (auto t : itime)
    out << t << '\n';

  out << "\n</DataArray>" << "</PointData>" << "</Piece>" << "</PolyData>"
      << "</VTKFile>" << '\n';

  std::cerr << "wrote " << num_seeds << " streamlines (" << num_points
            << " points) to " << filename << '\n';
}

// -------------------------------------------------------------------------

int main(int argc, char *argv[]) {
  dpct::device_ext &dev_ct1 = dpct::get_current_device();
  sycl::queue &q_ct1 = dev_ct1.in_order_queue();
  /*// here the number of seeds and of time steps are defined
  // also, the time interval.
  const float dt = 0.002;
  const int num_seeds = 10000;
  const int num_steps = 1000;*/
  // Hinzugefügt 26. Januar:
  // Handling command line parameters to automatise benchmarking
  // -----------------------------------------------------------------------
  std::string curr_arg = "";
  std::string str_seeds = "";
  std::string str_steps = "";
  std::string str_vtp = "";
  std::string str_dt = "";
  // parse all parameters
  std::vector<std::string> arguments;
  arguments.insert(arguments.end(), argv + 1, argv + argc);
  for (int n = 0; n < arguments.size(); ++n) {
    curr_arg = arguments[n];
    if (curr_arg == "-n" || curr_arg == "--nsteps") {
      str_steps = arguments[n + 1];
    }
    if (curr_arg == "-s" || curr_arg == "--nseeds") {
      str_seeds = arguments[n + 1];
    }
    if (curr_arg == "-v" || curr_arg == "--vtp") {
      str_vtp = arguments[n + 1];
    }
    if (curr_arg == "-t" || curr_arg == "--dt") {
      str_dt = arguments[n + 1];
    }
  }
  // -----------------------------------------------------------------------
  // here the number of seeds and of time steps are defined
  // also, the time interval.
  // Cambios que he hecho, para probar
  // implement some checking here to avoid passing a zero value by mistake
  unsigned int num_seeds;
  unsigned int num_steps;
  // float dt;
  if (str_seeds.length() > 0) {
    num_seeds = abs(std::stoi(str_seeds));
  } else {
    num_seeds = 10000;
    std::cout << "Incorrect or missing argument: number of seeds. Switching to "
                 "default value: "
              << num_seeds << " seeds." << std::endl;
  } // check if there was an incorrect input in number of seeds
  if ((num_seeds == 0)) {
    num_seeds = 10000; // default value
    std::cout << "Switching to default value for number of seeds: " << num_seeds
              << " seeds." << std::endl;
  } // check if there are 0 seeds, which can indicate other problems.
  if (str_steps.length() > 0) {
    num_steps = abs(std::stoi(str_steps));
  } else {
    num_steps = 1000;
    std::cout << "Switching to default value for number of steps: " << num_steps
              << " steps." << std::endl;
  } // check if there was an incorrect input in the value of time steps
  if ((num_steps == 0)) {
    num_steps = 1000; // default value
    std::cout << "Switching to default value for number of steps: " << num_steps
              << " steps." << std::endl;
  } // check if there are 0 steps, which can also indicate other problems.
  // asignation for dt:: to do, implement error handling here.
  float dt = std::stof(str_dt);

  // load input field
  hdf5_field field("../data/jet_v4.h5");

  // prepare output data
  // Here a vector residing in host memory of type integrator_rk4 will be
  // declared, with num_steps * num_seeds elements.  What that kind of element
  // is I don know yet.
  std::vector<integrator_rk4> houtput(num_steps * num_seeds);

  auto houti = houtput.begin(); // Dynamic typed variable (at compile time) to
                                // store interator

  // prepare output storeage
  // using streamline = std::vector<integrator_rk4>;
  // std::vector<streamline> streamlines( num_seeds );

  // create initial particle states
  dpct::device_vector<integrator_rk4> dintg(
      num_seeds); // a vector with num_seeds elements of type integrator_rk4 is
                  // created on the device
  dpct::for_each_index(
      oneapi::dpl::execution::make_device_policy(q_ct1), dintg.begin(),
      dintg.end(),
      seed_generator{num_seeds}); // and now its positions are filled with the
                                  // result of seed_generator. What
                                  // seed_generator returns, I don't know
  // quite obscure struct initialisation.
  houti = std::copy(oneapi::dpl::execution::make_device_policy(q_ct1),
                    dintg.begin(), dintg.end(),
                    houti); // aca se copian elementos desde el incio de
                            // dintg hasta el final en houti,

  // perform integration steps integrate particles
  // here they use a lambda function to create the parameter to pass to the
  // for_each thing
  auto Schritt = [=](auto &i) {
    i.step(field, dt); // whatever comes in i, it must have a step member, and
                       // it's invoked here.
  }; // changed here step for Schritt, to try to make the code more legible

  for (int s = 0; s < num_steps - 1;
       ++s) // se repite para cada paso hasta numero de pasos
  {
    std::cerr << "." << std::flush; // flush the cerror stream

    // perform integration step
    std::for_each(oneapi::dpl::execution::make_device_policy(q_ct1),
                  dintg.begin(), dintg.end(),
                  Schritt); // changed here step for Schritt, to try to make
                            // the code more legible, being hopefully the
                            // same step in the lambda function

    // copy back
    houti = std::copy(oneapi::dpl::execution::make_device_policy(q_ct1),
                      dintg.begin(), dintg.end(),
                      houti); // the result is copied back to houti, which
                              // means, to host memory
                              //

    size_t elements_written = houti - houtput.begin();

    // Now iterate only over the last `num_seeds` elements that were just
    // written
    /*
    for (size_t i = elements_written - num_seeds; i < elements_written; ++i) {
      const auto &val = houtput[i];
      std::cout << '(' << val.p.x << ", " << val.p.y << ", " << val.p.z
                << "): " << val.t << std::endl;
    } */
  }

  std::cerr << '\n';

  // copy back and output
  if (str_vtp == "1")
    save_as_vtk(houtput, num_seeds, num_steps, "test.vtp");
  return 0;
}
