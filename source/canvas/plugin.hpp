/**
 * \file
 * \brief Contains interface for Tool and Filter
*/


#ifndef _PLUGIN_H_
#define _PLUGIN_H_


#include "widget/widget.hpp"
#include "canvas/canvas.hpp"
#include "canvas/color_palette.hpp"


enum class State {
    Pressed,
    Released,
};


struct ControlState {
    ControlState(State state_) : state(state_) {}

    State state;
};


/// Base class for canvas tools
class CanvasTool {
protected:
    bool is_drawing;                ///< Tool is in drawing mode
    Canvas *canvas;                 ///< Canvas to draw on
    ColorPalette *color_palette;    ///< Color palette

public:
    CanvasTool() : is_drawing(false), canvas(nullptr), color_palette(nullptr) {}

    CanvasTool(const CanvasTool&) = default;
    
    CanvasTool &operator = (const CanvasTool&) = default;

    virtual void setColorPalette(ColorPalette &color_palette_) { color_palette = &color_palette_; };
    
    virtual void setActiveCanvas(Canvas &canvas_) { onCancel(); canvas = &canvas_; };

    virtual void onMainButton(ControlState state, const Vec2d &mouse) = 0;
    virtual void onSecondaryButton(ControlState state, const Vec2d &mouse) {}
    virtual void onModifier1(ControlState state) {}
    virtual void onModifier2(ControlState state) {}
    virtual void onModifier3(ControlState state) {}
    virtual void onMove(const Vec2d &mouse) {}
    virtual void onConfirm() { is_drawing = false; }
    virtual void onCancel() { is_drawing = false; }
    virtual Widget *getWidget() { return nullptr; };

    virtual ~CanvasTool() = default;
};


/// Base class for all filters
class CanvasFilter {
public:
    virtual void applyFilter(Canvas &canvas) const = 0;

    virtual Widget *getWidget() { return nullptr; };

    virtual ~CanvasFilter() = default;
};


#endif
