/**
 * \file
 * \brief Contains layout box interface and basic implementations
*/


#ifndef _LAYOUT_BOX_H_
#define _LAYOUT_BOX_H_


#include "standart/LayoutBox.h"


/// Simply stores given position and size
class LazyLayoutBox : public plug::LayoutBox {
public:
    LazyLayoutBox();

    LazyLayoutBox(const plug::Vec2d &position_, const plug::Vec2d &size_);
    
    virtual plug::Vec2d getPosition() const override;

    virtual bool setPosition(const plug::Vec2d& position_) override;

    virtual plug::Vec2d getSize() const override;

    virtual bool setSize(const plug::Vec2d& size_) override;

    virtual void onParentUpdate(const plug::LayoutBox &parent_layout) override;

    virtual plug::LayoutBox *clone() const override;

protected:
    plug::Vec2d position;         ///< Widget position
    plug::Vec2d size;             ///< Widget size
};


/// Respects parent's size and can't go outside of it
class BoundLayoutBox : public LazyLayoutBox {
public:
    /**
     * \brief Zero position and size
     * \note Bound is set to SCREEN SIZE
    */
    BoundLayoutBox();

    /**
     * \brief Sets position and size
     * \note Bound is set to SCREEN SIZE
    */
    BoundLayoutBox(const plug::Vec2d &position_, const plug::Vec2d &size_);

    /**
     * \brief Sets position according to bound
    */
    virtual bool setPosition(const plug::Vec2d& position_) override;

    /**
     * \brief Sets size according to bound
    */
    virtual bool setSize(const plug::Vec2d& size_) override;

    /**
     * \brief Updates bound and changes size and position according to new bound
    */
    virtual void onParentUpdate(const plug::LayoutBox &parent_layout) override;

    /**
     * \brief Clones position and size but not bounds
    */
    virtual plug::LayoutBox *clone() const override;

protected:
    plug::Vec2d bound;           ///< Parent size
};


/// Anchor implementation
class AnchorLayoutBox : public LazyLayoutBox {
public:
    /**
     * \brief Sets anchor
     * \note One unit is one pixel, max size is plug::Vec2d(SCREEN_W, SCREEN_H)
    */
    AnchorLayoutBox(
        const plug::Vec2d &offset_, const plug::Vec2d &size_,
        const plug::Vec2d &anchor_position_, const plug::Vec2d &anchor_size_
    );

    /**
     * \brief Sets widget offset from anchor
    */
    virtual bool setPosition(const plug::Vec2d& offset_) override;

    /**
     * \brief Sets widget basic size
    */
    virtual bool setSize(const plug::Vec2d& basic_size_) override;

    /**
     * \brief Updates widget position and size
    */
    virtual void onParentUpdate(const plug::LayoutBox &parent_layout) override;

    /**
     * \brief Copies all information
    */
    virtual plug::LayoutBox *clone() const override;

protected:
    plug::Vec2d offset;               ///< Widget offset from anchor top-left corner
    plug::Vec2d basic_size;           ///< Widget basic size (without scaling)
    plug::Vec2d anchor_position;      ///< Anchor's top-left corner position
    plug::Vec2d anchor_size;          ///< Defines anchor's bottom-right corner position
};


#endif
