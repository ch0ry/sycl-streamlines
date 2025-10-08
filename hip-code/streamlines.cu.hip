#define THRUST_DEVICE_SYSTEM THRUST_DEVICE_SYSTEM_HIP
#include <hip/hip_runtime.h>
#include "nrrd_field.cu.hip"

#include <thrust/device_vector.h>
#include <thrust/host_vector.h>
#include <thrust/system/hip/execution_policy.h>
#include <thrust/tabulate.h>

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
    hipHostMalloc((void **)&ptr, n * sizeof(T), hipHostMallocDefault);
    return ptr;
  }
  void deallocate(T *ptr, std::size_t) { hipHostFree(ptr); }
};

// -------------------------------------------------------------------------

std::ostream &operator<<(std::ostream &out, const float3 &p) {
  return (out << '(' << p.x << ", " << p.y << ", " << p.z << ')');
}

// -------------------------------------------------------------------------

struct integrator_rk4 {
  float3 p; // position
  float t;  // time

  template <typename Field>
  void __device__ step(const Field &field, const float dt) {
    if (isnan(t))
      return;

    const float dt_half = 0.5 * dt;

    float3 k1, k2, k3, k4;

    bool ok = true;

    if (!field.get(p, k1))
      ok = false;

    if (!field.get(p + dt_half * k1, k2))
      ok = false;

    if (!field.get(p + dt_half * k2, k3))
      ok = false;

    if (!field.get(p + dt * k3, k4))
      ok = false;
    //p += dt / 6.0f * (k1 + k2 + k3 + k4);

    float3 tmp = make_float3(
        (k1.x + k2.x + k3.x + k4.x) * (dt / 6.0f),
        (k1.y + k2.y + k3.y + k4.y) * (dt / 6.0f),
        (k1.z + k2.z + k3.z + k4.z) * (dt / 6.0f)
    );

    if (!ok){
      printf("Out of bounds at (%.3f, %.3f, %.3f)\n", p.x, p.y, p.z);
      t = nanf(0);
      return;
      }

    p.y += tmp.y;
    p.x += tmp.x;
    p.z += tmp.z;

    t += dt;
  }
};

// -------------------------------------------------------------------------

struct seed_generator {
  unsigned int num_seeds;

  integrator_rk4 __device__ operator()(unsigned int i) const {
    const float radius = 0.1;
    const float alpha = 2.0f * M_PI * (float)i / (float)num_seeds;

    integrator_rk4 intg;

    intg.t = 0.0;
    intg.p =
        float3{0.5f + radius * cos(alpha), 0.01, 0.5f + radius * sin(alpha)};

    return intg;
  }
};

// -------------------------------------------------------------------------

void save_as_vtk(thrust::host_vector<integrator_rk4> houtput,
                 unsigned int num_seeds, unsigned int num_steps,
                 const std::string &filename) {
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

      coord.push_back(si.p.x);
      coord.push_back(si.p.y);
      coord.push_back(si.p.z);
      itime.push_back(si.t);

      connectivity.push_back(num_points);

      ++num_points;
    }
  }

  // write to VTP file
  std::ofstream cout(filename);

  cout << "<?xml version=\"1.0\"?>\n"
       << "<VTKFile type=\"PolyData\" version=\"0.1\" "
          "byte_order=\"LittleEndian\">"
       << "<PolyData>" << "<Piece " << "NumberOfPoints=\"" << num_points
       << "\" "
       << "NumberOfVerts=\"0\" " << "NumberOfLines=\"" << num_seeds << "\" "
       << "NumberOfStris=\"0\" " << "NumberOfPolys=\"0\">" << "<Points>"
       << "<DataArray " << "type=\"Float32\" " << "NumberOfComponents=\"3\" "
       << "format=\"ascii\">\n";

  for (auto c : coord)
    cout << c << ' ';

  cout << "</DataArray>" << "</Points>";

  cout << "<Lines>" << "<DataArray Name=\"connectivity\" "
       << "type=\"Int32\" format=\"ascii\">\n";

  for (auto c : connectivity)
    cout << c << '\n';

  cout << "</DataArray>" << "<DataArray Name=\"offsets\" "
       << "type=\"Int32\" format=\"ascii\">\n";

  for (auto o : offset)
    cout << o << '\n';

  cout << "</DataArray>" << "</Lines>" << "<PointData Scalars=\"time\">"
       << "<DataArray Name=\"time\" type=\"Float32\" format=\"ascii\">\n";

  for (auto t : itime)
    cout << t << '\n';

  cout << "\n</DataArray>" << "</PointData>" << "</Piece>" << "</PolyData>"
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

  // load input field
  nrrd_field field("../data/jet_v4.h5");

  // prepare output data
  // Here a vector residing in host memory of type integrator_rk4 will be
  // declared, with num_steps * num_seeds elements.  What that kind of element
  // is I don know yet.
  thrust::host_vector<integrator_rk4> houtput(num_steps * num_seeds);

  auto houti = houtput.begin(); // Dynamic typed variable (at compile time) to
                                // store interator

  // prepare output storeage
  // using streamline = std::vector<integrator_rk4>;
  // std::vector<streamline> streamlines( num_seeds );

  // create initial particle states
  thrust::device_vector<integrator_rk4> dintg(
      num_seeds); // a vector with num_seeds elements of type integrator_rk4 is
                  // created on the device
  thrust::tabulate(
      dintg.begin(), dintg.end(),
      seed_generator{num_seeds}); // and now its positions are filled with the
                                  // result of seed_generator. What
                                  // seed_generator returns, I don't know
  // quite obscure struct initialisation.
  houti = thrust::copy(dintg.begin(), dintg.end(),
                       houti); // aca se copian elementos desde el incio de
                               // dintg hasta el final en houti,

  // perform integration steps integrate particles
  // here they use a lambda function to create the parameter to pass to the
  // for_each thing
  auto Schritt = [=] __device__(auto &i) {
    i.step(field, dt); // whatever comes in i, it must have a step member, and
                       // it's invoked here.
  }; // changed here step for Schritt, to try to make the code more legible

  for (int s = 0; s < num_steps - 1;
       ++s) // se repite para cada paso hasta numero de pasos
  {
    std::cerr << "." << std::flush; // flush the cerror stream

    // perform integration step
    thrust::for_each(dintg.begin(), dintg.end(),
                     Schritt); // changed here step for Schritt, to try to make
                               // the code more legible, being hopefully the
                               // same step in the lambda function

    // copy back
    houti = thrust::copy(dintg.begin(), dintg.end(),
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
