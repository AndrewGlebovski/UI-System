/**
 * \file
 * \brief Contains plug::ColorPalette and ColorPaletteView implementation
*/


#include "canvas/palettes/color_palette.hpp"
#include "canvas/palettes/palette_manager.hpp"
#include "common/utils.hpp"


// ============================================================================


const plug::Vec2d COLOR_ICON_SIZE = plug::Vec2d(94, 94);         ///< Tool icon size


// ============================================================================


ColorPalette::ColorPalette(plug::Color fg_color, plug::Color bg_color) :
    foreground(fg_color), background(bg_color) {}


plug::Color ColorPalette::getFGColor() const { return foreground; }


plug::Color ColorPalette::getBGColor() const { return background; }


void ColorPalette::setFGColor(plug::Color new_fg_color) {
    foreground = new_fg_color;
}


void ColorPalette::setBGColor(plug::Color new_bg_color) {
    background = new_bg_color;
}


// ============================================================================


class SetColorAction : public ButtonAction {
public:
    SetColorAction(plug::Color color_) :
        color(color_) {}

    void operator () () override {
        COLOR_PALETTE.setFGColor(color);
    }

    ButtonAction *clone() override {
        return new SetColorAction(color);
    }

protected:
    plug::Color color;
};


// ============================================================================


#define ADD_COLOR_BUTTON(COLOR, POSITION)           \
do {                                                \
    buttons.addChild(new SimpleRectButton(          \
        Widget::AUTO_ID,                            \
        LazyLayoutBox(POSITION, COLOR_ICON_SIZE),   \
        new SetColorAction(COLOR),                  \
        SimpleRectButtonStyle(COLOR, COLOR, COLOR)  \
    ));                                             \
} while(0)


ColorPaletteView::ColorPaletteView(size_t id_, const plug::LayoutBox &layout_) :
    Widget(id_, layout_), 
    buttons(1, AnchorLayoutBox(plug::Vec2d(), plug::Vec2d(SCREEN_W, SCREEN_H), plug::Vec2d(), plug::Vec2d(SCREEN_W, SCREEN_H)), false)
{
    buttons.setParent(this);

    ADD_COLOR_BUTTON(Black, plug::Vec2d());
    ADD_COLOR_BUTTON(White, plug::Vec2d(94, 0));
    ADD_COLOR_BUTTON(Red,plug::Vec2d(0, 94));
    ADD_COLOR_BUTTON(Green, plug::Vec2d(94, 94));
    ADD_COLOR_BUTTON(Blue, plug::Vec2d(0, 188));
    ADD_COLOR_BUTTON(Yellow, plug::Vec2d(94, 188));
    ADD_COLOR_BUTTON(Magenta, plug::Vec2d(0, 282));
    ADD_COLOR_BUTTON(Cyan, plug::Vec2d(94, 282));
}


#undef ADD_COLOR_BUTTON


void ColorPaletteView::draw(plug::TransformStack &stack, plug::RenderTarget &result) {
    TransformApplier add_transform(stack, getTransform());
    buttons.draw(stack, result);
}


void ColorPaletteView::onEvent(const plug::Event &event, plug::EHC &ehc) {
    TransformApplier canvas_transform(ehc.stack, getTransform());
    buttons.onEvent(event, ehc);
}


void ColorPaletteView::onParentUpdate(const plug::LayoutBox &parent_layout) {
    getLayoutBox().onParentUpdate(parent_layout);
    buttons.onParentUpdate(getLayoutBox());
}
