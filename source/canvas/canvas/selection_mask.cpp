/**
 * \file
 * \brief Contains implementation of selection mask
*/


#include "common/assert.hpp"
#include "canvas/canvas/selection_mask.hpp"


// ============================================================================


SelectionMask::SelectionMask(size_t width_, size_t height_) :
    mask(nullptr), width(width_), height(height_) 
{
    mask = new bool[width * height];
    ASSERT(mask, "Failed to allocate mask!\n");
}


bool SelectionMask::getPixel(size_t x, size_t y) const {
    ASSERT(mask, "Mask is nullptr!\n");
    ASSERT(x < width, "X is out of range!\n");
    ASSERT(y < height, "Y is out of range!\n");
    return mask[y * width + x];
}


size_t SelectionMask::getWidth() const { return width; }


size_t SelectionMask::getHeight() const { return height; }


void SelectionMask::setPixelMask(size_t x, size_t y, bool flag) {
    ASSERT(mask, "Mask is nullptr!\n");
    ASSERT(x < width, "X is out of range!\n");
    ASSERT(y < height, "Y is out of range!\n");
    mask[y * width + x] = flag;
}


void SelectionMask::fill(bool value) {
    ASSERT(mask, "Mask is nullptr!\n");

    size_t mask_size = width * height;
    for (size_t i = 0; i < mask_size; i++)
        mask[i] = value;
}


void SelectionMask::invert() {
    ASSERT(mask, "Mask is nullptr!\n");

    size_t mask_size = width * height;
    for (size_t i = 0; i < mask_size; i++)
        mask[i] = !mask[i];
}


SelectionMask::~SelectionMask() {
    if (mask) delete[] mask;
}
