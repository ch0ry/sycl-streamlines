
#ifndef __nrrd_field_hpp
#define __nrdd_field_hpp

#include <sycl/sycl.hpp>
#include <dpct/dpct.hpp>
#include "array3d.h"
#include "cutil_math.h"

// -------------------------------------------------------------------------

struct nrrd_field {
  /// initialize from NRRD file
  nrrd_field(const std::string &filename);

  /// get the interpolated field value at pos
  bool get(sycl::float3 pos, sycl::float3 &result,
           dpct::image_accessor_ext<sycl::float4, 3> m_texture) const {
    pos.x() = pos.x() - m_offset.x();
    pos.y() = pos.y() - m_offset.y();
    pos.z() = pos.z() - m_offset.z();

    pos.x() = pos.x() * m_scale.x();
    pos.y() = pos.y() * m_scale.y();
    pos.z() = pos.z() * m_scale.z();

    sycl::float4 r = m_array.get(pos.x(), pos.y(), pos.z(), m_texture);

    result = sycl::float3(r.x(), r.y(), r.z());

    return r.w() == 1.0;
  }

protected:
  array3D<sycl::float4> m_array;
  sycl::float3 m_offset;
  sycl::float3 m_scale;
};

#endif // __nrrd_field_hpp

