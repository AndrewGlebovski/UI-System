/**
 * \file
 * \brief Declares some canvas filters
*/


#ifndef _FILTER_H_
#define _FILTER_H_


#include "canvas/palette.hpp"


/// Changes image colors intensity
class IntensityFilter : public CanvasFilter {
private:
    int intensity;

    unsigned char clip(int channel) const;

public:
    IntensityFilter(char intensity_);

    virtual void applyFilter(Canvas &canvas) const override;
};


/// Change image colors to black and white style
class MonochromeFilter : public CanvasFilter {
public:
    MonochromeFilter();

    virtual void applyFilter(Canvas &canvas) const override;
};


/// Change image colors negative
class NegativeFilter : public CanvasFilter {
public:
    NegativeFilter();

    virtual void applyFilter(Canvas &canvas) const override;
};


#endif
