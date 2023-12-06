/**
 * @file Color.h
 * @author MeerkatBoss (solodovnikov.ia@phystech.edu)
 *
 * @brief Color structure
 *
 * @version 0.1
 * @date 2023-11-15
 *
 * @copyright Copyright MeerkatBoss (c) 2023
 */
#ifndef __PLUG_COLOR_H
#define __PLUG_COLOR_H

#include <cstdint>

namespace plug {

/**
 * @brief Color of pixel in RGBA color space
 */
class Color {
public:
  Color(void) : Color(255, 255, 255, 255) {}

  Color(uint8_t red_, uint8_t green_, uint8_t blue_, uint8_t alpha_ = 255)
      : red(red_), green(green_), blue(blue_), alpha(alpha_) {}

  union {
    uint8_t red; /*!< Red channel value 0 to 255 inclusive */
    uint8_t r;   /*!< Red channer value 0 to 255 inclusive */
  };

  union {
    uint8_t green; /*!< Green channel value 0 to 255 inclusive */
    uint8_t g;     /*!< Green channel value 0 to 255 inclusive */
  };

  union {
    uint8_t blue; /*!< Blue channel value 0 to 255 inclusive */
    uint8_t b;    /*!< Blue channel value 0 to 255 inclusive */
  };

  union {
    uint8_t alpha; /*!< Alpha channel value 0 to 255 inclusive */
    uint8_t a;     /*!< Alpha channel value 0 to 255 inclusive */
  };
};

} // namespace plug

#endif /* Color.h */
