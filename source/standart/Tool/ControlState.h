/**
 * @file ControlState.h
 * @author MeerkatBoss (solodovnikov.ia@phystech.edu)
 *
 * @brief
 *
 * @version 0.1
 * @date 2023-11-15
 *
 * @copyright Copyright MeerkatBoss (c) 2023
 */
#ifndef __PLUG_TOOL_CONTROL_STATE_H
#define __PLUG_TOOL_CONTROL_STATE_H

namespace plug {

/**
 * @brief Current state of tool control button
 */
enum class State {
  Pressed,  /*!< Button is pressed */
  Released, /*!< Button is released */
};

/**
 * @brief State of tool control
 */
struct ControlState {
  State state; /*!< State of tool control button */
};

} // namespace plug

#endif /* ControlState.h */
