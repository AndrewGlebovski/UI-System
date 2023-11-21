/**
 * \file
 * \brief Contains layout box interface and basic implementations
*/


#ifndef _LAYOUT_BOX_H_
#define _LAYOUT_BOX_H_


#include "common/vector.hpp"


/// Contains widget position and size
class LayoutBox {
public:
    /**
     * \brief Returns widget position
    */
    virtual Vec2d getPosition() const = 0;

    /**
     * \brief Sets widget position
     * \return True if position changed, false otherwise
    */
    virtual bool setPosition(const Vec2d& position_) = 0;

    /**
     * \brief Returns widget size
    */
    virtual Vec2d getSize() const = 0;

    /**
     * \brief Sets widget size
     * \return True if size changed, false otherwise
    */
    virtual bool setSize(const Vec2d& size_) = 0;

    /**
     * \brief Changes its position and size according to parent
    */
    virtual void onParentUpdate(const LayoutBox &parent_layout) = 0;

    /**
     * \brief Returns copy of this layout box
     * \warning Do not forget to delete it
    */
    virtual LayoutBox *clone() const = 0;

    /**
     * \brief Default destructor
    */
    virtual ~LayoutBox() = default;
};


/// Simply stores given position and size
class LazyLayoutBox : public LayoutBox {
public:
    LazyLayoutBox();

    LazyLayoutBox(const Vec2d &position_, const Vec2d &size_);
    
    virtual Vec2d getPosition() const override;

    virtual bool setPosition(const Vec2d& position_) override;

    virtual Vec2d getSize() const override;

    virtual bool setSize(const Vec2d& size_) override;

    virtual void onParentUpdate(const LayoutBox &parent_layout) override;

    virtual LayoutBox *clone() const override;

protected:
    Vec2d position;         ///< Widget position
    Vec2d size;             ///< Widget size
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
    BoundLayoutBox(const Vec2d &position_, const Vec2d &size_);

    /**
     * \brief Sets position according to bound
    */
    virtual bool setPosition(const Vec2d& position_) override;

    /**
     * \brief Sets size according to bound
    */
    virtual bool setSize(const Vec2d& size_) override;

    /**
     * \brief Updates bound and changes size and position according to new bound
    */
    virtual void onParentUpdate(const LayoutBox &parent_layout) override;

    /**
     * \brief Clones position and size but not bounds
    */
    virtual LayoutBox *clone() const override;

protected:
    Vec2d bound;           ///< Parent size
};


/// Anchor implementation
class AnchorLayoutBox : public LazyLayoutBox {
public:
    /**
     * \brief Sets anchor
     * \note One unit is one pixel, max size is Vec2d(SCREEN_W, SCREEN_H)
    */
    AnchorLayoutBox(
        const Vec2d &offset_, const Vec2d &size_,
        const Vec2d &anchor_position_, const Vec2d &anchor_size_
    );

    /**
     * \brief Sets widget offset from anchor
    */
    virtual bool setPosition(const Vec2d& offset_) override;

    /**
     * \brief Sets widget basic size
    */
    virtual bool setSize(const Vec2d& basic_size_) override;

    /**
     * \brief Updates widget position and size
    */
    virtual void onParentUpdate(const LayoutBox &parent_layout) override;

    /**
     * \brief Copies all information
    */
    virtual LayoutBox *clone() const override;

protected:
    Vec2d offset;               ///< Widget offset from anchor top-left corner
    Vec2d basic_size;           ///< Widget basic size (without scaling)
    Vec2d anchor_position;      ///< Anchor's top-left corner position
    Vec2d anchor_size;          ///< Defines anchor's bottom-right corner position
};


#endif
