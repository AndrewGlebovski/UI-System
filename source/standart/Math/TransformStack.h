/**
 * @file TransformStack.h
 * @author DenzeL2004 (kolomiitsev.dv@phystech.edu)
 *
 * @brief
 *
 * @version 0.1
 * @date 2023-11-16
 *
 * @copyright Copyright DenzeL2004 (c) 2023
 */
#ifndef __PLUG_MATH_TRANSFORM_STACK_H
#define __PLUG_MATH_TRANSFORM_STACK_H

#include "Plug/Math/Transform.h"
#include "Plug/Math/Vec2d.h"

namespace plug {
class TransformStack {
public:
  TransformStack() {}

  virtual ~TransformStack() {}

  /**
   * \brief Enter a transform to the top of the stack and pushes the new
   * transform onto the top of the stack
   */
  virtual void enter(const Transform &transform) = 0;

  /**
   * \brief Removes a transform from the top of the stack
   */
  virtual void leave() = 0;

  /**
   * \brief Gets stack top transform
   */
  virtual Transform top() const = 0;

  /**
   * \brief Converts local position to on-screen position using top transform
   */
  virtual Vec2d apply(const Vec2d &vec) const = 0;

  /**
   * \brief Converts on-screen position to local position using top transform
   */
  virtual Vec2d restore(const Vec2d &vec) const = 0;
};
} // namespace plug

#endif
