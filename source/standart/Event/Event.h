/**
 * @file Event.h
 * @author MeerkatBoss (solodovnikov.ia@phystech.edu)
 *
 * @brief
 *
 * @version 0.1
 * @date 2023-11-15
 *
 * @copyright Copyright MeerkatBoss (c) 2023
 */
#ifndef __PLUG_EVENT_EVENT_H
#define __PLUG_EVENT_EVENT_H

#include <cstddef>

#include "Plug/Event/KeyCode.h"
#include "Plug/Event/MouseButton.h"
#include "Plug/Math.h"

namespace plug {

/**
 * @brief Constants for types of predefined events
 */
enum EventType {
  Tick = 0,             /*!< TickEvent */
  MouseMove = 1,        /*!< MouseMoveEvent */
  MousePressed = 2,     /*!< MousePressedEvent */
  MouseReleased = 3,    /*!< MouseReleasedEvent */
  KeyboardPressed = 4,  /*!< KeyboardPressedEvent */
  KeyboardReleased = 5, /*!< KeyboardReleasedEvent */
};

/**
 * @brief Base class for all events
 */
class Event {
public:
  Event(size_t type) : m_type(type) {}

  /**
   * @brief Retrieve type of underlying event
   *
   * @return Retrieved type
   */
  size_t getType(void) const { return m_type; }

private:
  const size_t m_type;
};

/**
 * @brief Event of mouse changing position
 */
struct MouseMoveEvent : public Event {
  MouseMoveEvent(const Vec2d &position, bool shift_pressed, bool ctrl_pressed,
                 bool alt_pressed)
      : Event(MouseMove), shift(shift_pressed), ctrl(ctrl_pressed),
        alt(alt_pressed), pos(position) {}

  bool shift; /*!< Shift is pressed */
  bool ctrl;  /*!< Ctrl is pressed */
  bool alt;   /*!< Alt is pressed */
  Vec2d pos;  /*!< New mouse position */
};

/**
 * @brief Event of mouse button being pressed
 */
struct MousePressedEvent : public Event {
  MousePressedEvent(MouseButton button, const Vec2d &position,
                    bool shift_pressed, bool ctrl_pressed, bool alt_pressed)
      : Event(MousePressed), button_id(button), shift(shift_pressed),
        ctrl(ctrl_pressed), alt(alt_pressed), pos(position) {}
  MouseButton button_id; /*!< Pressed button */
  bool shift;            /*!< Shift is pressed */
  bool ctrl;             /*!< Ctrl is pressed */
  bool alt;              /*!< Alt is pressed */
  Vec2d pos;             /*!< Mouse position when pressed */
};

/**
 * @brief Event of mouse button being released
 */
struct MouseReleasedEvent : public Event {
  MouseReleasedEvent(MouseButton button, const Vec2d &position,
                     bool shift_pressed, bool ctrl_pressed, bool alt_pressed)
      : Event(MouseReleased), button_id(button), shift(shift_pressed),
        ctrl(ctrl_pressed), alt(alt_pressed), pos(position) {}
  MouseButton button_id; /*!< Released button */
  bool shift;            /*!< Shift is pressed */
  bool ctrl;             /*!< Ctrl is pressed */
  bool alt;              /*!< Alt is pressed */
  Vec2d pos;             /*!< Mouse position when released */
};

/**
 * @brief Event of keyboard button being pressed
 */
struct KeyboardPressedEvent : public Event {
  KeyboardPressedEvent(KeyCode key, bool shift_pressed, bool ctrl_pressed,
                       bool alt_pressed)
      : Event(KeyboardPressed), key_id(key), shift(shift_pressed),
        ctrl(ctrl_pressed), alt(alt_pressed) {}
  KeyCode key_id; /*!< Pressed key */
  bool shift;     /*!< Shift is pressed */
  bool ctrl;      /*!< Ctrl is pressed */
  bool alt;       /*!< Alt is pressed */
};

/**
 * @brief Event of keyboard button being released
 */
struct KeyboardReleasedEvent : public Event {
  KeyboardReleasedEvent(KeyCode key, bool shift_pressed, bool ctrl_pressed,
                        bool alt_pressed)
      : Event(KeyboardReleased), key_id(key), shift(shift_pressed),
        ctrl(ctrl_pressed), alt(alt_pressed) {}
  KeyCode key_id; /*!< Released key */
  bool shift;     /*!< Shift is pressed */
  bool ctrl;      /*!< Ctrl is pressed */
  bool alt;       /*!< Alt is pressed */
};

/**
 * @brief Event of time passing
 */
struct TickEvent : public Event {
  TickEvent(double seconds) : Event(Tick), delta_time(seconds) {}

  double delta_time; /*!< Time (in seconds) since last TickEvent */
};

} // namespace plug

#endif /* Event.h */
