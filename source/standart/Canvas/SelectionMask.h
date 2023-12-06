#ifndef __PLUG_CANVAS_SELECTION_MASK_H
#define __PLUG_CANVAS_SELECTION_MASK_H

#include <cstddef>

namespace plug {
class SelectionMask {
public:
  virtual ~SelectionMask(){};

  /**
   * \brief Get width of mask
   */
  virtual size_t getWidth(void) const = 0;

  /**
   * \brief Get height of mask
   */
  virtual size_t getHeight(void) const = 0;

  /**
   * \brief Get boolean flag of mask's cell by coordinates
   */
  virtual bool getPixel(size_t x, size_t y) const = 0;

  /**
   * \brief Set boolean flag of mask's cell by coordinates
   */
  virtual void setPixel(size_t x, size_t y, bool value) = 0;

  /**
   * \brief Fill mask by value
   */
  virtual void fill(bool value) = 0;

  /**
   * \brief Invert every boolean flag of mask's cells
   */
  virtual void invert(void) = 0;
};

} // namespace plug

#endif // __PLUG_CANVAS_SELECTION_MASK_H
