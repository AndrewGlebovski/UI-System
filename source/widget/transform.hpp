/**
 * \file
 * \brief Contains transform and stack transform
*/


#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_


#include "common/list.hpp"
#include "standart/Math.h"


/// Contains stack of transforms and final transform at the top of it
class TransformStack : public plug::TransformStack {
private:
    List<plug::Transform> transforms;     ///< Stack of transforms

public:
    /**
     * \brief Init transform stack with transform that does nothing
    */
    TransformStack();

    /**
     * \brief Pushes last transform to stack and applies it to the stack top
    */
    virtual void enter(const plug::Transform &transform) override;

    /**
     * \brief Pops last transform from stack
    */
    virtual void leave() override;

    /**
     * \brief Returns value of the stack first element
    */
    virtual plug::Transform top() const override;

    /**
     * \brief Applies final transform to position
    */
    virtual plug::Vec2d apply(const plug::Vec2d &vec) const override;

    /**
     * \brief Restore position using final transform
    */
    virtual plug::Vec2d restore(const plug::Vec2d &vec) const override;
};


plug::Vec2d applySize(const plug::TransformStack &stack, const plug::Vec2d &vec);


/// Tool class for temporarily applying local transform
class TransformApplier {
private:
    plug::TransformStack &stack;     ///< Transform stack

public:
    /**
     * \brief Applies local transform and pushes it to the transform stack
    */
    TransformApplier(plug::TransformStack &stack_, const plug::Transform &transform);

    /**
     * \brief Cancels local transform and pops it from the transform stack
    */
    ~TransformApplier();
};


#endif
