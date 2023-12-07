/**
 * \file
 * \brief Contains sources of transform and transform stack functions
*/


#include "widget/transform.hpp"


// ============================================================================


TransformStack::TransformStack() : transforms() {
    transforms.push_back(plug::Transform());
}


void TransformStack::enter(const plug::Transform &transform) {
    transforms.push_back(transform.combine(top()));
}


void TransformStack::leave() {
    transforms.pop_back();
}


plug::Transform TransformStack::top() const {
    return transforms.back();
}


plug::Vec2d TransformStack::apply(const plug::Vec2d &vec) const {
    return top().apply(vec);
}


plug::Vec2d TransformStack::restore(const plug::Vec2d &vec) const {
    return top().restore(vec);
}


// ============================================================================


plug::Vec2d applySize(const plug::TransformStack &stack, const plug::Vec2d &vec) {
    return vec * stack.top().getScale();
}


// ============================================================================


TransformApplier::TransformApplier(plug::TransformStack &stack_, const plug::Transform &transform) :
    stack(stack_) { stack.enter(transform); }


TransformApplier::~TransformApplier() { stack.leave(); }
