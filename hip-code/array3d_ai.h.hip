#ifndef __array3d_hip_hpp
#define __array3d_hip_hpp

#include <hip/hip_runtime.h>
#include <cassert>
#include <cstdint>
#include <cmath>
#include "cutil_math.h.hip"   // for float3/float4 operators

__host__ __device__ inline float fmaxf_safe(float x, float y) { return x > y ? x : y; }
__host__ __device__ inline float fminf_safe(float x, float y) { return x < y ? x : y; }

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
    __device__ T get(float i, float j, float k) const {
        i = fminf(fmaxf(i, 0.0f), m_size.x-1);
        j = fminf(fmaxf(j, 0), m_size.y-1);
        k = fminf(fmaxf(k, 0), m_size.z-1);
        return m_data[static_cast<int>((i + m_size.x*(j + m_size.y*k)) + 0.0f)];
    }

    };

#endif // __array3d_hpp
