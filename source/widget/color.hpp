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
};


#endif
