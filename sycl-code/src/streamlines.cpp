#include "hdf5_field_sycl.h"
#include <sycl/sycl.hpp>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <math.h>
#include <type_traits>
#include <vector>
#include <chrono>


// -------------------------------------------------------------------------

struct integrator_rk4 {
  sycl::float3 p; // position
  float t;        // time
template <typename Field> void step(const Field &field, const float dt) {
    if (sycl::isnan(t))
      return;

    bool ok = true;

    const float dt_half = 0.5 * dt;

    sycl::float3 k1, k2, k3, k4;

    if (!field.get(p, k1))
      ok = false;

    if (!field.get(p + dt_half * k1, k2))
      ok = false;

    if (!field.get(p + dt_half * k2, k3))
      ok = false;

    if (!field.get(p + dt * k3, k4))
      ok = false;

    if (!ok) {
      t = 0;
      return;
    }

    sycl::float3 tmp = {
        (k1.x() + k2.x() + k3.x() + k4.x()) * (dt / 6.0f),
        (k1.y() + k2.y() + k3.y() + k4.y()) * (dt / 6.0f),
        (k1.z() + k2.z() + k3.z() + k4.z()) * (dt / 6.0f)
    };

    // p += dt / 6.0f * (k1 + k2 + k3 + k4);
    p.y() += tmp.y();
    p.x() += tmp.x();
    p.z() += tmp.z();
    
    t += dt;
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

void save_as_vtk(const std::vector<integrator_rk4> houtput, unsigned int num_seeds,
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

  // std::cerr << "wrote " << num_seeds << " streamlines (" << num_points << " points) to " << filename << '\n';
}

// -------------------------------------------------------------------------

int main(int argc, char *argv[]) {
  sycl::queue q_ct1;
  // Hinzugefügt 26. Januar:
  // Handling command line parameters to automatise benchmarking
  // -----------------------------------------------------------------------
  std::string curr_arg = "";
  std::string str_seeds = "";
  std::string str_steps = "";
  std::string str_vtp = "";
  std::string str_dt = "";
  std::string str_id = "";
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
    if (curr_arg == "-i" || curr_arg == "--id") {
      str_id = arguments[n + 1];
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
    // std::cout << "Incorrect or missing argument: number of seeds. Switching to "
    //              "default value: "
    //           << num_seeds << " seeds." << std::endl;
  } // check if there was an incorrect input in number of seeds
  if (num_seeds == 0) {
    num_seeds = 10000; // default value
    // std::cout << "Switching to default value for number of seeds: " << num_seeds
    //           << " seeds." << std::endl;
  } // check if there are 0 seeds, which can indicate other problems.
  if (str_steps.length() > 0) {
    num_steps = abs(std::stoi(str_steps));
  } else {
    num_steps = 1000;
    // std::cout << "Switching to default value for number of steps: " << num_steps
    //           << " steps." << std::endl;
  } // check if there was an incorrect input in the value of time steps
  if (num_steps == 0) {
    num_steps = 1000; // default value
    // std::cout << "Switching to default value for number of steps: " << num_steps
    //           << " steps." << std::endl;
  } // check if there are 0 steps, which can also indicate other problems.
  // asignation for dt:: to do, implement error handling here.
  float dt = std::stof(str_dt);

  using clock = std::chrono::steady_clock;

  auto start_timer = clock::now();

  // load input field
  hdf5_field field(q_ct1, "data/jet_v4.h5");

  auto end_timer = clock::now();

  std::chrono::duration<double, std::milli> field_ms = end_timer - start_timer;
  
  /*
   * This code was initially converted to SYCL using Intel's oneAPI
   * Which used DPCT headers. Using the DPC compiler is more complicated
   * than using AdaptiveCpp's thus, all of the changes that use DPCT were 
   * refactored to a SYCL native apporach, which ACPP accepted.
   */

  /*
   * Since it's inconvenient to make houtput into USM variable
   * We'll leave it as is
   */

  std::vector<integrator_rk4> houtput(num_steps*num_seeds);

  /*
  * We no longer need houti in SYCL, we need to do it the SYCL way
  *
  * auto houti = houtput.begin(); 
  */

  integrator_rk4* dintg = sycl::malloc_device<integrator_rk4>(num_seeds, q_ct1);

  seed_generator gen{static_cast<unsigned>(num_seeds)};

  q_ct1.submit([&](sycl::handler& h){

    h.parallel_for(sycl::range<1>(num_seeds), [=](sycl::id<1> idx) {

        const unsigned i = static_cast<unsigned>(idx[0]);
        dintg[i] = gen(i);
    });

  }).wait();

  q_ct1.memcpy(houtput.data() + (0) * num_seeds, dintg, num_seeds * sizeof(integrator_rk4)).wait();
  /*
  *
  * houti was used in CUDA, with the help of thrust to ease the process 
  * of iterating throughout houtput in blocks of num_seeds, num_steps times
  *
  * This is done in SYCL using memcpy, and moving the adress num_seeds * (i + 1)
  * where i is the iteration number within the main loop
  *
  * houti = std::copy(oneapi::dpl::execution::make_device_policy(q_ct1),
  *                   dintg.begin(), dintg.end(),
  *                   houti); 
  *
  */                   
  
  /*
  *
  * This was previously passed to a thrust iterator to execute 
  * the step function for each integrator_rk4 inside dintg
  *
  * Currently considered clearer to leave the call to step
  *
  * auto Schritt = [field, dt](integrator_rk4 &i, sycl::stream *out) {
  *   i.step(field, dt, out); 
  *  };
  *
  */
  
  start_timer = clock::now();

  for (int s = 0; s < num_steps - 1; ++s) {

    // std::cerr << "." << std::flush; // flush the cerror stream

    /*
    *
    *Had to change mos of the thrust calls for a SYCL queue.submit()
    *It's clearer and also the way to do things using SYCL natively
    */

    q_ct1.submit([&](sycl::handler& h){

      //uncomment and swap for nullptr below for console output

      h.parallel_for(sycl::range<1>(num_seeds), [=](sycl::id<1> idx){

        dintg[idx[0]].step(field, dt);

      });

    });

    q_ct1.wait_and_throw();

    // copy back
    // houti = std::copy(oneapi::dpl::execution::make_device_policy(q_ct1),
    //                   dintg.begin(), dintg.end(),
    //                   houti); // the result is copied back to houti, which
                              // means, to host memory


    q_ct1.memcpy(houtput.data() + (s + 1) * num_seeds, dintg, num_seeds * sizeof(integrator_rk4)).wait();

  }

  end_timer = clock::now();

  sycl::free(dintg, q_ct1);


  // std::cerr << '\n';

  // copy back and output
  if (str_vtp == "1") {
    save_as_vtk(houtput, num_seeds, num_steps, "test.vtp");
  }

  std::chrono::duration<double, std::milli> rk4_ms = end_timer - start_timer;
  
  std::cout << str_id << "," << num_steps << "," << num_seeds << "," << rk4_ms.count() << "," << field_ms.count() << std::endl;

  return 0;
}
