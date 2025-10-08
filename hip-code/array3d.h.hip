#ifndef __array3d_hpp
#define __array3d_hpp

#include "cudautil.h.hip"

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
  hipArray *m_array = 0;
  hipExtent m_extent;
  hipTextureObject_t m_texture;
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
    hipDestroyTextureObject(m_texture);
    hipFreeArray(m_array);

    m_array = 0;
  }

  m_extent = make_hipExtent(size.x, size.y, size.z);

  auto cdesc = hipCreateChannelDesc<T>();

  cuda_check(hipMalloc3DArray(&m_array, &cdesc,
                               make_hipExtent(m_extent.width * sizeof(T),
                                               m_extent.height, m_extent.depth),
                               0));

  // set up texture
  hipResourceDesc tr;
  hipMemset(&tr, 0, sizeof(hipResourceDesc));
  tr.resType = hipResourceTypeArray;
  tr.res.array.array = m_array;

  hipTextureDesc td;
  hipMemset(&td, 0, sizeof(hipTextureDesc));

  td.filterMode = hipFilterModeLinear;
  td.addressMode[0] = hipAddressModeBorder;
  td.addressMode[1] = hipAddressModeBorder;
  td.addressMode[2] = hipAddressModeBorder;
  td.readMode = hipReadModeElementType;
  td.normalizedCoords = false;

  cuda_check(hipCreateTextureObject(&m_texture, &tr, &td, NULL));
}

// -------------------------------------------------------------------------

template <typename T> void array3D<T>::copy(T const *data) {
  hipMemcpy3DParms copyParams = {0};
  copyParams.srcPtr =
      make_hipPitchedPtr((void *)data, m_extent.width * sizeof(T),
                          m_extent.width, m_extent.height);

  copyParams.dstArray = m_array;
  copyParams.extent = m_extent;
  copyParams.kind = hipMemcpyHostToDevice;

  cuda_check(hipMemcpy3D(&copyParams));
}

// -------------------------------------------------------------------------

template <typename T>
__device__ T array3D<T>::get(float x, float y, float z) const {
  return tex3D<T>(m_texture, x, y, z);
}

// -------------------------------------------------------------------------

#endif // __array3d_hpp
