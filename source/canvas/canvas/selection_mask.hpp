/**
 * \file
 * \brief Contains selection mask interface
*/


#ifndef _SELECTION_MASK_H_
#define _SELECTION_MASK_H_


#include "standart/Canvas/SelectionMask.h"


class SelectionMask : public plug::SelectionMask {
private:
    bool *mask;
    size_t width;
    size_t height;

public:
    SelectionMask(size_t width_, size_t height_);

    SelectionMask(const SelectionMask &mask) = delete;

    SelectionMask &operator = (const SelectionMask &mask) = delete;

    virtual size_t getWidth() const override;

    virtual size_t getHeight() const override;

    virtual bool getPixel(size_t x, size_t y) const override;

    virtual void setPixel(size_t x, size_t y, bool flag) override;
    
    virtual void fill(bool value) override;
    
    virtual void invert() override;

    virtual ~SelectionMask() override;
};


#endif
