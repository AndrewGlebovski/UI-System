/**
 * \file
 * \brief Contains plug::ColorPalette and ColorPaletteView interface
*/


#ifndef _COLOR_PALETTE_H_
#define _COLOR_PALETTE_H_


#include "basic/container.hpp"
#include "standart/Tool/ColorPalette.h"


class ColorPalette : public plug::ColorPalette {
public:
    ColorPalette(plug::Color fg_color, plug::Color bg_color);

    virtual plug::Color getFGColor() const override;

    virtual plug::Color getBGColor() const override;

    virtual void setFGColor(plug::Color new_fg_color) override;

    virtual void setBGColor(plug::Color new_bg_color) override;

private:
    plug::Color foreground;
    plug::Color background;
};


/// GUI for plug::ColorPalette
class ColorPaletteView : public Widget {
public:
    ColorPaletteView(size_t id_, const plug::LayoutBox &layout_);

    ColorPaletteView(const ColorPaletteView &palette_view) = default;

    ColorPaletteView &operator = (const ColorPaletteView &palette_view) = default;

    /**
     * \brief Draws color buttons
    */
    virtual void draw(plug::TransformStack &stack, plug::RenderTarget &result) override;

    /**
     * \brief Broadcast events to color buttons
    */
    virtual void onEvent(const plug::Event &event, plug::EHC &ehc) override;

protected:
    /**
     * \brief Resizes self and internal container
    */
    virtual void onParentUpdate(const plug::LayoutBox &parent_layout) override;

private:
    Container buttons;      ///< plug::Color buttons for tool selection
};


#endif
