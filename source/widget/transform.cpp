/**
 * \file
 * \brief Contains sources of transform and transform stack functions
*/


#include "widget/transform.hpp"


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
    transforms.push_back(transform.combine(top()));
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


// ============================================================================


Vec2d applySize(const TransformStack &stack, const Vec2d &vec) {
    return vec * stack.top().getScale();
}


// ============================================================================


TransformApplier::TransformApplier(TransformStack &stack_, const Transform &transform) :
    stack(stack_) { stack.enter(transform); }


TransformApplier::~TransformApplier() { stack.leave(); }
