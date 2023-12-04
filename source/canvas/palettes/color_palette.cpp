/**
 * \file
 * \brief Contains implementation of ColorPalette
*/


#include "canvas/palettes/color_palette.hpp"


// ============================================================================


ColorPalette::ColorPalette(Color fg_color, Color bg_color) :
    foreground(fg_color), background(bg_color) {}


Color ColorPalette::getFGColor() const { return foreground; }


Color ColorPalette::getBGColor() const { return background; }


void ColorPalette::setFGColor(Color new_fg_color) { foreground = new_fg_color; }


void ColorPalette::setBGColor(Color new_bg_color) { background = new_bg_color; }
