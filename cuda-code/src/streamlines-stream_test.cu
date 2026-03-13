#include "nrrd_field.h"

#include <thrust/device_vector.h>
#include <thrust/host_vector.h>
#include <thrust/system/cuda/execution_policy.h>
#include <thrust/tabulate.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <math.h>
#include <type_traits>
#include <vector>

template <typename T> struct pinned_allocator {
  using value_type = T;

  T *allocate(std::size_t n) {
    T *ptr = nullptr;
    cudaMallocHost((void **)&ptr, n * sizeof(T), cudaHostAllocDefault);
    return ptr;
  }
  void deallocate(T *ptr, std::size_t) { cudaFreeHost(ptr); }
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
    t = nanf(0);
  }
};

// -------------------------------------------------------------------------

// We must create a page-locked host vector, so, apparently, the way to do it is
// using a pinned_allocator
typedef thrust::host_vector<integrator_rk4, pinned_allocator<integrator_rk4>>
    pinnedVector;

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

// Hinzugefügt 24. Januar:

int main(int argc, char *argv[]) {
  // Hinzugefügt 24. Januar:
  // Handling command line parameters to automatise benchmarking
  // -----------------------------------------------------------------------
  std::string curr_arg = "";
  std::string str_seeds = "";
  std::string str_steps = "";
  std::string str_vtp = "";
  std::string str_dt = "";
  std::string str_bdepth = "";
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
    if (curr_arg == "-b" || curr_arg == "--bdepth") {
      str_bdepth = arguments[n + 1];
    }
  }
  // -----------------------------------------------------------------------
  // here the number of seeds and of time steps are defined
  // also, the time interval.
  cudaStream_t main_process;
  cudaStreamCreate(&main_process);
  cudaStream_t buffered_copy;
  cudaStreamCreate(&buffered_copy);
  // implement some checking here to avoid passing a zero value by mistake
  unsigned int num_seeds;
  unsigned int num_steps;
  unsigned int buffer_depth;
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
  // end comparison for seeds
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
  // end comparison for steps
  if (str_bdepth.length() > 0) {
    buffer_depth = abs(std::stoi(str_bdepth));
  } else {
    buffer_depth = 1;
    std::cout << "Switching to default value for value of buffer depth: "
              << buffer_depth << " ." << std::endl;
  } // check if there was an incorrect input in the value of buffer depth
  if ((buffer_depth == 0)) {
    buffer_depth = 1; // default value
    std::cout << "Switching to default value for value of buffer depth: "
              << buffer_depth << " ." << std::endl;
  } // check if there is buffer depth 0, which can also indicate other problems.
  // end comparison for buffer depth value

  // these were the original asignations for numbers of seeds and steps.
  /*unsigned int num_seeds = abs(std::stoi(str_seeds));
  unsigned int num_steps = abs(std::stoi(str_steps));*/
  // const float dt = 0.002;
  // const int num_seeds = 10000;
  // const int num_steps = 1000;

  // asignation for dt:: to do, implement error handling here.
  float dt = std::stof(str_dt);

  // at the end, it should use try .. catch blocks here, but I'll leave that for
  // later. also remember to implement the option to turn off the exporting to
  // vtp.

  // load input field
  nrrd_field field("data/jet4_0.486_v.nrrd");

  // Here is where the fun begins anew.  We must creste a page-locked host
  // vector, so, apparently, the way to do it is using a pinned_allocator this
  // is the new form of creating the host_vector we need.
  pinnedVector houtput(num_steps * num_seeds);

  /*
  // prepare output data
  // Here a vector residing in host memory of type integrator_rk4 will be
  declared, with num_steps * num_seeds elements.  What that kind of element is I
  don know yet. thrust::host_vector<integrator_rk4> houtput( num_steps *
  num_seeds );
  */

  auto houti = houtput.begin(); // Dynamic typed variable (at compile time) to
                                // store interator

  // prepare output storeage
  // using streamline = std::vector<integrator_rk4>;
  // std::vector<streamline> streamlines( num_seeds );
  // create initial particle states
  thrust::device_vector<integrator_rk4> dintg(
      num_seeds); // a vector with num_seeds elements of type integrator_rk4 is
                  // created on the device
  std::cout << std::endl
            << "Value of buffer depth: " << buffer_depth << " ." << std::endl;
  // buffer with multiple rows
  thrust::device_vector<integrator_rk4> dintg_gpu_buffer(
      num_seeds *
      buffer_depth); // a vector with num_seeds elements of type integrator_rk4
                     // is created on the device, to serve as a buffer.
  thrust::tabulate(
      dintg.begin(), dintg.end(),
      seed_generator{num_seeds}); // and now its positions are filled with the
                                  // result of seed_generator. What
                                  // seed_generator returns, I don't know
  // quite obscure struct initialisation.
  houti = thrust::copy(dintg.begin(), dintg.end(),
                       houti); // aca se copian elementos desde el incio de
                               // dintg hasta el final en houti,

  // I save the interator of the buffer

  // perform integration steps integrate particles
  // here they use a lambda function to create the parameter to pass to the
  // for_each thing
  auto Schritt = [=] __device__(auto &i) {
    i.step(field, dt); // whatever comes in i, it must have a step member, and
                       // it's invoked here.
  }; // changed here step for Schritt, to try to make the code more legible

  unsigned int count_buffer_rows = 0;

  for (int s = 0; s < num_steps - 1;
       ++s) // se repite para cada paso hasta numero de pasos
  {
    std::cerr << "." << std::flush; // flush the cerror stream
    // perform integration step
    // The first part of the ycle begins here: The first set of data is in the
    // main device vector dintg The first part is, run Step from dintg, in strem
    // 1, called "main_process"
    thrust::for_each(thrust::cuda::par.on(main_process), dintg.begin(),
                     dintg.end(),
                     Schritt); // changed here step for Schritt, to try to make
                               // the code more legible, being hopefully the
                               // same step in the lambda function
    cudaStreamSynchronize(buffered_copy);
    // then, in the same stream, the data from dintg is copied in the buffer,
    // dintg_gpu_buffer I had to cast the vectors as raw pointers, and then make
    // to copy, note the stream name, main_process, at the end.
    if (count_buffer_rows < buffer_depth) {
      // std::cerr << "Element: " << s << "Entro a la copia normal (menos que el
      // largo del buffer) - " << std::flush;
      // cudaMemcpyAsync( thrust::raw_pointer_cast(dintg_gpu_buffer.data()) +
      // (dintg.size()*sizeof(integrator_rk4)*count_buffer_rows) ,
      // thrust::raw_pointer_cast(dintg.data()),
      // dintg.size()*sizeof(integrator_rk4), cudaMemcpyDeviceToDevice,
      // main_process);
      cudaMemcpyAsync(
          thrust::raw_pointer_cast(dintg_gpu_buffer.data() +
                                   (dintg.size() * count_buffer_rows)),
          thrust::raw_pointer_cast(dintg.data()),
          dintg.size() * sizeof(integrator_rk4), cudaMemcpyDeviceToDevice,
          main_process);
      cudaStreamSynchronize(main_process);
    }
    if (count_buffer_rows >= buffer_depth) {
      try {
        // std::cerr << "Element: " << s << "Entro a la copia del buffer a
        // disco" << std::flush;
        //                 for (int j = 0 ; j < buffer_depth ; ++j) {
        //                     houti = thrust::copy(
        //                     thrust::cuda::par.on(buffered_copy),
        //                     dintg_gpu_buffer.begin() + dintg.size()*j,
        //                     dintg_gpu_buffer.begin() + dintg.size()*(j+1) ,
        //                     houti );
        //                     // the result is copied back to houti, which
        //                     means, to host memory
        //                 }
        houti = thrust::copy(thrust::cuda::par.on(buffered_copy),
                             dintg_gpu_buffer.begin(), dintg_gpu_buffer.end(),
                             houti);
        count_buffer_rows = 0;
        // std::cerr << "Element: " << s << "Termina copia del buffer a disco -
        // falta sincronizar - " << std::flush;
        // cudaStreamSynchronize(buffered_copy);
        cudaMemcpyAsync(thrust::raw_pointer_cast(dintg_gpu_buffer.data()) +
                            (dintg.size() * count_buffer_rows),
                        thrust::raw_pointer_cast(dintg.data()),
                        dintg.size() * sizeof(integrator_rk4),
                        cudaMemcpyDeviceToDevice, main_process);
        cudaStreamSynchronize(main_process);

        // std::cerr << "Element: " << s << "Salió de la copia del buffer a
        // disco" << std::flush;
      } catch (thrust::system_error &e) {
        // output an error message and exit
        std::cerr << "Element: " << s
                  << "Error accessing vector element: " << e.what()
                  << std::endl;
        exit(-1);
      }
    }
    count_buffer_rows = count_buffer_rows + 1;

    /*
    cudaStreamSynchronize(buffered_copy); // safeguard of data integrity, it
    must be located here, if it is located above there will be no overlapping.
    Here is the point in execution flow in which the step process is
    // already launched and I need to check if previous copy is finished, so no
    new operations in that stream will be launched. cudaMemcpyAsync(
    thrust::raw_pointer_cast(dintg_gpu_buffer.data()),
    thrust::raw_pointer_cast(dintg.data()), dintg.size()*sizeof(integrator_rk4),
    cudaMemcpyDeviceToDevice, main_process);
    cudaStreamSynchronize(main_process);  // here the main_process stream is
    synchronised, to guarantee that the copy is finished before trying to access
    the buffer from the other stream.
    // the second part is copying the data in the buffer to host vector using
    houti.
    // since the copy was made treating the vector as raw data, all of the data,
    including wrapper information should be there, so the copy must be exactly
    the same as with the thrust only implementation.
    // of note: the stream must be different, of course, it's called
    "buffered_copy" houti = thrust::copy( thrust::cuda::par.on(buffered_copy),
    dintg_gpu_buffer.begin(), dintg_gpu_buffer.end(), houti );  // the result is
    copied back to houti, which means, to host memory
    // No stream sync here, because it'll have no use, If I sync here, there
    would be no overlapping.  I leave it here assuming that the copy to host
    will last more than then for_each in the next step.
    */
  }
  std::cerr << '\n';
  // Here I can destroy the streams, since all work has finished.
  cudaStreamDestroy(main_process);
  cudaStreamDestroy(buffered_copy);
  // copy back and output
  if (str_vtp == "1")
    save_as_vtk(houtput, num_seeds, num_steps, "test_streams.vtp");
  return 0;
}
