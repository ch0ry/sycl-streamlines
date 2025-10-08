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

  td.filterMode = cudaFilterModeLinear;
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
  return tex3D<T>(m_texture, x, y, z);
}

// -------------------------------------------------------------------------

#endif // __array3d_hpp
