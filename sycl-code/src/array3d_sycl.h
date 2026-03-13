#ifndef __array3d_sycl_hpp
#define __array3d_sycl_hpp

#include <sycl/sycl.hpp>


/// a simple wrapper for cudaArray3D
template <typename T> struct array3D {

  array3D() = default;

  array3D(T* data, sycl::float3 size);

  sycl::float3 size() const ;

  /// resize to (nx, ny, nz); array will be uninitialized
  // void resize(sycl::queue &q, dpct::dim3 size);

  /// copy host data (nx*ny*nz) elements into array
  // void copy(sycl::queue &q, const T *data);

  /// return result of an texture lookup (interpolated read)
  sycl::float4 get(float x, float y, float z) const;

  // index the data array
  int idx (int i, int j, int k) const;

protected:
  sycl::float3 m_size{0, 0, 0};
  T* m_data;
};

// -------------------------------------------------------------------------

template <typename T>
array3D<T>::array3D(T* data, sycl::float3 size)
: m_size(size), m_data(data)
{}

// -------------------------------------------------------------------------

template <typename T>
int array3D<T>::idx(int i, int j, int k) const {

  int value = i + m_size.x() * (j + m_size.y() * k);

  if (value >= m_size.x() * m_size.y() * m_size.z()) {
      // printf("Index out of bounds: i=%d, j=%d, k=%d, value=%d, max=%d\n", i, j, k, value, m_size.x * m_size.y * m_size.z);
      value = m_size.x() * m_size.y() * m_size.z() - 1;
  }

  return value;
}
// -------------------------------------------------------------------------
template <typename T>
sycl::float3 array3D<T>::size() const {
  return m_size; 
}
// -------------------------------------------------------------------------

// template <typename T>
// void array3D<T>::resize(sycl::queue &q, dpct::dim3 size) {
//   if (m_data) {
//     // free a poossibly previously allocated array
//     // and the associated texture object
//     sycl::free(m_data, q);
//
//     m_data = 0;
//   }
//   m_size = size;
//   m_data = sycl::malloc_device<T>(m_size.x * m_size.y * m_size.z * sizeof(T), q);
// }

// -------------------------------------------------------------------------

// template <typename T> void array3D<T>::copy(sycl::queue &q, T const *data) {
//     q.memcpy(m_data, data, m_size.x * m_size.y * m_size.z * sizeof(T)).wait();
// }

// -------------------------------------------------------------------------

template <typename T>
sycl::float4 array3D<T>::get(float x, float y, float z) const {

  if (z < 0 || z >= m_size.z() - 1 ||
      y < 0 || y >= m_size.y() - 1 ||
      x < 0 || x >= m_size.x() - 1) {
      // out of bounds
      return sycl::float4{0, 0, 0, 1.0f};
  }


  const int x0 = (int)sycl::floor(x);
  const int x1 = x0 + 1;
  const int y0 = (int)sycl::floor(y);
  const int y1 = y0 + 1;
  const int z0 = (int)sycl::floor(z);
  const int z1 = z0 + 1;

  const sycl::float4 c000 = m_data[idx(x0, y0, z0)];
  const sycl::float4 c001 = m_data[idx(x0, y0, z1)];
  const sycl::float4 c010 = m_data[idx(x0, y1, z0)];
  const sycl::float4 c011 = m_data[idx(x0, y1, z1)];
  const sycl::float4 c100 = m_data[idx(x1, y0, z0)];
  const sycl::float4 c101 = m_data[idx(x1, y0, z1)];
  const sycl::float4 c110 = m_data[idx(x1, y1, z0)];
  const sycl::float4 c111 = m_data[idx(x1, y1, z1)];

  const float xd = (x - x0)/(x1 - x0);
  const float yd = (y - y0)/(y1 - y0);
  const float zd = (z - z0)/(z1 - z0);


  const sycl::float4 c00 = c000 * (1 - xd) + c100 * xd;
  const sycl::float4 c01 = c001 * (1 - xd) + c101 * xd;
  const sycl::float4 c10 = c010 * (1 - xd) + c110 * xd;
  const sycl::float4 c11 = c011 * (1 - xd) + c111 * xd;

  const sycl::float4 c0 = c00 * (1 - yd) + c10 * yd;
  const sycl::float4 c1 = c01 * (1 - yd) + c11 * yd;

  const sycl::float4 c = c0 * (1 - zd) + c1 * zd;
  

  return c;
}

// -------------------------------------------------------------------------

#endif
