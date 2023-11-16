/**
 * \file
 * \brief Contains sources of widget class functions
*/


#include <SFML/Graphics.hpp>
#include <cmath>
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


Transform Transform::combine(const Transform &parent_transform) const {
    return Transform(
        parent_transform.getOffset() + offset * parent_transform.getScale(),
        scale * parent_transform.getScale()
    );
}


// ============================================================================


TransformStack::TransformStack() : transforms() {
    transforms.push_back(Transform());
}


void TransformStack::enter(const Transform &transform) {
    transforms.push_back(transforms.back().combine(transform));
}


void TransformStack::leave() {
    transforms.pop_back();
}


Transform TransformStack::top() const {
    return transforms.back();
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


TransformApplier::TransformApplier(TransformStack &stack_, const Transform &transform) :
    stack(stack_) { stack.enter(transform); }


TransformApplier::~TransformApplier() { stack.leave(); }


// ============================================================================


LazyLayoutBox::LazyLayoutBox() : position(), size() {}


LazyLayoutBox::LazyLayoutBox(const Vec2d &position_, const Vec2d &size_) :
    position(position_), size(size_) {}


Vec2d LazyLayoutBox::getPosition() const {
    return position;
}


bool LazyLayoutBox::setPosition(const Vec2d& position_) {
    if (position == position_) return false;
    position = position_;
    return true;
}


Vec2d LazyLayoutBox::getSize() const {
    return size;
}


bool LazyLayoutBox::setSize(const Vec2d& size_) {
    if (size == size_) return false;
    size = size_;
    return true;
}


void LazyLayoutBox::onParentUpdate(const LayoutBox &parent_layout) {}


LayoutBox *LazyLayoutBox::clone() const {
    return new LazyLayoutBox(position, size);
}


// ============================================================================


BoundLayoutBox::BoundLayoutBox() : LazyLayoutBox(), bound(SCREEN_W, SCREEN_H) {}


BoundLayoutBox::BoundLayoutBox(const Vec2d &position_, const Vec2d &size_) :
    LazyLayoutBox(position_, size_), bound(SCREEN_W, SCREEN_H) {}


bool BoundLayoutBox::setPosition(const Vec2d& position_) {
    Vec2d prev_pos = position;
    position = position_;

    if (position.x < 0)
        position.x = 0;
    else if (position.x + size.x > bound.x)
        position.x = bound.x - size.x;

    if (position.y < 0)
        position.y = 0;
    else if (position.y + size.y > bound.y)
        position.y = bound.y - size.y;

    return !(position == prev_pos);
}


bool BoundLayoutBox::setSize(const Vec2d& size_) {
    Vec2d prev_size = size;
    size = size_;

    if (size.x < 0)
        size.x = 0;
    else if (size_.x + position.x > bound.x)
        size.x = bound.x - position.x;

    if (size.y < 0)
        size.y = 0;
    else if (size.y + position.y > bound.y)
        size.y = bound.y - position.y;

    return !(size == prev_size);
}


void BoundLayoutBox::onParentUpdate(const LayoutBox &parent_layout) {
    bound = parent_layout.getSize();

    if (position.x + size.x > bound.x) {
        if (size.x > bound.x) {
            position.x = 0;
            size.x = bound.x;
        }
        else position.x = bound.x - size.x;
    }

    if (position.y + size.y > bound.y) {
        if (size.y > bound.y) {
            position.y = 0;
            size.y = bound.y;
        }
        else position.y = bound.y - size.y;
    }
}


LayoutBox *BoundLayoutBox::clone() const { return new BoundLayoutBox(position, size); }


// ============================================================================


AnchorLayoutBox::AnchorLayoutBox(
    const Vec2d &offset_, const Vec2d &size_,
    const Vec2d &anchor_position_, const Vec2d &anchor_size_
) :
    LazyLayoutBox(Vec2d(), size_),
    offset(offset_), basic_size(size_),
    anchor_position(anchor_position_), anchor_size(anchor_size_)
{}


bool AnchorLayoutBox::setPosition(const Vec2d& offset_) {
    if (offset == offset_) return false;
    offset = offset_;
    return true;
}


bool AnchorLayoutBox::setSize(const Vec2d& basic_size_) {
    if (basic_size == basic_size_) return false;
    basic_size = basic_size_;
    return true;
}


void AnchorLayoutBox::onParentUpdate(const LayoutBox &parent_layout) {
    Vec2d bound = parent_layout.getSize();

    bool split_x = fabs(anchor_size.x) > 1e-5;
    bool split_y = fabs(anchor_size.y) > 1e-5;

    if (split_x) {
        double sum = SCREEN_W - anchor_size.x;
        size.x = ((bound.x - sum) / (SCREEN_W - sum)) * basic_size.x;
        position.x = anchor_position.x + offset.x;
    }
    else {
        position.x = (anchor_position.x / SCREEN_W) * bound.x + offset.x;
        size.x = basic_size.x;
    }
    if (split_y) {
        double sum = SCREEN_H - anchor_size.y;
        size.y = ((bound.y - sum) / (SCREEN_H - sum)) * basic_size.y;
        position.y = anchor_position.y + offset.y;
    }
    else {
        position.y = (anchor_position.y / SCREEN_H) * bound.y + offset.y;
        size.y = basic_size.y;
    }
}


LayoutBox *AnchorLayoutBox::clone() const {
    return new AnchorLayoutBox(offset, size, anchor_position, anchor_size);
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
#ifdef DEBUG_DRAW
    sf::RectangleShape rect(Vec2d(25, 25));
    rect.setFillColor(sf::Color::Red);
    rect.setPosition(stack.apply(layout->getPosition()));
    result.draw(rect);
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


// ============================================================================


bool isInsideRect(Vec2d position, Vec2d size, Vec2d point) {
    if (point.x < position.x) return false;
    if (point.x > position.x + size.x) return false;
    if (point.y < position.y) return false;
    if (point.y > position.y + size.y) return false;
    return true;
}
