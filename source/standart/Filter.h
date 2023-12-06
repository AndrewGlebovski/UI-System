/**
 * @file Filter.h
 * @author MeerkatBoss (solodovnikov.ia@phystech.edu)
 *
 * @brief
 *
 * @version 0.1
 * @date 2023-11-15
 *
 * @copyright Copyright MeerkatBoss (c) 2023
 */
#ifndef __PLUG_FILTER_H
#define __PLUG_FILTER_H

#include "Plug/Canvas.h"
#include "Plug/Plugin.h"
#include "Plug/Widget.h"

namespace plug {

/**
 * @brief Interface for pixel filter plugin
 */
class Filter : public Plugin {
public:
  /**
   * @brief Apply filter to canvas
   *
   * @param[inout] canvas	Target canvas
   */
  virtual void applyFilter(Canvas &canvas) const = 0;

  /**
   * @brief Retrieve filter configuration widget
   *
   * @return Widget instance of `nullptr` if filter does not require
   *         configuration
   */
  virtual Widget *getWidget(void) = 0;

  virtual ~Filter() override = default;
};

} // namespace plug

#endif /* Filter.h */
