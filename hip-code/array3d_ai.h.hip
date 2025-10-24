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
        return i + m_size.x * (j + m_size.y * k);
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


    // nearest-neighbor for float coords
    __device__ T get(float x, float y, float z) const {

        float xmin = 0.0f;
        float xmax = 1.0f;
        float ymin = 0.0f;
        float ymax = 2.0f;
        float zmin = 0.0f;
        float zmax = 1.0f;

        const float dx = (xmax - xmin) / (float)m_size.x;
        const float dy = (ymax - ymin) / (float)m_size.y;
        const float dz = (zmax - zmin) / (float)m_size.z;

        // World > grid (texel) coords
        // float tx = (x - xmin) / dx;
        // float ty = (y - ymin) / dy;
        // float tz = (z - zmin) / dz;

        // x,y,z already in grid/voxel units
        float tx = x + 0.5f;  // centers at half-integers
        float ty = y + 0.5f;
        float tz = z + 0.5f;


        float ix = tx - 0.5f;
        float iy = ty - 0.5f;
        float iz = tz - 0.5f;

        int i0 = (int)floorf(ix);
        int j0 = (int)floorf(iy);
        int k0 = (int)floorf(iz);
        
        int i1 = i0 + 1;
        int j1 = j0 + 1;
        int k1 = k0 + 1;
        
        float u = ix - i0;
        float v = iy - j0;
        float w = iz - k0;

    
        float omu = 1.0f - u;
        float omv = 1.0f - v;
        float omw = 1.0f - w;

        float w000 = omu*omv*omw;
        float w100 = u*omv*omw;
        float w010 = omu*v*omw;
        float w110 = u*v*omw;
        float w001 = omu*omv*w;
        float w101 = u*omv*w;
        float w011 = omu*v*w;
        float w111 = u*v*w;

      // -------- fast interior path (no bounds checks) --------
      const float eps = 1e-6f;
      const bool interior =
          (tx >= 0.5f) && (tx < m_size.x - 0.5f - eps) &&
          (ty >= 0.5f) && (ty < m_size.y - 0.5f - eps) &&
          (tz >= 0.5f) && (tz < m_size.z - 0.5f - eps);

      if (interior) {
          // All 8 neighbors exist: read directly
          const float4 f000 = m_data[idx(i0,j0,k0)];
          const float4 f100 = m_data[idx(i1,j0,k0)];
          const float4 f010 = m_data[idx(i0,j1,k0)];
          const float4 f110 = m_data[idx(i1,j1,k0)];
          const float4 f001 = m_data[idx(i0,j0,k1)];
          const float4 f101 = m_data[idx(i1,j0,k1)];
          const float4 f011 = m_data[idx(i0,j1,k1)];
          const float4 f111 = m_data[idx(i1,j1,k1)];

          float4 acc = make_float4(0,0,0,0);
          acc = acc + (w000 * f000);
          acc = acc + (w100 * f100);
          acc = acc + (w010 * f010);
          acc = acc + (w110 * f110);
          acc = acc + (w001 * f001);
          acc = acc + (w101 * f101);
          acc = acc + (w011 * f011);
          acc = acc + (w111 * f111);
          // If your voxels have w=1, you can force exact alpha:
          // acc.w = 1.0f;
          printf("Inside Border\n");
          return acc;
      }

      // -------- true-border fallback (per-tap checks) --------
      float4 acc = make_float4(0,0,0,0);
      printf("Border Fallback\n");
      tap_accumulate(acc, i0,j0,k0, w000);
      tap_accumulate(acc, i1,j0,k0, w100);
      tap_accumulate(acc, i0,j1,k0, w010);
      tap_accumulate(acc, i1,j1,k0, w110);
      tap_accumulate(acc, i0,j0,k1, w001);
      tap_accumulate(acc, i1,j0,k1, w101);
      tap_accumulate(acc, i0,j1,k1, w011);
      tap_accumulate(acc, i1,j1,k1, w111);
      return acc;
        
    }
};

#endif // __array3d_hpp
