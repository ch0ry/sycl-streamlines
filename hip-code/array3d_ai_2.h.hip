#ifndef __array3d_hip_hpp
#define __array3d_hip_hpp

#include <hip/hip_runtime.h>
#include <cassert>
#include <cstdint>
#include <cmath>
#include "cutil_math.h.hip"   // for float3/float4 operators

template <typename T> struct array3D {
    dim3 m_size{0,0,0};
    T* m_data = nullptr;

    // return dimensions
    dim3 size() const { return m_size; }

    // resize to (nx, ny, nz); array will be uninitialized
    void resize(dim3 size) {
        if (m_data) hipFree(m_data);
        m_size = size;
        hipMalloc(&m_data, size.x * size.y * size.z * sizeof(T));
    }

    // copy host data (nx*ny*nz elements) into device array
    void copy(const T* host_data) {
        hipMemcpy(m_data, host_data,
                  m_size.x * m_size.y * m_size.z * sizeof(T),
                  hipMemcpyHostToDevice);
    }

    // linear indexing
    __device__ T get(int i, int j, int k) const {
        i = std::min(std::max(i, 0), int(m_size.x)-1);
        j = std::min(std::max(j, 0), int(m_size.y)-1);
        k = std::min(std::max(k, 0), int(m_size.z)-1);
        return m_data[i + m_size.x*(j + m_size.y*k)];
    }

    // nearest-neighbor for float coords
    __device__ T get(float x, float y, float z) const {
        int i = static_cast<int>(x + 0.5f);
        int j = static_cast<int>(y + 0.5f);
        int k = static_cast<int>(z + 0.5f);
        return get(i,j,k);
    }
};
#endif // __array3d_hpp
