#include <cstdio>
#include <cmath>
#include "vector.hpp"


const vec_t EPS = 1e-5;             ///< Requiered for comparsion


Vec2d::Vec2d(const sf::Vector2f &vec) : x(vec_t(vec.x)), y(vec_t(vec.y)) {}


Vec2d::Vec2d(const sf::Vector2u &vec) : x(vec_t(vec.x)), y(vec_t(vec.y)) {}


Vec2d::~Vec2d() { x = NAN, y = NAN; }


vec_t dot(const Vec2d &a, const Vec2d &b) {
    return a.x * b.x + a.y * b.y;
}


vec_t cross(const Vec2d &a, const Vec2d &b) {
    return a.x * b.y - a.y * b.x;
}


vec_t Vec2d::length() const {
    return sqrt(length2());
}


vec_t Vec2d::length2() const {
    return dot(*this, *this);
}


Vec2d normalize(const Vec2d &v) {
    vec_t len = v.length();
    return Vec2d(v.x / len, v.y / len);
}


Vec2d::operator sf::Vector2f() const { return sf::Vector2f(float(x), float(y)); }


Vec2d::operator sf::Vector2i() const { return sf::Vector2i(int(x), int(y)); }


Vec2d &operator += (Vec2d &a, const Vec2d &b) {
    a.x += b.x;
    a.y += b.y;
    return a;
}


Vec2d &operator -= (Vec2d &a, const Vec2d &b) {
    a.x -= b.x;
    a.y -= b.y;
    return a;
}


Vec2d &operator *= (Vec2d &a, const Vec2d &b) {
    a.x *= b.x;
    a.y *= b.y;
    return a;
}


Vec2d &operator /= (Vec2d &a, const Vec2d &b) {
    a.x /= b.x;
    a.y /= b.y;
    return a;
}


Vec2d &operator *= (Vec2d &a, vec_t arg) {
    a.x *= arg;
    a.y *= arg;
    return a;
}


Vec2d &operator /= (Vec2d &a, vec_t arg) {
    a.x /= arg;
    a.y /= arg;
    return a;
}


Vec2d operator + (const Vec2d &a, const Vec2d &b) {
    Vec2d res = a;
    return (res += b);
}


Vec2d operator - (const Vec2d &a, const Vec2d &b) {
    Vec2d res = a;
    return (res -= b);
}


Vec2d operator - (const Vec2d &a) {
    return Vec2d(-a.x, -a.y);
}


Vec2d operator * (const Vec2d &a, const Vec2d &b) {
    Vec2d res = a;
    return (res *= b);
}


Vec2d operator / (const Vec2d &a, const Vec2d &b) {
    Vec2d res = a;
    return (res /= b);
}


Vec2d operator * (const Vec2d &a, vec_t arg) {
    Vec2d res = a;
    return (res *= arg);
}


Vec2d operator * (vec_t arg, const Vec2d &a) {
    Vec2d res = a;
    return (res *= arg);
}


Vec2d operator / (const Vec2d &a, vec_t arg) {
    Vec2d res = a;
    return (res /= arg);
}


bool operator == (const Vec2d &a, const Vec2d &b) {
    return isEqual(a.x, b.x) && isEqual(a.y, b.y);
}


bool isEqual(vec_t a, vec_t b) {
    return (abs(a - b) < EPS);
}
