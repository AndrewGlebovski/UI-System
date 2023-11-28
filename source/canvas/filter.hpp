/**
 * \file
 * \brief Declares some canvas filters
*/


#ifndef _FILTER_H_
#define _FILTER_H_


#include "canvas/palettes.hpp"


/// Changes image colors intensity
class IntensityFilter : public BasicFilter {
private:
    int intensity;

    unsigned char clip(int channel) const;

public:
    IntensityFilter(char intensity_);

    virtual void applyFilter(Canvas &canvas) const override;
};


/// Change image colors to black and white style
class MonochromeFilter : public BasicFilter {
public:
    MonochromeFilter();

    virtual void applyFilter(Canvas &canvas) const override;
};


/// Change image colors negative
class NegativeFilter : public BasicFilter {
public:
    NegativeFilter();

    virtual void applyFilter(Canvas &canvas) const override;
};


#endif
