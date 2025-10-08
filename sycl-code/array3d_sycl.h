#ifndef __array3d_sycl_hpp
#define __array3d_sycl_hpp

#include <dpct/dpct.hpp>
#include <sycl/sycl.hpp>

// -------------------------------------------------------------------------
void cuda_check(dpct::err0 code) {
  if (code != 0) {
    throw std::runtime_error(std::string("CUDA error: ") +
                             dpct::get_error_string_dummy(code));
  }
}

/// a simple wrapper for cudaArray3D
template <typename T> struct array3D {
  dpct::dim3 size() const;

  /// resize to (nx, ny, nz); array will be uninitialized
  void resize(dpct::dim3 size);

  /// copy host data (nx*ny*nz) elements into array
  void copy(const T *);

  /// return result of an texture lookup (interpolated read)
  T get(float x, float y, float z,
        dpct::image_accessor_ext<sycl::float4, 3> m_texture) const;

protected:
  dpct::image_matrix *m_array = 0;
  sycl::range<3> m_extent{0, 0, 0};
  dpct::image_wrapper_base_p m_texture;
};

// -------------------------------------------------------------------------

template <typename T> dpct::dim3 array3D<T>::size() const {
  return dpct::dim3(m_extent[0], m_extent[1], m_extent[2]);
}
// -------------------------------------------------------------------------

template <typename T> void array3D<T>::resize(dpct::dim3 size) {
  if (m_array) {
    // free a poossibly previously allocated array
    // and the associated texture object
    delete m_texture;
    delete m_array;

    m_array = 0;
  }

  m_extent = sycl::range<3>(size.x, size.y, size.z);

  auto cdesc = dpct::image_channel::create<T>();

  cuda_check(DPCT_CHECK_ERROR(
      m_array = new dpct::image_matrix(
          cdesc,
          sycl::range<3>(m_extent[0] * sizeof(T), m_extent[1], m_extent[2]),
          dpct::image_type::standard)));

  // set up texture
  dpct::image_data tr;
  memset(&tr, 0, sizeof(dpct::image_data));

  tr.set_data(m_array);

  dpct::sampling_info td;
  memset(&td, 0, sizeof(dpct::sampling_info));

  td.set(sycl::filtering_mode::linear);
  td.set(sycl::addressing_mode::clamp);
  /*
  DPCT1062:1: SYCL Image doesn't support normalized read mode.
  */
  td.set(sycl::coordinate_normalization_mode::unnormalized);

  cuda_check(DPCT_CHECK_ERROR(m_texture = dpct::create_image_wrapper(tr, td)));
}

// -------------------------------------------------------------------------

template <typename T> void array3D<T>::copy(T const *data) {
  dpct::memcpy_parameter copyParams = {};
  copyParams.from.pitched = dpct::pitched_data(
      (void *)data, m_extent[0] * sizeof(T), m_extent[0], m_extent[1]);

  copyParams.to.image = m_array;
  copyParams.size = m_extent;
  copyParams.direction = dpct::host_to_device;

  cuda_check(DPCT_CHECK_ERROR(dpct::dpct_memcpy(copyParams)));
}

// -------------------------------------------------------------------------

template <typename T>
T array3D<T>::get(float x, float y, float z,
                  dpct::image_accessor_ext<sycl::float4, 3> m_texture) const {
  /*
  DPCT1064:2: Migrated tex3D call is used in a macro/template definition and may
  not be valid for all macro/template uses. Adjust the code.
  */
  return (m_texture).sycl::ext::oneapi::experimental::sample_image(x, y, z);
}

// -------------------------------------------------------------------------

#endif
