/**
 * \file
 * \brief Contains canvas, palettes and protypes of their functions 
*/


#ifndef _PALETTE_H_
#define _PALETTE_H_


#include "common/asset.hpp"
#include "basic/container.hpp"
#include "basic/button.hpp"
#include "canvas/canvas.hpp"


class ColorPalette {
public:
    ColorPalette(Color fg_color, Color bg_color);

    Color getFGColor() const;

    Color getBGColor() const;

    void setFGColor(Color new_fg_color);

    void setBGColor(Color new_bg_color);

private:
    Color foreground;
    Color background;
};


enum class State {
    Pressed,
    Released,
};


struct ControlState {
    ControlState(State state_) : state(state_) {}

    State state;
};


/// Base class for canvas tool
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


class CanvasFilter {
public:
    virtual void applyFilter(Canvas &canvas) const = 0;

    virtual Widget *getWidget() { return nullptr; };

    virtual ~CanvasFilter() = default;
};


/// Handles tools and colors for canvas
class ToolPalette {
public:
    /// Tools avalaible for the palette
    enum TOOLS {
        PENCIL_TOOL,        ///< Pencil tool
        RECT_TOOL,          ///< Rectangle tool
        LINE_TOOL,          ///< Line tool
        ERASER_TOOL,        ///< Eraser
        COLOR_PICKER,       ///< Color picker
        BUCKET_TOOL,        ///< Bucket tool
        POLYGON_TOOL,       ///< Polygon tool
        TEXT_TOOL,          ///< Text tool
        TOOLS_SIZE          ///< Tools size (this field must always be last!)
    };

    ToolPalette(ColorPalette &color_palette);

    size_t getCurrentIndex() const { return current_tool; };

    CanvasTool *getCurrentTool();

    void setCurrentTool(size_t index);

    void setActiveCanvas(Canvas &canvas);

    void setColorPalette(ColorPalette &color_palette);

    virtual ~ToolPalette();

private:
    List<CanvasTool*> tools;
    size_t current_tool;
};


/// GUI for ToolPalette
class ToolPaletteView : public Widget {
public:
    ToolPaletteView(
        size_t id_, const LayoutBox &layout_,
        ToolPalette *palette_, const PaletteViewAsset &asset_
    );


    ToolPaletteView(const ToolPaletteView &palette_view) = default;
    ToolPaletteView &operator = (const ToolPaletteView &palette_view) = default;

    /**
     * \brief Draws tool buttons
    */
    virtual void draw(RenderTarget &result, TransformStack &stack) override;

    /**
     * \brief Broadcast events to tool buttons
    */
    virtual void onEvent(const Event &event, EHC &ehc) override;

    /**
     * \brief Deletes tool buttons
    */
    virtual ~ToolPaletteView();

protected:
    Container buttons;                  ///< ToolPalette buttons for tool selection
    ToolPalette *palette;               ///< ToolPalette which this ToolPaletteView affects
    const PaletteViewAsset &asset;      ///< Assets for buttons
    ButtonGroup *group;                 ///< Tool buttons group

    /**
     * \brief Checks which tool is active and sets corresponding button as pressed in group
    */
    void updateToolButtons();

    virtual void onKeyboardPressed(const KeyboardPressedEvent &event, EHC &ehc) override;
};


/// Contains filters and remembers last used one 
class FilterPalette {
private:
    List<CanvasFilter*> filters;        ///< Filter instances
    size_t last_filter;                 ///< Last used filter index

public:
    /// Filters avalaible for the palette
    enum FILTERS {
        LIGHTEN_FILTER,     ///< Increase intensity
        DARKEN_FILTER,      ///< Decrease intensity
        MONOCHROME_FILTER,  ///< Black and white style
        NEGATIVE_FILTER,    ///< Negative colors
        FILTERS_SIZE        ///< Filters size (this field must always be last!)
    };

    /**
     * \brief Constructs all filters
    */
    FilterPalette();

    /**
     * \brief Returns last used filter instance
    */
    CanvasFilter *getLastFilter();

    /**
     * \brief Sets last used filter
    */
    void setLastFilter(FILTERS filter_id);

    /**
     * \brief Returns filter instance by ID
    */
    CanvasFilter *getFilter(FILTERS filter_id);

    /**
     * \brief Returns amount of supported filters
    */
    size_t getFilterCount() const;

    /**
     * \brief Delete filters
    */
    ~FilterPalette();
};


#endif
