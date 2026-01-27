#include <memory>
#include <stdexcept>
#include <vector>

#include "hdf5.h"
#include "hdf5_field_sycl.h"
#include <sycl/sycl.hpp>
#include "array3d_sycl.h"

// namespace sycl = cl::sycl;

// -------------------------------------------------------------------------


hdf5_field::hdf5_field(sycl::queue &q, const std::string &filename) {

  hid_t file = H5Fopen(filename.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);

  if (file < 0)
    throw std::runtime_error("Failed to open HDF5 file");

  hid_t dset = H5Dopen(file, "/field", H5P_DEFAULT);
  if (dset < 0)
    throw std::runtime_error("Failed to open HDF5 dataset");

  hid_t space = H5Dget_space(dset);
  int ndims = H5Sget_simple_extent_ndims(space);
  if (ndims != 4)
    throw std::runtime_error("Expected 4D dataset");

  hsize_t dims[4];
  H5Sget_simple_extent_dims(space, dims, nullptr);
  unsigned int nx = dims[0];
  unsigned int ny = dims[1];
  unsigned int nz = dims[2];

  // Allocate and read data
  // std::vector<float> rawData(nx * ny * nz * 3);
  float* rawData = sycl::malloc_host<float>(nx * ny * nz * 3, q);
  H5Dread(dset, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT,
          rawData);

  // Clean up
  H5Sclose(space);

  float* deviceRawData = sycl::malloc_device<float>(nx * ny * nz * 3, q);

  q.memcpy(deviceRawData, rawData, nx * ny * nz * 3 * sizeof(float)).wait();

  q.submit([&](sycl::handler &cgh){

    sycl::float4 *padded = sycl::malloc_device<sycl::float4>(nx * ny * nz, q);

    cgh.parallel_for(sycl::range<1>(nx * ny * nz), [=](sycl::id<1> idx){
      padded[idx[0]] = {deviceRawData[idx[0] * 3 + 0], deviceRawData[idx[0] * 3 + 1], deviceRawData[idx[0] * 3 + 2],
                  1.0f};
    });

    m_array = array3D<sycl::float4>(padded, {nx, ny, nz});

  });

  q.wait();

  sycl::free(rawData, q);
  sycl::free(deviceRawData, q);


  hid_t scale_dset = H5Dopen(file, "/scale", H5P_DEFAULT);
  float scale[3];
  H5Dread(scale_dset, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, scale);
  // m_scale = float3(scale[0], scale[1], scale[2]);
  m_scale = {scale[0], scale[1], scale[2]};

  // If you saved spacing as a dataset or attribute, read it here:
  H5Dclose(dset);
  H5Dclose(scale_dset);
  H5Fclose(file);

  // m_offset = sycl::float3(0.0f, 0.0f, 0.0f);
  m_offset = {0.0f, 0.0f, 0.0f};
}

// -------------------------------------------------------------------------
