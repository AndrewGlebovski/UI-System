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
#include "event.hpp"


/// Widget basic interface
class WidgetInterface {
public:
    virtual ~WidgetInterface() = default;

    virtual void draw(RenderTarget &result, TransformStack &stack) = 0;

    virtual void onEvent(const Event &event, EHC &ehc) = 0;

    virtual void onParentUpdate(const LayoutBox &parent_layout) = 0;

    virtual LayoutBox &getLayoutBox() = 0;

    virtual const LayoutBox &getLayoutBox() const = 0;

    virtual void setLayoutBox(const LayoutBox &layout_) = 0;

protected:
    virtual bool covers(TransformStack &stack, const Vec2d &position) const = 0;

    virtual void onTick(const TickEvent &event, EHC &ehc) {};
    
    virtual void onMouseMove(const MouseMoveEvent &event, EHC &ehc) {};
    
    virtual void onMousePressed(const MousePressedEvent &event, EHC &ehc) {};
    
    virtual void onMouseReleased(const MouseReleasedEvent &event, EHC &ehc) {};
    
    virtual void onKeyboardPressed(const KeyboardPressedEvent &event, EHC &ehc) {};
    
    virtual void onKeyboardReleased(const KeyboardReleasedEvent &event, EHC &ehc) {};
};


/// Extended widget interface with common implementation
class Widget : public WidgetInterface {
public:
    /// Pass into constructor to generate new ID
    static const size_t AUTO_ID = 0;

    /// Shows parent if some actions requiered for this widget
    enum class Status {
        Normal      = 0,    ///< Widget accepts events and draws
        Disabled    = 1,    ///< Widget draws but doesn't accept events
        Hidden      = 2,    ///< Widget accepts events but doesn't draw
        Pass        = 3,    ///< Widget doesn't accept events and doesn't draw
        Delete      = 4     ///< Widget will be deleted
    };

    /**
     * \brief Widget constructor
     * \note If id_ != AUTO_ID sets id to id_, otherwise generates "unique" id
    */
    Widget(size_t id_, const LayoutBox &layout_);

    /**
     * \brief Default copy constructor
     * \note id is AUTO, parent is set nullptr
    */
    Widget(const Widget &widget);

    /**
     * \brief Default assignment
     * \note Parent is set to nullptr
    */
    Widget &operator = (const Widget &widget);

    /**
     * \brief Returns widget id
    */
    size_t getId() const;

    /**
     * \brief Returns current layout box
    */
    virtual LayoutBox &getLayoutBox() override;

    /**
     * \brief Returns current layout box
    */
    virtual const LayoutBox &getLayoutBox() const override;

    /**
     * \brief Sets layout box
    */
    virtual void setLayoutBox(const LayoutBox &layout_) override;

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
     * \brief Returns widget status
    */
    Status getStatus() const;

    /**
     * \brief Sets widget status
    */
    void setStatus(Status new_status);

    /**
     * \brief Draws widget on render target
     * \note By default draws red rectangle for debug purposes
    */
    virtual void draw(RenderTarget &result, TransformStack &stack) override;

    /**
     * \brief Handle all sorts of events event
    */
    virtual void onEvent(const Event &event, EHC &ehc) override;

    /**
     * \brief Allows widget to change its position and size according to parent
    */
    virtual void onParentUpdate(const LayoutBox &parent_layout) override;

    /**
     * \brief Checks children statuses
     * \note By default does nothing
    */
    virtual void checkChildren() {}

    /**
     * \brief Delete layout box
    */
    virtual ~Widget() override;

protected:
    virtual bool covers(TransformStack &stack, const Vec2d &position) const override;

    /**
     * \brief If requested_id != AUTO_ID returns requested_id, otherwise returns unique id
    */
    size_t generateId(size_t requested_id);

    const size_t id;        ///< Widget ID that can be used for finding this widget in hierarchy
    LayoutBox *layout;      ///< Widget position and size encapsulated
    Widget *parent;         ///< Parent that holds this widget
    Status status;          ///< Shows parent if some actions requiered
};


/**
 * \brief Checks if point is inside rectangle
*/
bool isInsideRect(Vec2d position, Vec2d size, Vec2d point);


#endif
