/**
 * \file
 * \brief Contains canvas and his tools classes and protypes of their functions 
*/


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


/// Standart pencil tool
class PencilTool : public CanvasTool {
protected:
    Vec2d prev_position;        ///< Previous mouse click position

public:
    PencilTool();


    virtual void onMainButton(ButtonState state, const Vec2d &mouse, Canvas &canvas) override;
    virtual void onMove(const Vec2d &mouse, Canvas &canvas) override;
};


/// Tool for drawing rectangle
class RectTool : public CanvasTool {
protected:
    Vec2d draw_start;           ///< Previous mouse click position
    Widget *rect_preview;       ///< Widget that draws preview of the rectangle


    /**
     * \brief Creates SFML rectangle based on two points 
    */
    sf::RectangleShape createRect(const Vec2d &p1, const Vec2d &p2) const;

public:
    RectTool();


    RectTool(const RectTool &rect_tool) = delete;
    RectTool &operator = (const RectTool &rect_tool) = delete;


    virtual void onMainButton(ButtonState state, const Vec2d &mouse, Canvas &canvas) override;
    virtual void onMove(const Vec2d &mouse, Canvas &canvas) override;
    virtual void onConfirm(const Vec2d &mouse, Canvas &canvas) override;
    virtual Widget *getWidget() override;


    virtual ~RectTool() override;
};


/// Tool for drawing line
class LineTool : public CanvasTool {
protected:
    Vec2d draw_start;           ///< Previous mouse click position
    Widget *line_preview;       ///< Widget that draws preview of the line

public:
    LineTool();


    LineTool(const LineTool &line_tool) = delete;
    LineTool &operator = (const LineTool &line_tool) = delete;


    virtual void onMainButton(ButtonState state, const Vec2d &mouse, Canvas &canvas) override;
    virtual void onMove(const Vec2d &mouse, Canvas &canvas) override;
    virtual void onConfirm(const Vec2d &mouse, Canvas &canvas) override;
    virtual Widget *getWidget() override;


    virtual ~LineTool() override;
};


/// Eraser tool
class EraserTool : public CanvasTool {
protected:
    Vec2d prev_position;        ///< Previous mouse click position

public:
    EraserTool();


    virtual void onMainButton(ButtonState state, const Vec2d &mouse, Canvas &canvas) override;
    virtual void onMove(const Vec2d &mouse, Canvas &canvas) override;
};


/// Changes palette color to color of pixel under mouse
class ColorPicker : public CanvasTool {
public:
    virtual void onMainButton(ButtonState state, const Vec2d &mouse, Canvas &canvas) override;
};


/// Fill all pixels with the same color to palette color
class BucketTool : public CanvasTool {
public:
    virtual void onMainButton(ButtonState state, const Vec2d &mouse, Canvas &canvas) override;
};


/// Tool for drawing polygon
class PolygonTool : public CanvasTool {
protected:
    List<Vec2d> points;         ///< Points that form polygon
    Widget *polygon_preview;    ///< Widget that draws preview of the polygon

public:
    PolygonTool();


    PolygonTool(const PolygonTool &polygon_tool) = delete;
    PolygonTool &operator = (const PolygonTool &polygon_tool) = delete;


    List<Vec2d> &getPoints();


    virtual void onMainButton(ButtonState state, const Vec2d &mouse, Canvas &canvas) override;
    virtual void onConfirm(const Vec2d &mouse, Canvas &canvas) override;
    virtual void onCancel() override;
    virtual Widget *getWidget() override;


    virtual ~PolygonTool() override;
};


/// Draws text on the canvas
class TextTool : public CanvasTool {
protected:
    sf::Font text_font;             ///< Text font
    LineEdit *text_preview;         ///< Widget that draws preview of the polygon

public:
    TextTool();


    TextTool(const TextTool &text_tool) = delete;
    TextTool &operator = (const TextTool &text_tool) = delete;


    virtual void onMainButton(ButtonState state, const Vec2d &mouse, Canvas &canvas) override;
    virtual void onConfirm(const Vec2d &mouse, Canvas &canvas) override;
    virtual void onCancel() override;
    virtual Widget *getWidget() override;


