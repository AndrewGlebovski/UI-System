/**
 * @file ColorPalette.h
 * @author MeerkatBoss (solodovnikov.ia@phystech.edu)
 *
 * @brief
 *
 * @version 0.1
 * @date 2023-11-15
 *
 * @copyright Copyright MeerkatBoss (c) 2023
 */
#ifndef __PLUG_TOOL_COLOR_PALETTE_H
#define __PLUG_TOOL_COLOR_PALETTE_H

#include "Plug/Color.h"

namespace plug {

/**
 * @brief Palette of tool colors
 */
class ColorPalette {
public:
  /**
   * @brief Get active foreground color
   *
   * @return Active foreground color
   */
  virtual Color getFGColor(void) const = 0;

  /**
   * @brief Get active background color
   *
   * @return Active background color
   */
  virtual Color getBGColor(void) const = 0;

  /**
   * @brief Set new foreground color
   *
   * @param[in] new_fg_color  New value of background color
   */
  virtual void setFGColor(Color new_fg_color) = 0;

  /**
   * @brief Set new background color
   *
   * @param[in] new_bg_color  New value of background color
   */
  virtual void setBGColor(Color new_bg_color) = 0;

  virtual ~ColorPalette() {}
};

} // namespace plug

#endif /* ColorPalette.h */
