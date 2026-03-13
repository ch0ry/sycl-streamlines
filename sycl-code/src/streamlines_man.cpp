#include "hdf5_field_sycl.h"

#include <CL/sycl.hpp>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <math.h>
#include <type_traits>
#include <vector>

namespace sycl = cl::sycl;

// -------------------------------------------------------------------------

std::ostream &operator<<(std::ostream &out, const sycl::float3 &p) {
  return (out << '(' << p.x() << ", " << p.y() << ", " << p.z() << ')');
}

// -------------------------------------------------------------------------

struct integrator_rk4 {
  sycl::float3 p; // position
  float t;        // time

  template <typename Field> void step(const Field &field, const float dt) {
    if (sycl::isnan(t))
      return;

    const float dt_half = 0.5 * dt;

    sycl::float3 k1, k2, k3, k4;

    if (!field.get(p, k1))
      goto outside;

    if (!field.get(p + dt_half * k1, k2))
      goto outside;

    if (!field.get(p + dt_half * k2, k3))
      goto outside;

    if (!field.get(p + dt * k3, k4))
      goto outside;

    p += dt / 6.0f * (k1 + k2 + k3 + k4);
    t += dt;

    return;

  outside:
    printf("Out of bounds at (%.3f, %.3f, %.3f)\n", p.x(), p.y(), p.z());
    t = NAN;
  }
};

// -------------------------------------------------------------------------

// -------------------------------------------------------------------------

void save_as_vtk(integrator_rk4 *houtput, unsigned int num_seeds,
                 unsigned int num_steps, const std::string &filename) {
  std::vector<int> offset, connectivity;
  std::vector<float> coord, itime;

  unsigned int num_points = 0;

  // unpack all the streamline points into separate arrays
  // (discarding any invalid points)
  auto iter = houtput;

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

  sycl::queue q; // create a SYCL queue

  // load input field
  hdf5_field field(q, "../data/jet_v4.h5");

  // prepare output data
  // Here a vector residing in host memory of type integrator_rk4 will be
  // declared, with num_steps * num_seeds elements.  What that kind of element
  // is I don know yet.
  integrator_rk4 *houtput = sycl::malloc_host<integrator_rk4>(
      num_steps * num_seeds,
      q); // a vector with num_steps * num_seeds elements
          // of type integrator_rk4 is created on the host

  auto houti = houtput;

  // create initial particle states
  //
  integrator_rk4 *d_integrators = sycl::malloc_device<integrator_rk4>(
      num_seeds, q); // a vector with num_seeds elements of type integrator_rk4
                     // is created on the device

  q.parallel_for(sycl::range<1>(num_seeds), [&](sycl::id<1> i) {
    float radius = 0.1f;
    float alpha = 2.0f * M_PI * i[0] / num_seeds;
    integrator_rk4 val;
    val.t = 0.0f;
    val.p = {0.5f + radius * sycl::cos(alpha), 0.01f,
             0.5f + radius * sycl::sin(alpha)};
    d_integrators[i] = val;
  });
  // and now its positions are filled with the
  // result of seed_generator. What
  // seed_generator returns, I don't know
  // quite obscure struct initialisation.

  // std::memcpy(houtput + (step + 1) * num_seeds, d_integrators,
  //             sizeof(integrator_rk4) * num_seeds);
  q.memcpy(houti, d_integrators, sizeof(integrator_rk4) * num_seeds).wait();
  houti += num_seeds;

  // aca se copian elementos desde el incio de
  // dintg hasta el final en houti,

  // perform integration steps integrate particles
  // here they use a lambda function to create the parameter to pass to the
  // for_each thing
  //

  for (int s = 0; s < num_steps - 1;
       ++s) // se repite para cada paso hasta numero de pasos
  {
    std::cerr << "." << std::flush; // flush the cerror stream

    // perform integration steps

    q.parallel_for(sycl::range<1>(num_seeds),
                   [&](sycl::id<1> i) { d_integrators[i].step(field, dt); });
    // whatever comes in i, it must have a step member, and
    // it's invoked here.
    // changed here step for Schritt, to try to make the code more legible
    //
    q.wait();

    q.memcpy(houti, d_integrators, sizeof(integrator_rk4) * num_seeds).wait();
    // copy back

    size_t elements_written = houti - houtput;

    // Now iterate only over the last `num_seeds` elements that were just
    // written
    // for (size_t i = elements_written - num_seeds; i < elements_written; ++i)
    // {
    //   const auto &val = houtput[i];
    //   std::cout << '(' << val.p.x() << ", " << val.p.y() << ", " << val.p.z()
    //             << "): " << val.t << std::endl;
    // }

    houti += num_seeds;
  }
  std::cerr << '\n';

  // copy back and output
  if (str_vtp == "1")
    save_as_vtk(houtput, num_seeds, num_steps, "test.vtp");

  sycl::free(d_integrators, q);
  sycl::free(houtput, q);

  return 0;
}
