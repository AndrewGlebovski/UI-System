/**
 * \file
 * \brief Contains inteface of ColorPalette
*/


#ifndef _COLOR_PALETTE_H_
#define _COLOR_PALETTE_H_


#include "widget/color.hpp"


class ColorPalette {
public:
    ColorPalette(Color fg_color, Color bg_color);

    virtual Color getFGColor() const;

    virtual Color getBGColor() const;

    virtual void setFGColor(Color new_fg_color);

    virtual void setBGColor(Color new_bg_color);

    virtual ~ColorPalette() = default;

private:
    Color foreground;
    Color background;
};


#endif
