/**
 * \file
 * \brief Contains sources of widget class functions
*/


#include <cmath>
#include "widget.hpp"


// ============================================================================


Widget::Widget(size_t id_, const plug::LayoutBox &layout_) :
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


plug::LayoutBox &Widget::getLayoutBox() { return *layout; }


const plug::LayoutBox &Widget::getLayoutBox() const { return *layout; }


void Widget::setLayoutBox(const plug::LayoutBox &layout_) { layout = layout_.clone(); }


plug::Transform Widget::getTransform() const { return plug::Transform(layout->getPosition()); }


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


void Widget::draw(plug::TransformStack &stack, plug::RenderTarget &result) {
#ifdef DEBUG_DRAW
    RectShape rect(
        stack.apply(layout->getPosition()),
        applySize(stack, layout->getSize()),
        plug::Color(255, 0, 0)
    );
    rect.draw(result);
#endif
}


void Widget::onEvent(const plug::Event &event, plug::EHC &ehc) {
    switch(event.getType()) {
        case plug::Tick:
            onTick(static_cast<const plug::TickEvent&>(event), ehc);
            break;
        case plug::MouseMove:
            onMouseMove(static_cast<const plug::MouseMoveEvent&>(event), ehc);
            break;
        case plug::MousePressed:
            onMousePressed(static_cast<const plug::MousePressedEvent&>(event), ehc);
            break;
        case plug::MouseReleased:
            onMouseReleased(static_cast<const plug::MouseReleasedEvent&>(event), ehc);
            break;
        case plug::KeyboardPressed:
            onKeyboardPressed(static_cast<const plug::KeyboardPressedEvent&>(event), ehc);
            break;
        case plug::KeyboardReleased:
            onKeyboardReleased(static_cast<const plug::KeyboardReleasedEvent&>(event), ehc);
            break;
        default:
            printf("Uknown event type!\n");
            abort();
    }
}


void Widget::onParentUpdate(const plug::LayoutBox &parent_layout) {
    getLayoutBox().onParentUpdate(parent_layout);
}


bool Widget::covers(plug::TransformStack &stack, const plug::Vec2d &position) const {
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


bool isInsideRect(plug::Vec2d position, plug::Vec2d size, plug::Vec2d point) {
    if (point.x < position.x) return false;
    if (point.x > position.x + size.x) return false;
    if (point.y < position.y) return false;
    if (point.y > position.y + size.y) return false;
    return true;
}
