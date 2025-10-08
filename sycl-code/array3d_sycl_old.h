#ifndef __array3d_sycl_hpp
#define __array3d_sycl_hpp

#include <CL/sycl.hpp>
#include <cmath>

namespace sycl = cl::sycl;

// -------------------------------------------------------------------------

/// a simple wrapper for cudaArray3D
template <typename T> class array3D {
public:
  array3D(sycl::queue &q) : m_q(q) {}

  void resize(sycl::range<3> size) {
    m_size = size;
    if (m_data)
      sycl::free(m_data, m_q);
    m_data = sycl::malloc_device<T>(m_size.size(), m_q);
  }

  void copy(const T *host_data) {
    m_q.memcpy(m_data, host_data, m_size.size() * sizeof(T)).wait();
  }

  sycl::range<3> size() const { return m_size; }

  T *data() const { return m_data; }

  T get(const float x, const float y, const float z) const {
    int x0 = sycl::floor(x), x1 = x0 + 1;
    int y0 = sycl::floor(y), y1 = y0 + 1;
    int z0 = sycl::floor(z), z1 = z0 + 1;

    float dx = x - x0;
    float dy = y - y0;
    float dz = z - z0;

    auto index = [&](int i, int j, int k) -> size_t {
      if (i < 0 || j >= m_size[2] || j < 0 || j >= m_size[1] || k < 0 ||
          k >= m_size[0])
        return 0; // out of bounds, return zero
      return i * m_size[1] * m_size[0] + j * m_size[0] + k;
    };

    T c000 = m_data[index(z0, y0, x0)];
    T c100 = m_data[index(z0, y0, x1)];
    T c010 = m_data[index(z0, y1, x0)];
    T c110 = m_data[index(z0, y1, x1)];
    T c001 = m_data[index(z1, y0, x0)];
    T c101 = m_data[index(z1, y0, x1)];
    T c011 = m_data[index(z1, y1, x0)];
    T c111 = m_data[index(z1, y1, x1)];

    return c000 * (1 - dx) * (1 - dy) * (1 - dz) +
           c100 * (dx) * (1 - dy) * (1 - dz) +
           c010 * (1 - dx) * (dy) * (1 - dz) + c110 * (dx) * (dy) * (1 - dz) +
           c001 * (1 - dx) * (1 - dy) * (dz) + c101 * (dx) * (1 - dy) * (dz) +
           c011 * (1 - dx) * (dy) * (dz) + c111 * (dx) * (dy) * (dz);
  }

private:
  sycl::queue &m_q;
  sycl::range<3> m_size;
  T *m_data = nullptr;
};

#endif
