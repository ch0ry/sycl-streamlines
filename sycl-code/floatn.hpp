#ifndef FLOATN_HPP
#define FLOATN_HPP

#include <cmath>   // for sqrt, etc. if needed
#include <cstdint> // for standard types if needed

// Minimal plain-old-data structs (like CUDA float3/float4)

struct float3 {
  float x, y, z;

  // Optional: constructors
  float3() = default;
  float3(float x, float y, float z) : x(x), y(y), z(z) {}

  // Optional: operator overloads
  float3 operator+(const float3 &b) const {
    return float3{x + b.x, y + b.y, z + b.z};
  }

  float3 operator-(const float3 &b) const {
    return float3{x - b.x, y - b.y, z - b.z};
  }

  float3 operator*(float s) const { return float3{x * s, y * s, z * s}; }

  float3 &operator+=(const float3 &b) {
    x += b.x;
    y += b.y;
    z += b.z;
    return *this;
  }

  // You can add more as needed: dot, cross, etc.
};

inline float3 operator*(float s, const float3 &v) {
  return float3{v.x * s, v.y * s, v.z * s};
}

struct float4 {
  float x, y, z, w;

  float4() = default;
  float4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
};

#endif // FLOATN_HPP
