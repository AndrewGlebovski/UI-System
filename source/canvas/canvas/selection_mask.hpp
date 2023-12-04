/**
 * \file
 * \brief Contains selection mask interface
*/


#ifndef _SELECTION_MASK_H_
#define _SELECTION_MASK_H_


class SelectionMask {
private:
    bool *mask;
    size_t width;
    size_t height;

public:
    SelectionMask(size_t width_, size_t height_);

    SelectionMask(const SelectionMask &mask) = delete;

    SelectionMask &operator = (const SelectionMask &mask) = delete;

    virtual ~SelectionMask();

    virtual size_t getWidth() const;

    virtual size_t getHeight() const;

    virtual bool getPixel(size_t x, size_t y) const;

    virtual void setPixelMask(size_t x, size_t y, bool flag);
    
    virtual void fill(bool value);
    
    virtual void invert();
};


#endif
