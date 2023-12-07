/**
 * \file
 * \brief Contains interface of ToolPalette and ToolPaletteView
*/


#ifndef _TOOL_PALETTE_H_
#define _TOOL_PALETTE_H_


#include "canvas/plugin.hpp"
#include "basic/container.hpp"
#include "basic/button.hpp"


/// Handles tools and colors for canvas
class ToolPalette {
public:
    /// Tools avalaible for the palette
    enum TOOLS {
        PENCIL_TOOL,        ///< Pencil tool
        RECT_TOOL,          ///< Rectangle tool
        LINE_TOOL,          ///< Line tool
        ERASER_TOOL,        ///< Eraser
        COLOR_PICKER,       ///< plug::Color picker
        BUCKET_TOOL,        ///< Bucket tool
        POLYGON_TOOL,       ///< Polygon tool
        TEXT_TOOL,          ///< Text tool
        TOOLS_SIZE          ///< Count of predefined tools (this field must always be last!)
    };

    ToolPalette(plug::ColorPalette &color_palette);

    plug::Tool *getTool(size_t index);

    size_t getCurrentIndex() const;

    plug::Tool *getCurrentTool();

    void setCurrentTool(size_t index);

    void setActiveCanvas(plug::Canvas &canvas);

    void setColorPalette(plug::ColorPalette &color_palette);

    void addTool(plug::Tool &new_tool);

    size_t getToolCount() const;

    virtual ~ToolPalette();

private:
    List<plug::Tool*> tools;
    size_t current_tool;
};


/// GUI for ToolPalette
class ToolPaletteView : public Widget {
public:
    ToolPaletteView(
        size_t id_, const plug::LayoutBox &layout_,
        const PaletteViewAsset &asset_
    );

    ToolPaletteView(const ToolPaletteView &palette_view) = default;

    ToolPaletteView &operator = (const ToolPaletteView &palette_view) = default;

    /**
     * \brief Draws tool buttons
    */
    virtual void draw(plug::TransformStack &stack, plug::RenderTarget &result) override;

    /**
     * \brief Broadcast events to tool buttons
    */
    virtual void onEvent(const plug::Event &event, plug::EHC &ehc) override;

    /**
     * \brief Deletes tool buttons
    */
    virtual ~ToolPaletteView() override;

protected:
    /**
     * \brief Checks hot keys for tools
    */
    virtual void onKeyboardPressed(const plug::KeyboardPressedEvent &event, plug::EHC &ehc) override;

    /**
     * \brief Resizes self and internal container
    */
    virtual void onParentUpdate(const plug::LayoutBox &parent_layout) override;

private:
    /**
     * \brief Checks which tool is active and sets corresponding button as pressed in group
    */
    void updateCurrentButton();

    /**
     * \brief Checks for new tools in ToolPalette and adds them to PaletteView
    */
    void updateButtons();

    /**
     * \brief Adds new tool from ToolPalette to PaletteView
    */
    void addTool(size_t tool_id);

    Container buttons;                  ///< ToolPalette buttons for tool selection
    const PaletteViewAsset &asset;      ///< Assets for buttons
    ButtonGroup *group;                 ///< Tool buttons group
    List<plug::Texture*> icons;         ///< Additional tools icons
};


#endif
