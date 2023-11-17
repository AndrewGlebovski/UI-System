/**
 * \file
 * \brief Contains sources of scrollbar and scrollbar action classes functions
*/


#include "basic/scrollbar.hpp"


ScrollBar::ScrollBar(
    size_t id_, const LayoutBox &layout_,
    ScrollAction *action_, const ScrollBarStyle &style_
) :
    Widget(id_, layout_),
    action(action_), style(style_), scroller(Vec2d()), 
    is_moving(false), mouse_prev(Vec2d()) 
{
    ASSERT(action, "Action is nullptr!\n");
}


void ScrollBar::draw(sf::RenderTarget &result, TransformStack &stack) {
    Vec2d global_position = stack.apply(layout->getPosition());
    Vec2d global_size = stack.apply_size(layout->getSize());
    
    sf::RectangleShape frame(global_size);
    frame.setFillColor(style.background_color);
    frame.setOutlineColor(style.frame_color);
    frame.setOutlineThickness(style.frame_outline);
    frame.setPosition(global_position);
    result.draw(frame);
    
    Vec2d scroller_offset = scroller.getPosition();

    scroller.setPosition(global_position + scroller_offset);
    result.draw(scroller);

    scroller.setPosition(scroller_offset);
}


void ScrollBar::onMouseMove(const MouseMoveEvent &event, EHC &ehc) {
    if (is_moving) {
        scrollTo(event.pos - mouse_prev);
        mouse_prev = event.pos;

        ehc.overlapped = true;
    }
}


void ScrollBar::onMousePressed(const MousePressedEvent &event, EHC &ehc) {
    Vec2d global_position = ehc.stack.apply(layout->getPosition());
    Vec2d global_size = ehc.stack.apply_size(layout->getSize());

    if (isInsideRect(global_position, global_size, event.pos)) {
        Vec2d scroller_absolute = global_position + scroller.getPosition();

        if (!isInsideRect(scroller_absolute, scroller.getSize(), event.pos))
            scrollTo(event.pos - (scroller_absolute + scroller.getSize() / 2));

        is_moving = true;
        mouse_prev = event.pos;

        ehc.stopped = true;
    }
}


void ScrollBar::onMouseReleased(const MouseReleasedEvent &event, EHC &ehc) {
    is_moving = false;
}


ScrollBar::~ScrollBar() {
    if (action) delete action;
}


VScrollBar::VScrollBar(
    size_t id_, const LayoutBox &layout_,
    ScrollAction *action_, const ScrollBarStyle &style_
) :
    ScrollBar(id_, layout_, action_, style_)
{
    Vec2d size = layout->getSize();

    scroller.setPosition(0, 0);
    scroller.setFillColor(style.scroller_color);
    scroller.setSize(Vec2d(size.x, style.scroller_factor * size.y));
}


void VScrollBar::scrollTo(const Vec2d &shift) {
    Vec2d size = layout->getSize();
    Vec2d new_position = scroller.getPosition() + Vec2d(0, shift.y);

    if (new_position.y < 0) new_position.y = 0;
    if (new_position.y + scroller.getSize().y > size.y) new_position.y = size.y - scroller.getSize().y;

    scroller.setPosition(new_position);

    (*action)(new_position.y / (size.y - scroller.getSize().y));
}


void VScrollBar::onParentUpdate(const LayoutBox &parent_layout) {
    double prev = scroller.getPosition().y / (layout->getSize().y - scroller.getSize().y);

    layout->onParentUpdate(parent_layout);

    scroller.setSize(Vec2d(layout->getSize().x, style.scroller_factor * layout->getSize().y));
    scroller.setPosition(Vec2d(scroller.getPosition().x, prev * (layout->getSize().y - scroller.getSize().y)));
}


HScrollBar::HScrollBar(
    size_t id_, const LayoutBox &layout_,
    ScrollAction *action_, const ScrollBarStyle &style_
) :
    ScrollBar(id_, layout_, action_, style_)
{
    Vec2d size = layout->getSize();

    scroller.setPosition(0, 0);
    scroller.setFillColor(style.scroller_color);
    scroller.setSize(Vec2d(style.scroller_factor * size.x, size.y));
}


void HScrollBar::scrollTo(const Vec2d &shift) {
    Vec2d size = layout->getSize();
    Vec2d new_position = scroller.getPosition() + Vec2d(shift.x, 0);

    if (new_position.x < 0) new_position.x = 0;
    if (new_position.x + scroller.getSize().x > size.x) new_position.x = size.x - scroller.getSize().x;

    scroller.setPosition(new_position);

    (*action)(new_position.x / (size.x - scroller.getSize().x));
}


void HScrollBar::onParentUpdate(const LayoutBox &parent_layout) {
    double prev = scroller.getPosition().x / (layout->getSize().x - scroller.getSize().x);

    layout->onParentUpdate(parent_layout);

    scroller.setSize(Vec2d(style.scroller_factor * layout->getSize().x, layout->getSize().y));
    scroller.setPosition(Vec2d(prev * (layout->getSize().x - scroller.getSize().x), scroller.getPosition().y));
}
