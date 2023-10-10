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
#include "style.hpp"
#include "ui-base.hpp"
#include "scrollbar.hpp"


VScrollBar::VScrollBar(
    const Vector2D &position_, const Vector2D &size_, int z_index_, BaseUI *parent_,
    ScrollAction *action_, const ScrollBarStyle &style_
) :
    BaseUI(position_, size_, z_index_, parent_),
    action(action_), style(style_), scroller(Vector2D()), 
    is_moving(false), mouse_prev(Vector2D()) 
{
    scroller.setPosition(0, 0);
    scroller.setFillColor(style.scroller_color);
    scroller.setSize(Vector2D(size.x, style.scroller_factor * size.y));
}


void VScrollBar::draw(sf::RenderTexture &result, List<Vector2D> &transforms) {
    TransformApplier add_transform(transforms, position);
    
    sf::RectangleShape frame(size);
    frame.setFillColor(style.background_color);
    frame.setOutlineColor(style.frame_color);
    frame.setOutlineThickness(style.frame_outline);
    frame.setPosition(transforms[0]);
    result.draw(frame);
    
    Vector2D scroller_offset = scroller.getPosition();

    scroller.setPosition(transforms[0] + scroller_offset);
    result.draw(scroller);

    scroller.setPosition(scroller_offset);
}


int VScrollBar::onMouseMove(int mouse_x, int mouse_y, List<Vector2D> &transforms) {
    TransformApplier add_transform(transforms, position);

    Vector2D mouse(mouse_x, mouse_y);

    if (is_moving) {
        Vector2D new_position = scroller.getPosition() + Vector2D(0, mouse.y - mouse_prev.y);

        if (new_position.y < 0) new_position.y = 0;
        if (new_position.y + scroller.getSize().y > size.y) new_position.y = size.y - scroller.getSize().y;

        scroller.setPosition(new_position);

        (*action)(new_position.y / (size.y - scroller.getSize().y));

        mouse_prev = mouse;

        return HANDLED;
    }

    return UNHANDLED;
}


int VScrollBar::onMouseButtonDown(int mouse_x, int mouse_y, int button_id, List<Vector2D> &transforms) {
    TransformApplier add_transform(transforms, position);

    Vector2D mouse(mouse_x, mouse_y);

    if (isInsideRect(transforms[0], size, mouse)) {
        if (isInsideRect(transforms[0] + scroller.getPosition(), scroller.getSize(), mouse)) {
            is_moving = true;
            mouse_prev = mouse;
        }

        return HANDLED;
    }

    return UNHANDLED;
}


int VScrollBar::onMouseButtonUp(int mouse_x, int mouse_y, int button_id, List<Vector2D> &transforms) {
    is_moving = false;
    return UNHANDLED;
}


VScrollBar::~VScrollBar() {
    if (action) delete action;
}


HScrollBar::HScrollBar(
    const Vector2D &position_, const Vector2D &size_, int z_index_, BaseUI *parent_,
    ScrollAction *action_, const ScrollBarStyle &style_
) :
    BaseUI(position_, size_, z_index_, parent_),
    action(action_), style(style_), scroller(Vector2D()), 
    is_moving(false), mouse_prev(Vector2D()) 
{
    scroller.setPosition(0, 0);
    scroller.setFillColor(style.scroller_color);
    scroller.setSize(Vector2D(style.scroller_factor * size.x, size.y));
}


void HScrollBar::draw(sf::RenderTexture &result, List<Vector2D> &transforms) {
    TransformApplier add_transform(transforms, position);
    
    sf::RectangleShape frame(size);
    frame.setFillColor(style.background_color);
    frame.setOutlineColor(style.frame_color);
    frame.setOutlineThickness(style.frame_outline);
    frame.setPosition(transforms[0]);
    result.draw(frame);
    
    Vector2D scroller_offset = scroller.getPosition();

    scroller.setPosition(transforms[0] + scroller_offset);
    result.draw(scroller);

    scroller.setPosition(scroller_offset);
}


int HScrollBar::onMouseMove(int mouse_x, int mouse_y, List<Vector2D> &transforms) {
    TransformApplier add_transform(transforms, position);

    Vector2D mouse(mouse_x, mouse_y);

    if (is_moving) {
        Vector2D new_position = scroller.getPosition() + Vector2D(mouse.x - mouse_prev.x, 0);

        if (new_position.x < 0) new_position.x = 0;
        if (new_position.x + scroller.getSize().x > size.x) new_position.x = size.x - scroller.getSize().x;

        scroller.setPosition(new_position);

        (*action)(new_position.x / (size.x - scroller.getSize().x));

        mouse_prev = mouse;

        return HANDLED;
    }

    return UNHANDLED;
}


int HScrollBar::onMouseButtonDown(int mouse_x, int mouse_y, int button_id, List<Vector2D> &transforms) {
    TransformApplier add_transform(transforms, position);

    Vector2D mouse(mouse_x, mouse_y);

    if (isInsideRect(transforms[0], size, mouse)) {
        if (isInsideRect(transforms[0] + scroller.getPosition(), scroller.getSize(), mouse)) {
            is_moving = true;
            mouse_prev = mouse;
        }

        return HANDLED;
    }

    return UNHANDLED;
}


int HScrollBar::onMouseButtonUp(int mouse_x, int mouse_y, int button_id, List<Vector2D> &transforms) {
    is_moving = false;
    return UNHANDLED;
}


HScrollBar::~HScrollBar() {
    if (action) delete action;
}


#pragma GCC diagnostic pop
