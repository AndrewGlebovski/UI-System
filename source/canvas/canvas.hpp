/**
 * \file
 * \brief Contains canvas, palettes and protypes of their functions 
*/


#ifndef _CANVAS_H_
#define _CANVAS_H_


#include "common/asset.hpp"
#include "widget/widget.hpp"
#include "basic/container.hpp"
#include "basic/button.hpp"
#include "canvas/selection_mask.hpp"


class Canvas;


/// Base class for canvas tool
class CanvasTool {
protected:
    bool is_drawing;            ///< Tool is in drawing mode

public:
    enum ButtonState {
        PRESSED,
        REALEASED
    };


    CanvasTool() : is_drawing(false) {}


    virtual void onMainButton(ButtonState state, const Vec2d &mouse, Canvas &canvas) = 0;
    virtual void onSecondaryButton(ButtonState state, const Vec2d &mouse, Canvas &canvas) {}
    virtual void onModifier1(ButtonState state, const Vec2d &mouse, Canvas &canvas) {}
    virtual void onModifier2(ButtonState state, const Vec2d &mouse, Canvas &canvas) {}
    virtual void onModifier3(ButtonState state, const Vec2d &mouse, Canvas &canvas) {}
    virtual void onMove(const Vec2d &mouse, Canvas &canvas) {}
    virtual void onConfirm(const Vec2d &mouse, Canvas &canvas) { is_drawing = false; }
    virtual void onCancel() { is_drawing = false; }
    virtual Widget *getWidget() { return nullptr; };


    virtual ~CanvasTool() = default;
};


/// Handles tools and colors for canvas
class ToolPalette {
protected:
    List<CanvasTool*> tools;
    size_t current_tool;

    sf::Color current_color;

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


    ToolPalette();


    size_t getCurrentIndex() const { return current_tool; };


    CanvasTool *getCurrentTool();


    void setCurrentTool(size_t index);


    const sf::Color &getCurrentColor() const;


    void setCurrentColor(const sf::Color &color);


    virtual ~ToolPalette();
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
    virtual void draw(sf::RenderTarget &result, TransformStack &stack) override;

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


class CanvasFilter {
public:
    virtual void applyFilter(Canvas &canvas) const = 0;


    virtual ~CanvasFilter() = default;
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


class CanvasGroup {
private:
    List<Canvas*> canvases;     ///< Canvases in this group
    size_t active;              ///< Currently active canvas


    /**
     * \brief Returns index of the canvas in list
    */
    size_t getIndex(Canvas *canvas) const;

public:
    /**
     * \brief Creates empty group
    */
    CanvasGroup();


    /**
     * \brief Sets active canvas
    */
    void setActive(Canvas *new_active);


    /**
     * \brief Returns active canvas
    */
    Canvas *getActive();


    /**
     * \brief Adds canvas to this group
    */
    void addCanvas(Canvas *new_canvas);


    /**
     * \brief Removes canvas from this group
    */
    void removeCanvas(Canvas *canvas);


    /**
     * \brief Checks if the canvas is in this group
    */
    bool isInGroup(Canvas *canvas) const;
};


const sf::Color CANVAS_BACKGROUND = sf::Color::White;   ///< Canvas background color


/// Holds texture to draw on
class Canvas : public Widget {
public:
    /**
     * \brief Creates empty canvas
    */
    Canvas(
        size_t id_, const LayoutBox &layout_,
        ToolPalette &palette_, CanvasGroup &group_
    );

    Canvas(const Canvas &canvas) = default;
    Canvas &operator = (const Canvas &canvas) = default;

    /**
     * \brief Clears canvas with background color
    */
    void clearCanvas();

    /**
     * \brief Creates image with the given size filled with background color
    */
    bool createImage(size_t width, size_t height);

    /**
     * \brief Opens image file
     * \note If file fails to open, nothing happens
    */
    bool openImage(const char *filename_);

    /**
     * \brief Saves texture to current image file
     * \warning Assert will be called if image is not open
    */
    void saveImage();

    /**
     * \brief Saves texture to path
     * \note Changes filename field to filename_
    */
    void saveImageAs(const char *filename_);

    /**
     * \brief Returns path to current image
     * \note Returns nullptr if image is not open
    */
    const char *getFilename() const;

    /**
     * \brief Returns true if image is open
    */
    bool isImageOpen() const;

    /**
     * \brief Returns size of the texture
    */
    Vec2d getTextureSize() const;

    /**
     * \brief Returns reference to texture
    */
    sf::RenderTexture &getTexture();

    /**
     * \brief Returns palette that this canvas is using
    */
    ToolPalette *getPalette();

    /**
     * \brief Returns canvas filter mask
    */
    SelectionMask &getSelectionMask();

    /**
     * \brief Returns true if canvas is active in his group
    */
    bool isActive() const;

    /**
     * \brief Returns texture offset
    */
    const Vec2d &getTextureOffset() const;

    /**
     * \brief Sets texture offset
    */
    void setTextureOffset(const Vec2d &texture_offset_);

    /**
     * \brief Draws canvas inner texture
    */
    virtual void draw(sf::RenderTarget &result, TransformStack &stack) override;

    /**
     * \brief Broadcast events to tool widget
    */
    virtual void onEvent(const Event &event, EHC &ehc) override;

    /**
     * \brief Removes canvas from his group
    */
    virtual ~Canvas() override;

protected:
    sf::RenderTexture texture;
    Vec2d texture_offset;
    ToolPalette *palette;
    Vec2d last_position;
    CanvasGroup *group;
    SelectionMask filter_mask;
    std::string filename;

    virtual void onMouseMove(const MouseMoveEvent &event, EHC &ehc) override;
    virtual void onMousePressed(const MousePressedEvent &event, EHC &ehc) override;
    virtual void onMouseReleased(const MouseReleasedEvent &event, EHC &ehc) override;
    virtual void onKeyboardPressed(const KeyboardPressedEvent &event, EHC &ehc) override;
    virtual void onKeyboardReleased(const KeyboardReleasedEvent &event, EHC &ehc) override;
};


#endif
