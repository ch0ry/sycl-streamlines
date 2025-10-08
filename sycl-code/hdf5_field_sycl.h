#ifndef __hdf5_field_sycl_hpp
#define __hdf5_field_sycl_hpp

#include "array3d_sycl.h"
#include "floatn.hpp"
#include <sycl/sycl.hpp>

// namespace sycl = cl::sycl;

// -------------------------------------------------------------------------

struct hdf5_field {
  /// initialize from HDF5 file
  hdf5_field(sycl::queue &q, const std::string &filename);

  /// get the interpolated field value at pos
  bool get(sycl::float3 pos, sycl::float3 &result) const {
    pos.x() = pos.x() - m_offset.x();
    pos.y() = pos.y() - m_offset.y();
    pos.z() = pos.z() - m_offset.z();

    pos.x() = pos.x() * m_scale.x();
    pos.y() = pos.y() * m_scale.y();
    pos.z() = pos.z() * m_scale.z();

    // q.submit([&](sycl::handler &h) {
    //   h.single_task([=]() {
    //     sycl::float4
    //   });
    // })
    sycl::global_ptr<const sycl::float4> d_data = m_array.data();
    sycl::float4 r = m_array.get(d_data, pos.x(), pos.y(), pos.z());

    result.x() = r.x();
    result.y() = r.y();
    result.z() = r.z();

    std::cout << "Interpolated value at w is " << r.x() << r.y() << r.z()
              << r.w() << std::endl;

    return r.w() == 1.0;
  }

protected:
  array3D<sycl::float4> m_array;
  sycl::float3 m_offset;
  sycl::float3 m_scale;
};

#endif // __nrrd_field_hpp
