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
    SelectionMask();


    SelectionMask(const SelectionMask &mask) = delete;
    SelectionMask &operator = (const SelectionMask &mask) = delete;


    void initMask(size_t width_, size_t height_);


    bool getPixelMask(size_t x, size_t y) const;


    size_t getWidth() const;


    size_t getHeight() const;


    void setPixelMask(size_t x, size_t y, bool flag);
    
    
    void fill(bool value);
    
    
    void invert();


    ~SelectionMask();
};


#endif
