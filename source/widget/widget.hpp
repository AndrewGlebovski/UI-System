/**
 * \file
 * \brief Contains widget class and protypes of his functions 
*/


#ifndef _WIDGET_H_
#define _WIDGET_H_


#include <cstdio>
#include "SFML/Graphics.hpp"
#include "config/configs.hpp"
#include "config/key_id.hpp"
#include "common/list.hpp"
#include "common/vector.hpp"
#include "widget/layout_box.hpp"
#include "widget/transform.hpp"
#include "render_target.hpp"
#include "shape.hpp"


/// Base class for all widgets
class Widget {
public:
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

    /// Pass into constructor to generate new ID
    static const size_t AUTO_ID = 0;


    /// Shows parent if some actions requiered for this widget
    enum WIDGET_STATUS {
        PASS        = 0,    ///< Nothing to be done for this widget
        DELETE      = 1     ///< This widget should be deleted
    };

    /**
     * \brief Widget constructor
     * \note If id_ != AUTO_ID sets id to id_, otherwise generates "unique" id
    */
    Widget(size_t id_, const LayoutBox &layout_);

    /**
     * \brief Default copy constructor
     * \note id is AUTO, z-index is set to 0, parent is set nullptr
    */
    Widget(const Widget &widget);

    /**
     * \brief Default assignment
     * \note z-index is set to 0, parent is set to nullptr
    */
    Widget &operator = (const Widget &widget);

    /**
     * \brief Returns widget id
    */
    size_t getId() const;

    /**
     * \brief Returns current layout box
    */
    LayoutBox &getLayoutBox();

    /**
     * \brief Returns current layout box
    */
    const LayoutBox &getLayoutBox() const;

    /**
     * \brief Sets layout box
    */
    void setLayoutBox(const LayoutBox &layout_);

    /**
     * \brief Generates transform using widget position
    */
    Transform getTransform() const;

    /**
     * \brief Returns parent
    */
    Widget *getParent();

    /**
     * \brief Returns parent
    */
    const Widget *getParent() const;

    /**
     * \brief Sets parent
    */
    void setParent(Widget *parent_);

    /**
     * \brief Searches widget in hierarchy using id
    */
    virtual Widget *findWidget(size_t widget_id);

    /**
     * \brief Adds child widget for this
     * \warning If widget is not supposed to have children, abort() will be called
    */
    virtual size_t addChild(Widget *child);

    /**
     * \brief Removes child by its id
     * \warning If widget is not supposed to have children, abort() will be called
    */
    virtual void removeChild(size_t child_id);

    /**
     * \brief Returns widget status
    */
    int getStatus() const;

    /**
     * \brief Sets widget status
    */
    void setStatus(WIDGET_STATUS new_status);

    /**
     * \brief Draws widget on render target
     * \note By default draws red rectangle for debug purposes
    */
    virtual void draw(RenderTarget &result, TransformStack &stack);

    /**
     * \brief Handle all sorts of events event
    */
    virtual void onEvent(const Event &event, EHC &ehc);

    /**
     * \brief Allows widget to change its position and size according to parent
    */
    virtual void onParentUpdate(const LayoutBox &parent_layout) { layout->onParentUpdate(parent_layout); }

    /**
     * \brief Checks children statuses
     * \note By default does nothing
    */
    virtual void checkChildren() {}

    /**
     * \brief Delete layout box
    */
    virtual ~Widget() { delete layout; };

protected:
    const size_t id;        ///< Widget ID that can be used for finding this widget in hierarchy
    LayoutBox *layout;      ///< Widget position and size encapsulated
    int z_index;            ///< Shows order in which widgets are drawn
    Widget *parent;         ///< Parent that holds this widget
    int status;             ///< Shows parent if some actions requiered

    /**
     * \brief If requested_id != AUTO_ID returns requested_id, otherwise returns unique id
    */
    size_t generateId(size_t requested_id);

    virtual void onTick(const TickEvent &event, EHC &ehc) {}
    virtual void onMouseMove(const MouseMoveEvent &event, EHC &ehc) {}
    virtual void onMousePressed(const MousePressedEvent &event, EHC &ehc) {}
    virtual void onMouseReleased(const MouseReleasedEvent &event, EHC &ehc) {}
    virtual void onKeyboardPressed(const KeyboardPressedEvent &event, EHC &ehc) {}
    virtual void onKeyboardReleased(const KeyboardReleasedEvent &event, EHC &ehc) {}
};


/**
 * \brief Checks if point is inside rectangle
*/
bool isInsideRect(Vec2d position, Vec2d size, Vec2d point);


#endif
