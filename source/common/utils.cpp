/**
 * \file
 * \brief Contains implementation of useful functions
*/


#include "common/utils.hpp"


// ============================================================================


sf::Vector2f getSfmlVector2f(const plug::Vec2d &vec) {
    return sf::Vector2f(vec.x, vec.y);
}


sf::Vector2i getSfmlVector2i(const plug::Vec2d &vec) {
    return sf::Vector2i(vec.x, vec.y);
}


sf::Vector2u getSfmlVector2u(const plug::Vec2d &vec) {
    return sf::Vector2u(vec.x, vec.y);
}


plug::Vec2d getPlugVector(const sf::Vector2f &vec) {
    return plug::Vec2d(vec.x, vec.y);
}


plug::Vec2d getPlugVector(const sf::Vector2i &vec) {
    return plug::Vec2d(vec.x, vec.y);
}


plug::Vec2d getPlugVector(const sf::Vector2u &vec) {
    return plug::Vec2d(vec.x, vec.y);
}


sf::Color getSfmlColor(plug::Color color) {
    return sf::Color(color.r, color.g, color.b, color.a);
}


plug::Color getPlugColor(sf::Color color) {
    return plug::Color(color.r, color.g, color.b, color.a);
}


bool isEqual(double a, double b) {
    return fabs(a - b) < 1e-4;
}


bool isEqual(const plug::Vec2d &a, const plug::Vec2d &b) {
    return isEqual(a.x, b.x) && isEqual(a.y, b.y);
}


bool isEqual(plug::Color a, plug::Color b) {
    return (a.r == b.r) && (a.g == b.g) && (a.b == b.b) && (a.a == b.a);
}


plug::Color hex2Color(uint32_t hex) {
    return plug::Color(hex >> 24, hex >> 16, hex >> 8, hex);
}
