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
    size_t id_, const Transform &transform_, const Vector2D &size_, int z_index_, Widget *parent_,
    ScrollAction *action_, const ScrollBarStyle &style_
) :
    Widget(id_, transform_, size_, z_index_, parent_),
    action(action_), style(style_), scroller(Vector2D()), 
    is_moving(false), mouse_prev(Vector2D()) 
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
    
    Vector2D scroller_offset = scroller.getPosition();

    scroller.setPosition(transforms.front().offset + scroller_offset);
    result.draw(scroller);

    scroller.setPosition(scroller_offset);
}


EVENT_STATUS ScrollBar::onMouseMove(int mouse_x, int mouse_y, List<Transform> &transforms) {
    if (is_moving) {
        Vector2D mouse(mouse_x, mouse_y);

        scrollTo(mouse - mouse_prev);
        mouse_prev = mouse;

        return HANDLED;
    }

    return UNHANDLED;
}


EVENT_STATUS ScrollBar::onMouseButtonDown(int mouse_x, int mouse_y, int button_id, List<Transform> &transforms) {
    TransformApplier add_transform(transforms, transform);

    Vector2D mouse(mouse_x, mouse_y);

    if (isInsideRect(transforms.front().offset, size, mouse)) {
        Vector2D scroller_absolute = transforms.front().offset + scroller.getPosition();

        if (!isInsideRect(scroller_absolute, scroller.getSize(), mouse))
            scrollTo(mouse - (scroller_absolute + scroller.getSize() / 2));

        is_moving = true;
        mouse_prev = mouse;

        return HANDLED;
    }

    return UNHANDLED;
}


EVENT_STATUS ScrollBar::onMouseButtonUp(int mouse_x, int mouse_y, int button_id, List<Transform> &transforms) {
    is_moving = false;
    return UNHANDLED;
}


ScrollBar::~ScrollBar() {
    if (action) delete action;
}


VScrollBar::VScrollBar(
    size_t id_, const Transform &transform_, const Vector2D &size_, int z_index_, Widget *parent_,
    ScrollAction *action_, const ScrollBarStyle &style_
) :
    ScrollBar(id_, transform_, size_, z_index_, parent_, action_, style_)
{
    scroller.setPosition(0, 0);
    scroller.setFillColor(style.scroller_color);
    scroller.setSize(Vector2D(size.x, style.scroller_factor * size.y));
}


void VScrollBar::scrollTo(const Vector2D &shift) {
    Vector2D new_position = scroller.getPosition() + Vector2D(0, shift.y);

    if (new_position.y < 0) new_position.y = 0;
    if (new_position.y + scroller.getSize().y > size.y) new_position.y = size.y - scroller.getSize().y;

    scroller.setPosition(new_position);

    (*action)(new_position.y / (size.y - scroller.getSize().y));
}


EVENT_STATUS VScrollBar::onParentResize() {
    float prev = scroller.getPosition().y / (size.y - scroller.getSize().y);

    tryTransform(Transform(Vector2D(parent->size.x - 20, transform.offset.y)));
    tryResize(Vector2D(size.x, parent->size.y - 30));

    scroller.setSize(Vector2D(size.x, style.scroller_factor * size.y));
    scroller.setPosition(Vector2D(scroller.getPosition().x, prev * (size.y - scroller.getSize().y)));

    return UNHANDLED;
}


HScrollBar::HScrollBar(
    size_t id_, const Transform &transform_, const Vector2D &size_, int z_index_, Widget *parent_,
    ScrollAction *action_, const ScrollBarStyle &style_
) :
    ScrollBar(id_, transform_, size_, z_index_, parent_, action_, style_)
{
    scroller.setPosition(0, 0);
    scroller.setFillColor(style.scroller_color);
    scroller.setSize(Vector2D(style.scroller_factor * size.x, size.y));
}


void HScrollBar::scrollTo(const Vector2D &shift) {
    Vector2D new_position = scroller.getPosition() + Vector2D(shift.x, 0);

    if (new_position.x < 0) new_position.x = 0;
    if (new_position.x + scroller.getSize().x > size.x) new_position.x = size.x - scroller.getSize().x;

    scroller.setPosition(new_position);

    (*action)(new_position.x / (size.x - scroller.getSize().x));
}


EVENT_STATUS HScrollBar::onParentResize() {
    float prev = scroller.getPosition().x / (size.x - scroller.getSize().x);

    tryTransform(Transform(Vector2D(transform.offset.x, parent->size.y - 20)));
    tryResize(Vector2D(parent->size.x, size.y));

    scroller.setSize(Vector2D(style.scroller_factor * size.x, size.y));
    scroller.setPosition(Vector2D(prev * (size.x - scroller.getSize().x), scroller.getPosition().y));

    return UNHANDLED;
}


#pragma GCC diagnostic pop
