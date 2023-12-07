/**
 * \file
 * \brief Contains scrollbar and scrollbar action classes and protypes of their functions 
*/


#ifndef _SCROLLBAR_H_
#define _SCROLLBAR_H_


#include "widget/widget.hpp"


/// Triggers when scroller moves
class ScrollAction {
public:
    /**
     * \brief Calls when scroller moves
     * \param [in]  param   Real number from [0;1] respreseting scroller position
    */
    virtual void operator () (double param) = 0;


    /**
     * \brief Default destructor
    */
    virtual ~ScrollAction() = default;
};


/// Contains scroll bar style
class ScrollBarStyle {
public:
    plug::Color frame_color;          ///< Frame color
    float frame_outline;        ///< Frame outline thickness
    plug::Color background_color;     ///< Background color
    plug::Color scroller_color;       ///< Scroller
    double scroller_factor;      ///< Scroller height = factor * scrollbar.size

    ScrollBarStyle(
        plug::Color frame_color_, float frame_outline_,
        plug::Color background_color_,
        plug::Color scroller_color_, double scroller_factor_
    ) :
        frame_color(frame_color_), frame_outline(frame_outline_),
        background_color(background_color_),
        scroller_color(scroller_color_), scroller_factor(scroller_factor_) 
    {}
};


/// Base class for scroll bars
class ScrollBar : public Widget {
public:
    ScrollBar(
        size_t id_, const plug::LayoutBox &layout_,
        ScrollAction *action_, const ScrollBarStyle &style_
    );

    ScrollBar(const ScrollBar &scrollbar) = delete;
    ScrollBar &operator = (const ScrollBar& scrollbar) = delete;

    /**
     * \brief Draws background, border and scroller
    */
    virtual void draw(plug::TransformStack &stack, plug::RenderTarget &result) override;

    /**
     * \brief Deletes scroll action
    */
    virtual ~ScrollBar() override;

protected:
    ScrollAction *action;               ///< Triggers when scroller moves
    ScrollBarStyle style;               ///< Style
    RectShape scroller;                 ///< Rectangle that represents scroller
    bool is_moving;                     ///< Scroller is moving right now
    plug::Vec2d mouse_prev;                   ///< Previous mouse click position

    /**
     * \brief Sets scroller position and calls scroll action
    */
    virtual void scrollTo(const plug::Vec2d &shift) = 0;

    virtual void onMouseMove(const plug::MouseMoveEvent &event, plug::EHC &ehc) override;
    virtual void onMousePressed(const plug::MousePressedEvent &event, plug::EHC &ehc) override;
    virtual void onMouseReleased(const plug::MouseReleasedEvent &event, plug::EHC &ehc) override;
};


/// Can be scrolled up and down
class VScrollBar : public ScrollBar {
protected:
    virtual void scrollTo(const plug::Vec2d &shift) override;

public:
    VScrollBar(
        size_t id_, const plug::LayoutBox &layout_,
        ScrollAction *action_, const ScrollBarStyle &style_
    );

    virtual void onParentUpdate(const plug::LayoutBox &parent_layout) override;
};


/// Can be scrolled left and right
class HScrollBar : public ScrollBar {
protected:
    virtual void scrollTo(const plug::Vec2d &shift) override;

public:
    HScrollBar(
        size_t id_, const plug::LayoutBox &layout_,
        ScrollAction *action_, const ScrollBarStyle &style_
    );

    virtual void onParentUpdate(const plug::LayoutBox &parent_layout) override;
};


#endif
