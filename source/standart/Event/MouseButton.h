/**
 * @file MouseButton.h
 * @author MeerkatBoss (solodovnikov.ia@phystech.edu)
 *
 * @brief
 *
 * @version 0.1
 * @date 2023-11-15
 *
 * @copyright Copyright MeerkatBoss (c) 2023
 */
#ifndef __PLUG_EVENT_MOUSE_BUTTON_H
#define __PLUG_EVENT_MOUSE_BUTTON_H

namespace plug {

/**
 * @brief Mouse button
 */
enum class MouseButton {
  Left,
  Right,
  Middle,
  XButton1,
  XButton2,

  ButtonCount
};

} // namespace plug

#endif /* MouseButton.h */
