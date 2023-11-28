/**
 * \file
 * \brief Contains ToolPalette and FilterPalette interface
*/


#ifndef _PALETTES_H_
#define _PALETTES_H_


#include "basic/container.hpp"
#include "basic/button.hpp"
#include "canvas/plugin.hpp"


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

    Tool *getCurrentTool();

    void setCurrentTool(size_t index);

    void setActiveCanvas(Canvas &canvas);

    void setColorPalette(ColorPalette &color_palette);

    virtual ~ToolPalette();

private:
    List<Tool*> tools;
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
    List<Filter*> filters;        ///< Filter instances
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
    Filter *getLastFilter();

    /**
     * \brief Sets last used filter
    */
    void setLastFilter(FILTERS filter_id);

    /**
     * \brief Returns filter instance by ID
    */
    Filter *getFilter(FILTERS filter_id);

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
