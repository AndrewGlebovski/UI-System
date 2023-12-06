/**
 * @file Texture.h
 * @author MeerkatBoss (solodovnikov.ia@phystech.edu)
 *
 * @brief Texture data structure
 *
 * @version 0.1
 * @date 2023-11-16
 *
 * @copyright Copyright MeerkatBoss (c) 2023
 */
#ifndef __PLUG_GRAPHICS_TEXTURE_H
#define __PLUG_GRAPHICS_TEXTURE_H

#include <cassert>
#include <cstddef>

#include "Plug/Color.h"

namespace plug {

/**
 * @brief Array of colors (stored on the CPU)
 *
 */
struct Texture {
  Texture(size_t a_width, size_t a_height)
      : data((Color *)new Color[a_width * a_height]), width(a_width),
        height(a_height) {}

  Texture(size_t a_width, size_t a_height, const Color *a_data)
      : Texture(a_width, a_height) {
    for (size_t i = 0; i < width * height; ++i) {
      data[i] = a_data[i];
    }
  }

  Texture(const Texture &other)
      : Texture(other.width, other.height, other.data) {}

  Texture &operator=(const Texture &other) = delete;

  ~Texture(void) { delete[] data; }

  plug::Color getPixel(size_t x, size_t y) const {
    assert(x < width);
    assert(y < height);
    return data[y * width + x];
  }

  void setPixel(size_t x, size_t y, plug::Color color) const {
    assert(x < width);
    assert(y < height);
    data[y * width + x] = color;
  }

  /**
   * @brief Image data
   */
  Color *const data;

  /**
   * @brief Image width (in pixels)
   */
  const size_t width;

  /**
   * @brief Image height (in pixels)
   */
  const size_t height;
};

} // namespace plug

#endif /* Texture.h */
