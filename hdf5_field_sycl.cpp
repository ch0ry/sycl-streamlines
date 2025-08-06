#include <memory>
#include <stdexcept>
#include <vector>

#include "hdf5.h"
#include "hdf5_field_sycl.h"
#include <sycl/sycl.hpp>

// namespace sycl = cl::sycl;

// -------------------------------------------------------------------------

hdf5_field::hdf5_field(sycl::queue &q, const std::string &filename)
    : m_array() {

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
  unsigned int ny = dims[0];
  unsigned int nx = dims[1];
  unsigned int nz = dims[2];

  // Allocate and read data
  std::vector<float> rawData(nx * ny * nz * 3);
  H5Dread(dset, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT,
          rawData.data());

  // Clean up
  H5Sclose(space);

  std::vector<sycl::float4> padded(nx * ny * nz);
  for (size_t i = 0; i < padded.size(); ++i) {
    padded[i] = {rawData[i * 3 + 0], rawData[i * 3 + 1], rawData[i * 3 + 2],
                 1.0f};
  }

  m_array.resize(q, nx, ny, nz);
  m_array.copy_to_device(q, padded);

  hid_t scale_dset = H5Dopen(file, "/scale", H5P_DEFAULT);
  float scale[3];
  H5Dread(scale_dset, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, scale);
  // m_scale = float3(scale[0], scale[1], scale[2]);
  sycl::float3 m_scale = {scale[0], scale[1], scale[2]};

  // If you saved spacing as a dataset or attribute, read it here:
  H5Dclose(dset);
  H5Dclose(scale_dset);
  H5Fclose(file);

  // m_offset = sycl::float3(0.0f, 0.0f, 0.0f);
  sycl::float3 m_offset = {0.0f, 0.0f, 0.0f};
}

// -------------------------------------------------------------------------
