/**
 * \file
 * \brief Contains sources of layout boxes functions
*/


#include <cmath>
#include "config/configs.hpp"
#include "widget/layout_box.hpp"
#include "common/utils.hpp"


// ============================================================================


LazyLayoutBox::LazyLayoutBox() : position(), size() {}


LazyLayoutBox::LazyLayoutBox(const plug::Vec2d &position_, const plug::Vec2d &size_) :
    position(position_), size(size_) {}


plug::Vec2d LazyLayoutBox::getPosition() const {
    return position;
}


bool LazyLayoutBox::setPosition(const plug::Vec2d& position_) {
    if (isEqual(position, position_)) return false;
    position = position_;
    return true;
}


plug::Vec2d LazyLayoutBox::getSize() const {
    return size;
}


bool LazyLayoutBox::setSize(const plug::Vec2d& size_) {
    if (isEqual(size, size_)) return false;
    size = size_;
    return true;
}


void LazyLayoutBox::onParentUpdate(const plug::LayoutBox &parent_layout) {}


plug::LayoutBox *LazyLayoutBox::clone() const {
    return new LazyLayoutBox(position, size);
}


// ============================================================================


BoundLayoutBox::BoundLayoutBox() : LazyLayoutBox(), bound(SCREEN_W, SCREEN_H) {}


BoundLayoutBox::BoundLayoutBox(const plug::Vec2d &position_, const plug::Vec2d &size_) :
    LazyLayoutBox(position_, size_), bound(SCREEN_W, SCREEN_H) {}


bool BoundLayoutBox::setPosition(const plug::Vec2d& position_) {
    plug::Vec2d prev_pos = position;
    position = position_;

    if (position.x < 0)
        position.x = 0;
    else if (position.x + size.x > bound.x)
        position.x = bound.x - size.x;

    if (position.y < 0)
        position.y = 0;
    else if (position.y + size.y > bound.y)
        position.y = bound.y - size.y;

    return !isEqual(position, prev_pos);
}


bool BoundLayoutBox::setSize(const plug::Vec2d& size_) {
    plug::Vec2d prev_size = size;
    size = size_;

    if (size.x < 0)
        size.x = 0;
    else if (size_.x + position.x > bound.x)
        size.x = bound.x - position.x;

    if (size.y < 0)
        size.y = 0;
    else if (size.y + position.y > bound.y)
        size.y = bound.y - position.y;

    return !isEqual(size, prev_size);
}


void BoundLayoutBox::onParentUpdate(const plug::LayoutBox &parent_layout) {
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


plug::LayoutBox *BoundLayoutBox::clone() const { return new BoundLayoutBox(position, size); }


// ============================================================================


AnchorLayoutBox::AnchorLayoutBox(
    const plug::Vec2d &offset_, const plug::Vec2d &size_,
    const plug::Vec2d &anchor_position_, const plug::Vec2d &anchor_size_
) :
    LazyLayoutBox(plug::Vec2d(), size_),
    offset(offset_), basic_size(size_),
    anchor_position(anchor_position_), anchor_size(anchor_size_)
{}


bool AnchorLayoutBox::setPosition(const plug::Vec2d& offset_) {
    if (isEqual(offset, offset_)) return false;
    offset = offset_;
    return true;
}


bool AnchorLayoutBox::setSize(const plug::Vec2d& basic_size_) {
    if (isEqual(basic_size, basic_size_)) return false;
    basic_size = basic_size_;
    return true;
}


void AnchorLayoutBox::onParentUpdate(const plug::LayoutBox &parent_layout) {
    plug::Vec2d bound = parent_layout.getSize();

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


plug::LayoutBox *AnchorLayoutBox::clone() const {
    return new AnchorLayoutBox(offset, size, anchor_position, anchor_size);
}
