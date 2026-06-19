#include <cmath>
#include <iostream>
struct Vector3D {

  union {
    struct {
      float x, y, z;
    };
    float raw[3];
  };

  Vector3D() = default;
  Vector3D(float tx, float ty, float tz) : x(tx), y(ty), z(tz) {}

  float& operator[](int i) { return raw[i]; }
  const float& operator[](int i) const { return raw[i]; }
  Vector3D& operator*=(float s) {
    x *= s;
    y *= s;
    z *= s;
    return (*this);
  }
  Vector3D& operator/=(float s) {
    s = 1.0F / s;
    x *= s;
    y *= s;
    z *= s;
    return (*this);
  }
};
inline Vector3D operator*(const Vector3D& v, float s) {
  return (Vector3D(v[0] * s, v[1] * s, v[2] * s));
}
inline Vector3D operator/(const Vector3D& v, float s) {
  s = 1.0F / s;
  return (Vector3D(v[0] * s, v[1] * s, v[2] * s));
}
inline Vector3D operator-(const Vector3D& v) {
  return (Vector3D(-v[0], -v[1], -v[2]));
}
inline float Magnitude(const Vector3D& v) {
  return (sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]));
}
inline Vector3D Normalize(const Vector3D& v) { return (v / Magnitude(v)); }

int main(int argc, char* argv[]) {
  Vector3D V3D(10, 9, 8);
  V3D *= 3.0f;
  V3D = Normalize(V3D);
  std::cout << V3D[0] << "\t" << V3D[1] << "\t" << V3D[2] << "\n";
}
