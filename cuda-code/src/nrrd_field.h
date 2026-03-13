
#ifndef __nrrd_field_hpp
#define __nrrd_field_hpp

#include "array3d.h"
#include "cutil_math.h"

// -------------------------------------------------------------------------

struct nrrd_field {
  /// initialize from NRRD file
  nrrd_field(const std::string &filename);

  /// get the interpolated field value at pos
  __device__ bool get(float3 pos, float3 &result) const {
    pos.x = pos.x - m_offset.x;
    pos.y = pos.y - m_offset.y;
    pos.z = pos.z - m_offset.z;

    pos.x = pos.x * m_scale.x;
    pos.y = pos.y * m_scale.y;
    pos.z = pos.z * m_scale.z;

    float4 r = m_array.get(pos.x, pos.y, pos.z);

    result = make_float3(r.x, r.y, r.z);

    return r.w == 1.0;
  }

protected:
  array3D<float4> m_array;
  float3 m_offset;
  float3 m_scale;
};

#endif // __nrrd_field_hpp
