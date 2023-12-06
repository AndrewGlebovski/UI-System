/**
 * @file TransformStack.h
 * @author DenzeL2004 (kolomiitsev.dv@phystech.edu)
 *
 * @brief
 *
 * @version 0.1
 * @date 2023-11-16
 *
 * @copyright Copyright DenzeL2004 (c) 2023
 */
#ifndef __PLUG_MATH_VEC2D_H
#define __PLUG_MATH_VEC2D_H

#include <cmath>

namespace plug {
class Vec2d {
public:
  explicit Vec2d(const double x_ = 0.0, const double y_ = 0.0) : x(x_), y(y_) {}

  Vec2d(const Vec2d &src) = default;

  ~Vec2d() = default;

  double length2() const { return x * x + y * y; }

  double length() const { return sqrt(this->length2()); }

  Vec2d &operator=(const Vec2d &vec) {
    x = vec.x;
    y = vec.y;

    return *this;
  }

  Vec2d &operator+=(const Vec2d &vec) {
    x += vec.x;
    y += vec.y;

    return *this;
  }

  Vec2d &operator-=(const Vec2d &vec) {
    x -= vec.x;
    y -= vec.y;

    return *this;
  }

  Vec2d &operator*=(const Vec2d &vec) {
    x *= vec.x;
    y *= vec.y;

    return *this;
  }

  Vec2d &operator/=(const Vec2d &vec) {
    x /= vec.x;
    y /= vec.y;

    return *this;
  }

  Vec2d &operator*=(const double val) {
    x *= val;
    y *= val;

    return *this;
  }

  Vec2d &operator/=(const double val) {
    x /= val;
    y /= val;

    return *this;
  }

  double x;
  double y;
};

inline double dot(const Vec2d &lhs, const Vec2d &rhs) {
  return lhs.x * rhs.x + lhs.y * rhs.y;
}

inline double cross(const Vec2d &lhs, const Vec2d &rhs) {
  return lhs.x * rhs.y - lhs.y * rhs.x;
}

inline Vec2d normalize(const Vec2d &vec) {
  double len = vec.length();
  Vec2d res = Vec2d(vec.x / len, vec.y / len);

  return res;
}

inline Vec2d operator+(const Vec2d &vec1, const Vec2d &vec2) {
  Vec2d res = vec1;
  res += vec2;

  return res;
}

inline Vec2d operator-(const Vec2d &vec1, const Vec2d &vec2) {
  Vec2d res = vec1;
  res -= vec2;

  return res;
}

inline Vec2d operator*(const Vec2d &lhs, const Vec2d &rhs) {
  Vec2d res = lhs;
  res *= rhs;

  return res;
}

inline Vec2d operator/(const Vec2d &lhs, const Vec2d &rhs) {
  Vec2d res = lhs;
  res /= rhs;

  return res;
}

inline Vec2d operator*(const Vec2d &vec, const double scale) {
  Vec2d res = vec;
  res *= scale;

  return res;
}

inline Vec2d operator*(const double scale, const Vec2d &vec) {
  return operator*(vec, scale);
}

inline Vec2d operator/(const Vec2d &vec, const double scale) {
  Vec2d res = vec;
  res /= scale;

  return res;
}
} // namespace plug

#endif