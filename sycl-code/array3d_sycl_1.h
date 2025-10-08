#ifndef __array3d_sycl_hpp
#define __array3d_sycl_hpp

#include <CL/sycl.hpp>
#include <vector>
#include <memory>

namespace sycl = cl::sycl;

template<typename T>
class array3D {
public:
  array3D() : m_nx(0), m_ny(0), m_nz(0) {}

  void resize(sycl::queue &q, int nx, int ny, int nz) {
    m_nx = nx;
    m_ny = ny;
    m_nz = nz;
    size_t total = nx * ny * nz;
    m_data = sycl::malloc_device<T>(total, q);
  }

  void copy_to_device(sycl::queue &q, const std::vector<T> &host_data) {
    q.memcpy(m_data, host_data.data(), host_data.size() * sizeof(T)).wait();
  }

  sycl::float4 get(const sycl::global_ptr<const sycl::float4> &data,
                   float x, float y, float z) const {
    // use as index space
    int x0 = static_cast<int>(sycl::floor(x));
    int y0 = static_cast<int>(sycl::floor(y));
    int z0 = static_cast<int>(sycl::floor(z));

    float fx = x - x0;
    float fy = y - y0;
    float fz = z - z0;

    sycl::float4 result = {0, 0, 0, 0};

    for (int dz = 0; dz <= 1; ++dz) {
      int zc = z0 + dz;
      if (zc < 0 || zc >= m_nz) continue;
      float wz = dz ? fz : 1.f - fz;

      for (int dy = 0; dy <= 1; ++dy) {
        int yc = y0 + dy;
        if (yc < 0 || yc >= m_ny) continue;
        float wy = dy ? fy : 1.f - fy;

        for (int dx = 0; dx <= 1; ++dx) {
          int xc = x0 + dx;
          if (xc < 0 || xc >= m_nx) continue;
          float wx = dx ? fx : 1.f - fx;

          size_t idx = zc * m_ny * m_nx + yc * m_nx + xc;
          sycl::float4 val = data[idx];
          result += val * (wx * wy * wz);
        }
      }
    }

    return result;
  }

  sycl::global_ptr<T> data() const { return m_data; }
  int nx() const { return m_nx; }
  int ny() const { return m_ny; }
  int nz() const { return m_nz; }

private:
  int m_nx, m_ny, m_nz;
  sycl::global_ptr<T> m_data;
};

#endif // __array3d_sycl_hpp
