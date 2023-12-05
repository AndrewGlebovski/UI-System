/**
 * \file
 * \brief Contains ColorPalette and ColorPaletteView interface
*/


#ifndef _COLOR_PALETTE_H_
#define _COLOR_PALETTE_H_


#include "widget/color.hpp"
#include "basic/container.hpp"


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


/// GUI for ColorPalette
class ColorPaletteView : public Widget {
public:
    ColorPaletteView(size_t id_, const LayoutBox &layout_);

    ColorPaletteView(const ColorPaletteView &palette_view) = default;

    ColorPaletteView &operator = (const ColorPaletteView &palette_view) = default;

    /**
     * \brief Draws color buttons
    */
    virtual void draw(TransformStack &stack, RenderTarget &result) override;

    /**
     * \brief Broadcast events to color buttons
    */
    virtual void onEvent(const Event &event, EHC &ehc) override;

protected:
    /**
     * \brief Resizes self and internal container
    */
    virtual void onParentUpdate(const LayoutBox &parent_layout) override;

private:
    Container buttons;      ///< Color buttons for tool selection
};


#endif
