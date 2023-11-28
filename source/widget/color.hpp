/**
 * \file
 * \brief Contains my color struct implementation
*/


#ifndef _COLOR_H_
#define _COLOR_H_


#include <cstdint>
#include <SFML/Graphics.hpp>


/// Color of pixel in RGBA color space
class Color {
public:
    Color() : Color(255, 255, 255, 255) {}

    Color(uint8_t red_, uint8_t green_, uint8_t blue_, uint8_t alpha_ = 255) :
        red(red_), green(green_), blue(blue_), alpha(alpha_) {}

    explicit Color(uint32_t hex) :
        Color(hex >> 24, hex >> 16, hex >> 8, hex) {}

    union {
        uint8_t red;
        uint8_t r;
    };

    union {
        uint8_t green;
        uint8_t g;
    };

    union {
        uint8_t blue;
        uint8_t b;
    };

    union {
        uint8_t alpha;
        uint8_t a;
    };

    operator sf::Color() const {
        return sf::Color(r, g, b, a);
    }

    bool operator == (Color color) const {
        return (r == color.r) && (g == color.g) && (b == color.b) && (a == color.a);
    }
};


const Color Black(0, 0, 0);
const Color White(255, 255, 255);
const Color Red(255, 0, 0);
const Color Green(0, 255, 0);
const Color Blue(0, 0, 255);
const Color Yellow(255, 255, 0);
const Color Magenta(255, 0, 255);
const Color Cyan(0, 255, 255);


#endif
