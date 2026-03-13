#ifndef __array3d_hpp
#define __array3d_hpp

#include "cudautil.h"

// -------------------------------------------------------------------------

/// a simple wrapper for cudaArray3D
template <typename T> struct array3D {
  dim3 size() const;

  /// resize to (nx, ny, nz); array will be uninitialized
  void resize(dim3 size);

  /// copy host data (nx*ny*nz) elements into array
  void copy(const T *);

  /// return result of an texture lookup (interpolated read)
  __device__ T get(float x, float y, float z) const;

protected:
  cudaArray *m_array = 0;
  cudaExtent m_extent;
  cudaTextureObject_t m_texture;
};

// -------------------------------------------------------------------------

template <typename T> dim3 array3D<T>::size() const {
  return dim3(m_extent.width, m_extent.height, m_extent.depth);
}
// -------------------------------------------------------------------------

template <typename T> void array3D<T>::resize(dim3 size) {
  if (m_array) {
    // free a poossibly previously allocated array
    // and the associated texture object
    cudaDestroyTextureObject(m_texture);
    cudaFreeArray(m_array);

    m_array = 0;
  }

  m_extent = make_cudaExtent(size.x, size.y, size.z);

  auto cdesc = cudaCreateChannelDesc<T>();

  cuda_check(cudaMalloc3DArray(&m_array, &cdesc,
                               make_cudaExtent(m_extent.width * sizeof(T),
                                               m_extent.height, m_extent.depth),
                               0));

  // set up texture
  cudaResourceDesc tr;
  memset(&tr, 0, sizeof(cudaResourceDesc));
  tr.resType = cudaResourceTypeArray;
  tr.res.array.array = m_array;

  cudaTextureDesc td;
  memset(&td, 0, sizeof(cudaTextureDesc));

  td.filterMode = cudaFilterModePoint;
  td.addressMode[0] = cudaAddressModeBorder;
  td.addressMode[1] = cudaAddressModeBorder;
  td.addressMode[2] = cudaAddressModeBorder;
  td.readMode = cudaReadModeElementType;
  td.normalizedCoords = false;

  cuda_check(cudaCreateTextureObject(&m_texture, &tr, &td, NULL));
}

// -------------------------------------------------------------------------

template <typename T> void array3D<T>::copy(T const *data) {
  cudaMemcpy3DParms copyParams = {0};
  copyParams.srcPtr =
      make_cudaPitchedPtr((void *)data, m_extent.width * sizeof(T),
                          m_extent.width, m_extent.height);

  copyParams.dstArray = m_array;
  copyParams.extent = m_extent;
  copyParams.kind = cudaMemcpyHostToDevice;

  cuda_check(cudaMemcpy3D(&copyParams));
}

// -------------------------------------------------------------------------

template <typename T>
__device__ T array3D<T>::get(float x, float y, float z) const {
  
  int x0 = (int)floorf(x + 0.5f);
  int x1 = x0 + 1;
  int y0 = (int)floorf(y + 0.5f);
  int y1 = y0 + 1;
  int z0 = (int)floorf(z + 0.5f);
  int z1 = z0 + 1;

  float4 c000 = tex3D<float4>(m_array, x0, y0, z0);
  float4 c001 = tex3D<float4>(m_array, x0, y0, z1);
  float4 c010 = tex3D<float4>(m_array, x0, y1, z0);
  float4 c011 = tex3D<float4>(m_array, x0, y1, z1);
  float4 c100 = tex3D<float4>(m_array, x1, y0, z0);
  float4 c101 = tex3D<float4>(m_array, x1, y0, z1);
  float4 c110 = tex3D<float4>(m_array, x1, y1, z0);
  float4 c111 = tex3D<float4>(m_array, x1, y1, z1);

  float xd = (x - x0)/(x1 - x0);
  float yd = (y - y0)/(y1 - y0);
  float zd = (z - z0)/(z1 - z0);


  float4 c00 = c000 * (1 - xd) + c100 * xd;
  float4 c01 = c001 * (1 - xd) + c101 * xd;
  float4 c10 = c010 * (1 - xd) + c110 * xd;
  float4 c11 = c011 * (1 - xd) + c111 * xd;

  float4 c0 = c00 * (1 - yd) + c10 * yd;
  float4 c1 = c01 * (1 - yd) + c11 * yd;

  float4 c = c0 * (1 - zd) + c1 * zd;

  return c;
  // return tex3D<T>(m_texture, x, y, z);

}

// -------------------------------------------------------------------------

#endif // __array3d_hpp
