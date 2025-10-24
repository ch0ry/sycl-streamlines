
#ifndef __nrrd_field_hpp
#define __nrrd_field_hpp

#include "array3d_ai.h.hip"
#include "cutil_math.h.hip"

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
  
 // printf("Standing at (%.3f, %.3f, %.3f)\n", pos.x, pos.y, pos.z);

   float4 r = m_array.get(pos.x, pos.y, pos.z);

   result = make_float3(r.x, r.y, r.z);

   const float eps = 1e-5f;
   bool fully_inside = (r.w >= 1.0f - eps);
  
  // printf("w = %.3f\n", r.w);

   return fully_inside;
 }


protected:
  array3D<float4> m_array;
  float3 m_offset;
  float3 m_scale;
};

#endif // __nrrd_field_hpp
