#include <SFML/Graphics.hpp>
#include <stdio.h>
#include <math.h>
#include "vector.hpp"


Vector2D::Vector2D() : x(0), y(0) {}


Vector2D::Vector2D(vec_t init_x, vec_t init_y) : x(init_x), y(init_y) {}


Vector2D::Vector2D(const sf::Vector2f &vec) : x(vec.x), y(vec.y) {}


Vector2D::~Vector2D() { x = NAN, y = NAN; }


vec_t Vector2D::dot(const Vector2D &arg) const {
    return x * arg.x + y * arg.y;
}


vec_t Vector2D::length() const {
    return sqrt(this->dot(*this));
}


Vector2D Vector2D::normalize() const {
    vec_t len = length();
    return Vector2D(x / len, y / len);
}


Vector2D Vector2D::reflected(const Vector2D &normal) const {
    return (*this) - normal * (2 * (*this).dot(normal));
}


void Vector2D::print() const { 
    printf("x: %15.3f, y: %15.3f, len: %15.3f\n", x, y, length());
}


Vector2D::operator sf::Vector2f() const { return sf::Vector2f(x, y); }


Vector2D &operator += (Vector2D &a, const Vector2D &b) {
    a.x += b.x;
    a.y += b.y;
    return a;
}


Vector2D &operator -= (Vector2D &a, const Vector2D &b) {
    a.x -= b.x;
    a.y -= b.y;
    return a;
}


Vector2D &operator *= (Vector2D &a, const Vector2D &b) {
    a.x *= b.x;
    a.y *= b.y;
    return a;
}


Vector2D &operator *= (Vector2D &a, vec_t arg) {
    a.x *= arg;
    a.y *= arg;
    return a;
}


Vector2D &operator /= (Vector2D &a, vec_t arg) {
    a.x /= arg;
    a.y /= arg;
    return a;
}


Vector2D operator + (const Vector2D &a, const Vector2D &b) {
    Vector2D res = a;
    return (res += b);
}


Vector2D operator - (const Vector2D &a, const Vector2D &b) {
    Vector2D res = a;
    return (res -= b);
}


Vector2D operator - (const Vector2D &a) {
    return Vector2D(-a.x, -a.y);
}


Vector2D operator * (const Vector2D &a, const Vector2D &b) {
    Vector2D res = a;
    return (res *= b);
}


Vector2D operator * (const Vector2D &a, vec_t arg) {
    Vector2D res = a;
    return (res *= arg);
}


Vector2D operator / (const Vector2D &a, vec_t arg) {
    Vector2D res = a;
    return (res /= arg);
}
