/**
 * \file
 * \brief Contains transform and stack transform
*/


#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_


#include "common/vector.hpp"
#include "common/list.hpp"


/// Holds transformation
class Transform {
private:
    Vec2d offset;        ///< Offset from parent
    Vec2d scale;         ///< Scale

public:
    /**
     * \brief Creates transform that does nothing
    */
    Transform();

    /**
     * \brief Creates transform
    */
    Transform(const Vec2d &offset_, const Vec2d &scale_ = Vec2d(1, 1));

    /**
     * \brief Returns offset
    */
    Vec2d getOffset() const;

    /**
     * \brief Sets offset
    */
    void setOffset(const Vec2d &offset_);

    /**
     * \brief Returns scale
    */
    Vec2d getScale() const;

    /**
     * \brief Sets scale
    */
    void setScale(const Vec2d &scale_);

    /**
     * \brief Applies transform to position
    */
    Vec2d apply(const Vec2d &vec) const;

    /**
     * \brief Restores position using transform
    */
    Vec2d restore(const Vec2d &vec) const;

    /**
     * \brief Combines parent transform with this one
    */
    Transform combine(const Transform &parent_transform) const;
};


/// Contains stack of transforms and final transform at the top of it
class TransformStack {
private:
    List<Transform> transforms;     ///< Stack of transforms

public:
    /**
     * \brief Init transform stack with transform that does nothing
    */
    TransformStack();

    virtual ~TransformStack() = default;

    /**
     * \brief Pushes last transform to stack and applies it to the stack top
    */
    virtual void enter(const Transform &transform);

    /**
     * \brief Pops last transform from stack
    */
    virtual void leave();

    /**
     * \brief Returns value of the stack first element
    */
    virtual Transform top() const;

    /**
     * \brief Applies final transform to position
    */
    virtual Vec2d apply(const Vec2d &vec) const;

    /**
     * \brief Restore position using final transform
    */
    virtual Vec2d restore(const Vec2d &vec) const;
};


Vec2d applySize(const TransformStack &stack, const Vec2d &vec);


/// Tool class for temporarily applying local transform
class TransformApplier {
private:
    TransformStack &stack;     ///< Transform stack

public:
    /**
     * \brief Applies local transform and pushes it to the transform stack
    */
    TransformApplier(TransformStack &stack_, const Transform &transform);

    /**
     * \brief Cancels local transform and pops it from the transform stack
    */
    ~TransformApplier();
};


#endif
