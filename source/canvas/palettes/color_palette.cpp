/**
 * \file
 * \brief Contains ColorPalette and ColorPaletteView implementation
*/


#include "canvas/palettes/color_palette.hpp"
#include "canvas/palettes/palette_manager.hpp"


// ============================================================================


const Vec2d COLOR_ICON_SIZE = Vec2d(94, 94);         ///< Tool icon size


// ============================================================================


ColorPalette::ColorPalette(Color fg_color, Color bg_color) :
    foreground(fg_color), background(bg_color) {}


Color ColorPalette::getFGColor() const { return foreground; }


Color ColorPalette::getBGColor() const { return background; }


void ColorPalette::setFGColor(Color new_fg_color) { foreground = new_fg_color; }


void ColorPalette::setBGColor(Color new_bg_color) { background = new_bg_color; }


// ============================================================================


class SetColorAction : public ButtonAction {
public:
    SetColorAction(Color color_) :
        color(color_) {}

    void operator () () override {
        COLOR_PALETTE.setFGColor(color);
    }

    ButtonAction *clone() override {
        return new SetColorAction(color);
    }

protected:
    Color color;
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


ColorPaletteView::ColorPaletteView(size_t id_, const LayoutBox &layout_) :
    Widget(id_, layout_), 
    buttons(1, AnchorLayoutBox(Vec2d(), Vec2d(SCREEN_W, SCREEN_H), Vec2d(), Vec2d(SCREEN_W, SCREEN_H)), false)
{
    buttons.setParent(this);

    ADD_COLOR_BUTTON(Black, Vec2d());
    ADD_COLOR_BUTTON(White, Vec2d(94, 0));
    ADD_COLOR_BUTTON(Red,Vec2d(0, 94));
    ADD_COLOR_BUTTON(Green, Vec2d(94, 94));
    ADD_COLOR_BUTTON(Blue, Vec2d(0, 188));
    ADD_COLOR_BUTTON(Yellow, Vec2d(94, 188));
    ADD_COLOR_BUTTON(Magenta, Vec2d(0, 282));
    ADD_COLOR_BUTTON(Cyan, Vec2d(94, 282));
}


#undef ADD_COLOR_BUTTON


void ColorPaletteView::draw(RenderTarget &result, TransformStack &stack) {
    TransformApplier add_transform(stack, getTransform());
    buttons.draw(result, stack);
}


void ColorPaletteView::onEvent(const Event &event, EHC &ehc) {
    TransformApplier canvas_transform(ehc.stack, getTransform());
    buttons.onEvent(event, ehc);
}


void ColorPaletteView::onParentUpdate(const LayoutBox &parent_layout) {
    getLayoutBox().onParentUpdate(parent_layout);
    buttons.onParentUpdate(getLayoutBox());
}
