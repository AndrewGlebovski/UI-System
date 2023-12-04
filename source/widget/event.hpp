/**
 * \file
 * \brief Contains event structures
*/


#ifndef _EVENT_H_
#define _EVENT_H_


#include "widget/transform.hpp"
#include "config/key_id.hpp"


struct EHC {
    TransformStack &stack;
    bool stopped;
    bool overlapped;

    EHC(TransformStack &stack_, bool stopped_, bool overlapped_) :
        stack(stack_), stopped(stopped_), overlapped(overlapped_) {}
};


enum EventType {
    Tick,
    MouseMove,
    MousePressed,
    MouseReleased,
    KeyboardPressed,
    KeyboardReleased,
};


class Event {
private:
    const size_t type;
public:
    Event (size_t type_) : type(type_) {}

    size_t getType() const { return type; }
};


struct TickEvent : public Event {
    double delta_time;

    TickEvent(double delta_time_) :
        Event(Tick), delta_time(delta_time_) {}
};


struct MouseMoveEvent : public Event {
    bool shift, ctrl, alt;
    Vec2d pos;

    MouseMoveEvent(bool shift_, bool ctrl_, bool alt_, const Vec2d &pos_) :
        Event(MouseMove), shift(shift_), ctrl(ctrl_), alt(alt_), pos(pos_) {}
};


struct MousePressedEvent : public Event {
    MOUSE_BUTTON_ID button_id;
    bool shift, ctrl, alt;
    Vec2d pos;

    MousePressedEvent(MOUSE_BUTTON_ID button_id_, bool shift_, bool ctrl_, bool alt_, const Vec2d &pos_) :
        Event(MousePressed), button_id(button_id_), shift(shift_), ctrl(ctrl_), alt(alt_), pos(pos_) {}
};


struct MouseReleasedEvent : public Event {
    MOUSE_BUTTON_ID button_id;
    bool shift, ctrl, alt;
    Vec2d pos;

    MouseReleasedEvent(MOUSE_BUTTON_ID button_id_, bool shift_, bool ctrl_, bool alt_, const Vec2d &pos_) :
        Event(MouseReleased), button_id(button_id_), shift(shift_), ctrl(ctrl_), alt(alt_), pos(pos_) {}
};


struct KeyboardPressedEvent : public Event {
    KEY_ID key_id;
    bool shift, ctrl, alt;

    KeyboardPressedEvent(KEY_ID key_id_, bool shift_, bool ctrl_, bool alt_) :
        Event(KeyboardPressed), key_id(key_id_), shift(shift_), ctrl(ctrl_), alt(alt_) {}
};


struct KeyboardReleasedEvent : public Event {
    KEY_ID key_id;
    bool shift, ctrl, alt;

    KeyboardReleasedEvent(KEY_ID key_id_, bool shift_, bool ctrl_, bool alt_) :
        Event(KeyboardReleased), key_id(key_id_), shift(shift_), ctrl(ctrl_), alt(alt_) {}
};


#endif
