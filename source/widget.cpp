/**
 * \file
 * \brief Contains sources of widget class functions
*/


#include <SFML/Graphics.hpp>
#include "vector.hpp"
#include "configs.hpp"
#include "list.hpp"
#include "widget.hpp"


// ============================================================================


Transform::Transform() :
    offset(Vec2d()), scale(Vec2d(1, 1)) {}


Transform::Transform(const Vec2d &offset_, const Vec2d &scale_) :
    offset(offset_), scale(scale_) {}


Vec2d Transform::getOffset() const { return offset; }


void Transform::setOffset(const Vec2d &offset_) { offset = offset_; }


Vec2d Transform::getScale() const { return scale; }


void Transform::setScale(const Vec2d &scale_) { scale = scale_; }


Vec2d Transform::apply(const Vec2d &vec) const {
    return offset + vec * scale;
}


Vec2d Transform::restore(const Vec2d &vec) const {
    return (vec - offset) / scale;
}


// ============================================================================


TransformStack::TransformStack() : transforms() {
    transforms.push_back(Transform());
}


void TransformStack::enter(const Transform &transform) {
    transforms.push_back(transform);

    transforms.front().setOffset(transforms.front().getOffset() + transform.getOffset());
    transforms.front().setScale(transforms.front().getScale() * transform.getScale());
}


void TransformStack::leave() {
    Transform back = transforms.back();

    transforms.front().setOffset(transforms.front().getOffset() - back.getOffset());
    transforms.front().setScale(transforms.front().getScale() / back.getScale());

    transforms.pop_back();
}


Transform TransformStack::top() const {
    return transforms.front();
}


Vec2d TransformStack::apply(const Vec2d &vec) const {
    return top().apply(vec);
}


Vec2d TransformStack::restore(const Vec2d &vec) const {
    return top().restore(vec);
}


Vec2d TransformStack::apply_size(const Vec2d &vec) const {
    return vec * top().getScale();
}


// ============================================================================


TransformApplier::TransformApplier(TransformStack &stack_, const Transform &transform) : stack(stack_)
{ stack.enter(transform); }


TransformApplier::~TransformApplier() { stack.leave(); }


// ============================================================================


BasicLayoutBox::BasicLayoutBox() : position(), size(), bounds(SCREEN_W, SCREEN_H) {}


BasicLayoutBox::BasicLayoutBox(const Vec2d &position_, const Vec2d &size_) :
    position(position_), size(size_), bounds(SCREEN_W, SCREEN_H) {}


Vec2d BasicLayoutBox::getPosition() const { return position; }


bool BasicLayoutBox::setPosition(const Vec2d& position_) {
    Vec2d prev_pos = position;
    position = position_;

    if (position.x < 0)
        position.x = 0;
    else if (position.x + size.x > bounds.x)
        position.x = bounds.x - size.x;

    if (position.y < 0)
        position.y = 0;
    else if (position.y + size.y > bounds.y)
        position.y = bounds.y - size.y;

    return !(position == prev_pos);
}


Vec2d BasicLayoutBox::getSize() const { return size; }


bool BasicLayoutBox::setSize(const Vec2d& size_) {
    Vec2d prev_size = size;
    size = size_;

    if (size.x < 0)
        size.x = 0;
    else if (size_.x + position.x > bounds.x)
        size.x = bounds.x - position.x;

    if (size.y < 0)
        size.y = 0;
    else if (size.y + position.y > bounds.y)
        size.y = bounds.y - position.y;

    return !(size == prev_size);
}


void BasicLayoutBox::onParentUpdate(const LayoutBox &parent_layout) {
    bounds = parent_layout.getSize();

    if (position.x + size.x > bounds.x) {
        if (size.x > bounds.x) {
            position.x = 0;
            size.x = bounds.x;
        }
        else position.x = bounds.x - size.x;
    }

    if (position.y + size.y > bounds.y) {
        if (size.y > bounds.y) {
            position.y = 0;
            size.y = bounds.y;
        }
        else position.y = bounds.y - size.y;
    }
}


LayoutBox *BasicLayoutBox::clone() const { return new BasicLayoutBox(position, size); }


// ============================================================================


OffsetLayoutBox::OffsetLayoutBox(const Vec2d &offset_, const Vec2d &origin_, const Vec2d &size_) :
    BasicLayoutBox(Vec2d(), size_), offset(offset_), origin(origin_) {}


void OffsetLayoutBox::onParentUpdate(const LayoutBox &parent_layout) {
    bounds = parent_layout.getSize();

    position = origin * bounds + offset;
}


LayoutBox *OffsetLayoutBox::clone() const {
    return new OffsetLayoutBox(offset, origin, size);
}


// ============================================================================


Widget::Widget(size_t id_, const LayoutBox &layout_) :
    id(generateId(id_)),
    layout(layout_.clone()),
    z_index(0),
    parent(nullptr),
    status(PASS)
{}


Widget::Widget(const Widget &widget) :
    id(AUTO_ID),
    layout(widget.getLayoutBox().clone()),
    z_index(0),
    parent(nullptr),
    status(PASS)
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


int Widget::getZIndex() const { return z_index; }


void Widget::setZIndex(int z_index_) { z_index = z_index_; }


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


size_t Widget::addChild(Widget *child) {
    printf("This widget does not support children!\n");
    abort();
}


void Widget::removeChild(size_t child_id) {
    printf("This widget does not support children!\n");
    abort();
}


int Widget::getStatus() const { return status; }


void Widget::setStatus(WIDGET_STATUS new_status) { status = new_status; }


void Widget::draw(sf::RenderTarget &result, TransformStack &stack) {
    /* DEBUG DRAWING
    sf::RectangleShape rect(Vec2d(25, 25));
    rect.setFillColor(sf::Color::Red);
    rect.setPosition(stack.apply(layout->getPosition()));
    result.draw(rect);
    */
}


// ============================================================================


bool isInsideRect(Vec2d position, Vec2d size, Vec2d point) {
    if (point.x < position.x) return false;
    if (point.x > position.x + size.x) return false;
    if (point.y < position.y) return false;
    if (point.y > position.y + size.y) return false;
    return true;
}
