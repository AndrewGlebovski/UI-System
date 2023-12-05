/**
 * \file
 * \brief Contains sources of widget class functions
*/


#include <cmath>
#include "widget.hpp"


// ============================================================================


Widget::Widget(size_t id_, const LayoutBox &layout_) :
    id(generateId(id_)),
    layout(layout_.clone()),
    parent(nullptr),
    status(Status::Normal)
{}


Widget::Widget(const Widget &widget) :
    id(AUTO_ID),
    layout(widget.getLayoutBox().clone()),
    parent(nullptr),
    status(Status::Normal)
{}


Widget &Widget::operator = (const Widget &widget) {
    if (this != &widget)
        layout = widget.getLayoutBox().clone();
    return *this;
}


size_t Widget::generateId(size_t requested_id) {
    if (requested_id != AUTO_ID) return requested_id;
    return size_t(this);
}


size_t Widget::getId() const { return id; }


LayoutBox &Widget::getLayoutBox() { return *layout; }


const LayoutBox &Widget::getLayoutBox() const { return *layout; }


void Widget::setLayoutBox(const LayoutBox &layout_) { layout = layout_.clone(); }


Transform Widget::getTransform() const { return Transform(layout->getPosition()); }


Widget *Widget::getParent() { return parent; }


const Widget *Widget::getParent() const { return parent; }


void Widget::setParent(Widget *parent_) {
    parent = parent_;

    if (parent)
        layout->onParentUpdate(parent->getLayoutBox());
}


Widget *Widget::findWidget(size_t widget_id) {
    return (widget_id == getId()) ? this : nullptr;
}


Widget::Status Widget::getStatus() const { return status; }


void Widget::setStatus(Status new_status) { status = new_status; }


void Widget::draw(TransformStack &stack, RenderTarget &result) {
#ifdef DEBUG_DRAW
    RectShape rect(
        stack.apply(layout->getPosition()),
        applySize(stack, layout->getSize()),
        Color(255, 0, 0)
    );
    rect.draw(result);
#endif
}


void Widget::onEvent(const Event &event, EHC &ehc) {
    switch(event.getType()) {
        case Tick: onTick(static_cast<const TickEvent&>(event), ehc); break;
        case MouseMove: onMouseMove(static_cast<const MouseMoveEvent&>(event), ehc); break;
        case MousePressed: onMousePressed(static_cast<const MousePressedEvent&>(event), ehc); break;
        case MouseReleased: onMouseReleased(static_cast<const MouseReleasedEvent&>(event), ehc); break;
        case KeyboardPressed: onKeyboardPressed(static_cast<const KeyboardPressedEvent&>(event), ehc); break;
        case KeyboardReleased: onKeyboardReleased(static_cast<const KeyboardReleasedEvent&>(event), ehc); break;
        default: printf("Uknown event type!\n"); abort();
    }
}


void Widget::onParentUpdate(const LayoutBox &parent_layout) {
    getLayoutBox().onParentUpdate(parent_layout);
}


bool Widget::covers(TransformStack &stack, const Vec2d &position) const {
    return isInsideRect(
        stack.apply(getLayoutBox().getPosition()),
        applySize(stack, getLayoutBox().getSize()),
        position
    );
}


Widget::~Widget() {
    delete layout;
}


// ============================================================================


bool isInsideRect(Vec2d position, Vec2d size, Vec2d point) {
    if (point.x < position.x) return false;
    if (point.x > position.x + size.x) return false;
    if (point.y < position.y) return false;
    if (point.y > position.y + size.y) return false;
    return true;
}
