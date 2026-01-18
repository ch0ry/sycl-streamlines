#ifndef __array3d_hpp
#define __array3d_hpp

#include <hip/hip_runtime.h>
#include <cassert>
#include <cstdint>
#include <cmath>
#include "cutil_math.h.hip"   // for float3/float4 operators
// -------------------------------------------------------------------------

template <typename T> struct array3D {
    dim3 m_size{0,0,0};
    T* m_data = nullptr;

    // return dimensions
    dim3 size() const { return m_size; }

    inline  __device__ int idx(int i, int j, int k) const{

        int value = i + m_size.x * (j + m_size.y * k);

        if (value >= m_size.x * m_size.y * m_size.z) {
            printf("Index out of bounds: i=%d, j=%d, k=%d, value=%d, max=%d\n", i, j, k, value, m_size.x * m_size.y * m_size.z);
            value = m_size.x * m_size.y * m_size.z - 1;
        }

        return value;
    }

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

    __device__ __inline__ void tap_accumulate(
        float4& acc, int ti, int tj, int tk, float w) const
    {
        float4 samp = make_float4(-999.0f, -999.0f, -999.0f, 1.0f);
        if (inRange(ti, m_size.x) && inRange(tj,m_size.y) && inRange(tk,m_size.z)) {
            samp = m_data[idx(ti,tj,tk)];
        }
        acc = acc + (w * samp);
    }

      inline __device__  bool inRange(int i, int N) const
      {
          // Checks if 0 <= i < N
          return (unsigned)i < (unsigned)N;
      }

    __device__ T get(float x, float y, float z) const {

       if (z < 0 || z >= m_size.z - 1 ||
           y < 0 || y >= m_size.y - 1 ||
           x < 0 || x >= m_size.x - 1) {
           // out of bounds
           return make_float4(0, 0, 0, 1.0f);
       }

          //printf("%f, %f, %f\n", x, y, z);


        int x0 = (int)floorf(x);
        int x1 = x0 + 1;
        int y0 = (int)floorf(y);
        int y1 = y0 + 1;
        int z0 = (int)floorf(z);
        int z1 = z0 + 1;

        float4 c000 = m_data[idx(x0, y0, z0)];
        float4 c001 = m_data[idx(x0, y0, z1)];
        float4 c010 = m_data[idx(x0, y1, z0)];
        float4 c011 = m_data[idx(x0, y1, z1)];
        float4 c100 = m_data[idx(x1, y0, z0)];
        float4 c101 = m_data[idx(x1, y0, z1)];
        float4 c110 = m_data[idx(x1, y1, z0)];
        float4 c111 = m_data[idx(x1, y1, z1)];

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

      }

};

#endif // __array3d_hpp
