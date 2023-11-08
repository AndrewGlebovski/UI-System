/**
 * \file
 * \brief Contains sources of scrollbar and scrollbar action classes functions
*/


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"


#include <SFML/Graphics.hpp>
#include "vector.hpp"
#include "list.hpp"
#include "asset.hpp"
#include "widget.hpp"
#include "scrollbar.hpp"


ScrollBar::ScrollBar(
    size_t id_, const Transform &transform_, const Vec2d &size_, int z_index_, Widget *parent_,
    ScrollAction *action_, const ScrollBarStyle &style_
) :
    Widget(id_, transform_, size_, z_index_, parent_),
    action(action_), style(style_), scroller(Vec2d()), 
    is_moving(false), mouse_prev(Vec2d()) 
{
    ASSERT(action, "Action is nullptr!\n");
}


void ScrollBar::draw(sf::RenderTarget &result, List<Transform> &transforms) {
    TransformApplier add_transform(transforms, transform);
    
    sf::RectangleShape frame(size);
    frame.setFillColor(style.background_color);
    frame.setOutlineColor(style.frame_color);
    frame.setOutlineThickness(style.frame_outline);
    frame.setPosition(transforms.front().offset);
    result.draw(frame);
    
    Vec2d scroller_offset = scroller.getPosition();

    scroller.setPosition(transforms.front().offset + scroller_offset);
    result.draw(scroller);

    scroller.setPosition(scroller_offset);
}


EVENT_STATUS ScrollBar::onMouseMove(const Vec2d &mouse, List<Transform> &transforms) {
    if (is_moving) {
        scrollTo(mouse - mouse_prev);
        mouse_prev = mouse;

        return HANDLED;
    }

    return UNHANDLED;
}


EVENT_STATUS ScrollBar::onMouseButtonDown(const Vec2d &mouse, int button_id, List<Transform> &transforms) {
    TransformApplier add_transform(transforms, transform);

    if (isInsideRect(transforms.front().offset, size, mouse)) {
        Vec2d scroller_absolute = transforms.front().offset + scroller.getPosition();

        if (!isInsideRect(scroller_absolute, scroller.getSize(), mouse))
            scrollTo(mouse - (scroller_absolute + scroller.getSize() / 2));

        is_moving = true;
        mouse_prev = mouse;

        return HANDLED;
    }

    return UNHANDLED;
}


EVENT_STATUS ScrollBar::onMouseButtonUp(const Vec2d &mouse, int button_id, List<Transform> &transforms) {
    is_moving = false;
    return UNHANDLED;
}


ScrollBar::~ScrollBar() {
    if (action) delete action;
}


VScrollBar::VScrollBar(
    size_t id_, const Transform &transform_, const Vec2d &size_, int z_index_, Widget *parent_,
    ScrollAction *action_, const ScrollBarStyle &style_
) :
    ScrollBar(id_, transform_, size_, z_index_, parent_, action_, style_)
{
    scroller.setPosition(0, 0);
    scroller.setFillColor(style.scroller_color);
    scroller.setSize(Vec2d(size.x, style.scroller_factor * size.y));
}


void VScrollBar::scrollTo(const Vec2d &shift) {
    Vec2d new_position = scroller.getPosition() + Vec2d(0, shift.y);

    if (new_position.y < 0) new_position.y = 0;
    if (new_position.y + scroller.getSize().y > size.y) new_position.y = size.y - scroller.getSize().y;

    scroller.setPosition(new_position);

    (*action)(new_position.y / (size.y - scroller.getSize().y));
}


EVENT_STATUS VScrollBar::onParentResize() {
    double prev = scroller.getPosition().y / (size.y - scroller.getSize().y);

    tryTransform(Transform(Vec2d(parent->size.x - 20, transform.offset.y)));
    tryResize(Vec2d(size.x, parent->size.y - 30));

    scroller.setSize(Vec2d(size.x, style.scroller_factor * size.y));
    scroller.setPosition(Vec2d(scroller.getPosition().x, prev * (size.y - scroller.getSize().y)));

    return UNHANDLED;
}


HScrollBar::HScrollBar(
    size_t id_, const Transform &transform_, const Vec2d &size_, int z_index_, Widget *parent_,
    ScrollAction *action_, const ScrollBarStyle &style_
) :
    ScrollBar(id_, transform_, size_, z_index_, parent_, action_, style_)
{
    scroller.setPosition(0, 0);
    scroller.setFillColor(style.scroller_color);
    scroller.setSize(Vec2d(style.scroller_factor * size.x, size.y));
}


void HScrollBar::scrollTo(const Vec2d &shift) {
    Vec2d new_position = scroller.getPosition() + Vec2d(shift.x, 0);

    if (new_position.x < 0) new_position.x = 0;
    if (new_position.x + scroller.getSize().x > size.x) new_position.x = size.x - scroller.getSize().x;

    scroller.setPosition(new_position);

    (*action)(new_position.x / (size.x - scroller.getSize().x));
}


EVENT_STATUS HScrollBar::onParentResize() {
    double prev = scroller.getPosition().x / (size.x - scroller.getSize().x);

    tryTransform(Transform(Vec2d(transform.offset.x, parent->size.y - 20)));
    tryResize(Vec2d(parent->size.x, size.y));

    scroller.setSize(Vec2d(style.scroller_factor * size.x, size.y));
    scroller.setPosition(Vec2d(prev * (size.x - scroller.getSize().x), scroller.getPosition().y));

    return UNHANDLED;
}


#pragma GCC diagnostic pop