    virtual ~TextTool() override;
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


class FilterMask {
private:
    bool *mask;
    size_t width;
    size_t height;

public:
    FilterMask();


    FilterMask(const FilterMask &mask) = delete;
    FilterMask &operator = (const FilterMask &mask) = delete;


    void initMask(size_t width_, size_t height_);


    bool getPixelMask(size_t x, size_t y) const;


    size_t getWidth() const;


    size_t getHeight() const;


    void setPixelMask(size_t x, size_t y, bool flag);
    
    
    void fill(bool value);
    
    
    void invert();


    ~FilterMask();
};


class CanvasFilter {
public:
    virtual void applyFilter(Canvas &canvas) const = 0;


    virtual ~CanvasFilter() = default;
};


/// Changes image colors intensity
class IntensityFilter : public CanvasFilter {
private:
    int intensity;

    unsigned char clip(int channel) const;

public:
    IntensityFilter(char intensity_);

    virtual void applyFilter(Canvas &canvas) const override;
};


/// Change image colors to black and white style
class MonochromeFilter : public CanvasFilter {
public:
    MonochromeFilter();

    virtual void applyFilter(Canvas &canvas) const override;
};


/// Change image colors negative
class NegativeFilter : public CanvasFilter {
public:
    NegativeFilter();

    virtual void applyFilter(Canvas &canvas) const override;
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


class FilterAction : public ButtonAction {
private:
    FilterPalette::FILTERS filter_id;
    FilterPalette &palette;
    CanvasGroup &group;

public:
    FilterAction(FilterPalette::FILTERS filter_id_, FilterPalette &palette_, CanvasGroup &group_);


    virtual void operator () () override;


    virtual FilterAction *clone() override;
};


class VScrollCanvas;
class HScrollCanvas;


/// Holds texture to draw on
class Canvas : public Widget {
public:
    friend VScrollCanvas;
    friend HScrollCanvas;

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
    FilterMask &getFilterMask();

    /**
     * \brief Returns true if canvas is active in his group
    */
    bool isActive() const;

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
    FilterMask filter_mask;
    std::string filename;

    virtual void onMouseMove(const MouseMoveEvent &event, EHC &ehc) override;
    virtual void onMousePressed(const MousePressedEvent &event, EHC &ehc) override;
    virtual void onMouseReleased(const MouseReleasedEvent &event, EHC &ehc) override;
    virtual void onKeyboardPressed(const KeyboardPressedEvent &event, EHC &ehc) override;
    virtual void onKeyboardReleased(const KeyboardReleasedEvent &event, EHC &ehc) override;
};


/// Moves canvas texture in vertical direction
class VScrollCanvas : public ScrollAction {
protected:
    Canvas &canvas;

public:
    VScrollCanvas(Canvas &canvas_) : canvas(canvas_) {}

    virtual void operator () (vec_t param) override {
        vec_t canvas_y = canvas.getLayoutBox().getSize().y;

        if (canvas.getTextureSize().y > canvas_y)
            canvas.texture_offset.y = param * (canvas.getTextureSize().y - canvas_y);
    }
};


/// Moves canvas texture in horizontal direction
class HScrollCanvas : public ScrollAction {
protected:
    Canvas &canvas;

public:
    HScrollCanvas(Canvas &canvas_) : canvas(canvas_) {}

    virtual void operator () (vec_t param) override {
        vec_t canvas_x = canvas.getLayoutBox().getSize().x;

        if (canvas.getTextureSize().x > canvas_x)
            canvas.texture_offset.x = param * (canvas.getTextureSize().x - canvas_x);
    }
};


/// Supports hot keys for applying filters
class FilterHotkey : public Widget {
public:
    FilterHotkey(Widget *parent_, FilterPalette &palette_, CanvasGroup &group_);

protected:
    virtual void onKeyboardPressed(const KeyboardPressedEvent &event, EHC &ehc) override;

private:
    FilterPalette &palette;
    CanvasGroup &group;
};
