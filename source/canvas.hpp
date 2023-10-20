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


    virtual void onMainButton(ButtonState state, const Vector2D &mouse, Canvas &canvas) = 0;
    virtual void onSecondaryButton(ButtonState state, const Vector2D &mouse, Canvas &canvas) {}
    virtual void onModifier1(ButtonState state, const Vector2D &mouse, Canvas &canvas) {}
    virtual void onModifier2(ButtonState state, const Vector2D &mouse, Canvas &canvas) {}
    virtual void onModifier3(ButtonState state, const Vector2D &mouse, Canvas &canvas) {}
    virtual void onMove(const Vector2D &mouse, Canvas &canvas) {}
    virtual void onConfirm(const Vector2D &mouse, Canvas &canvas) { is_drawing = false; }
    virtual void onCancel() { is_drawing = false; }
    virtual BaseUI *getWidget() { return nullptr; };


    virtual ~CanvasTool() = default;
};


/// Standart pencil tool
class PencilTool : public CanvasTool {
protected:
    Vector2D prev_position;        ///< Previous mouse click position

public:
    PencilTool();


    virtual void onMainButton(ButtonState state, const Vector2D &mouse, Canvas &canvas) override;
    virtual void onMove(const Vector2D &mouse, Canvas &canvas) override;
};


/// Tool for drawing rectangle
class RectTool : public CanvasTool {
protected:
    Vector2D draw_start;            ///< Previous mouse click position
    BaseUI *rect_preview;           ///< Widget that draws preview of the rectangle


    /**
     * \brief Creates SFML rectangle based on two points 
    */
    sf::RectangleShape createRect(const Vector2D &p1, const Vector2D &p2) const;

public:
    RectTool();


    virtual void onMainButton(ButtonState state, const Vector2D &mouse, Canvas &canvas) override;
    virtual void onMove(const Vector2D &mouse, Canvas &canvas) override;
    virtual void onConfirm(const Vector2D &mouse, Canvas &canvas) override;
    virtual BaseUI *getWidget() override;


    virtual ~RectTool() override;
};


/// Tool for drawing line
class LineTool : public CanvasTool {
protected:
    Vector2D draw_start;            ///< Previous mouse click position
    BaseUI *line_preview;           ///< Widget that draws preview of the line

public:
    LineTool();


    virtual void onMainButton(ButtonState state, const Vector2D &mouse, Canvas &canvas) override;
    virtual void onMove(const Vector2D &mouse, Canvas &canvas) override;
    virtual void onConfirm(const Vector2D &mouse, Canvas &canvas) override;
    virtual BaseUI *getWidget() override;


    virtual ~LineTool() override;
};


/// Eraser tool
class EraserTool : public CanvasTool {
protected:
    Vector2D prev_position;        ///< Previous mouse click position

public:
    EraserTool();


    virtual void onMainButton(ButtonState state, const Vector2D &mouse, Canvas &canvas) override;
    virtual void onMove(const Vector2D &mouse, Canvas &canvas) override;
};


/// Changes palette color to color of pixel under mouse
class ColorPicker : public CanvasTool {
public:
    virtual void onMainButton(ButtonState state, const Vector2D &mouse, Canvas &canvas) override;
};


/// Fill all pixels with the same color to palette color
class BucketTool : public CanvasTool {
public:
    virtual void onMainButton(ButtonState state, const Vector2D &mouse, Canvas &canvas) override;
};


/// Tool for drawing polygon
class PolygonTool : public CanvasTool {
protected:
    List<Vector2D> points;          ///< Points that form polygon
    BaseUI *polygon_preview;        ///< Widget that draws preview of the polygon

public:
    PolygonTool();


    List<Vector2D> &getPoints();


    virtual void onMainButton(ButtonState state, const Vector2D &mouse, Canvas &canvas) override;
    virtual void onConfirm(const Vector2D &mouse, Canvas &canvas) override;
    virtual void onCancel() override;
    virtual BaseUI *getWidget() override;


    virtual ~PolygonTool() override;
};


/// Handles tools and colors for canvas
class Palette {
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
        TOOLS_SIZE          ///< Tools size (this field must always be last!)
    };


    Palette();


    size_t getCurrentIndex() const { return current_tool; };


    CanvasTool *getCurrentTool();


    void setCurrentTool(size_t index);


    const sf::Color &getCurrentColor() const;


    void setCurrentColor(const sf::Color &color);


    virtual ~Palette();
};


/// GUI for Palette
class PaletteView : public BaseUI {
protected:
    Container buttons;                  ///< Palette buttonы for tool selection
    Palette *palette;                   ///< Palette which this PaletteView affects
    const PaletteViewAsset &asset;      ///< Assets for buttons
    ButtonGroup *group;                 ///< Tool buttons group


    /**
     * \brief Checks which tool is active and sets corresponding button as pressed in group
    */
    void updateToolButtons();

public:
    PaletteView(
        size_t id_, const Transform &transform_, const Vector2D &size_, int z_index_, BaseUI *parent_,
        Palette *palette_, const PaletteViewAsset &asset_
    );


    PaletteView(const PaletteView &palette_view) = default;


    PaletteView &operator = (const PaletteView &palette_view) = default;


    virtual void draw(sf::RenderTexture &result, List<Transform> &transforms) override;


    virtual int onMouseMove(int mouse_x, int mouse_y, List<Transform> &transforms) override;
    virtual int onMouseButtonDown(int mouse_x, int mouse_y, int button_id, List<Transform> &transforms) override;
    virtual int onMouseButtonUp(int mouse_x, int mouse_y, int button_id, List<Transform> &transforms) override;
    virtual int onKeyDown(int key_id) override;


    virtual ~PaletteView();
};


class VScrollCanvas;
class HScrollCanvas;


/// Holds texture to draw on
class Canvas : public BaseUI {
protected:
    sf::RenderTexture texture;
    Vector2D texture_offset;

    Palette *palette;

    Vector2D last_position;


    /**
     * \brief Clears canvas with white color
    */
    void clear_canvas();

public:
    friend VScrollCanvas;
    friend HScrollCanvas;


    /**
     * \brief Creates empty canvas or opens image specified by path
    */
    Canvas(
        size_t id_, const Transform &transform_, const Vector2D &size_, int z_index_, BaseUI *parent_,
        const char *image_path, Palette *palette_
    );


    Canvas(const Canvas &canvas) = default;


    Canvas &operator = (const Canvas &canvas) = default;


    Vector2D getTextureSize() const;


    sf::RenderTexture &getTexture();


    Palette *getPalette();


    virtual void draw(sf::RenderTexture &result, List<Transform> &transforms) override;


    virtual int onMouseMove(int mouse_x, int mouse_y, List<Transform> &transforms) override;
    virtual int onMouseButtonDown(int mouse_x, int mouse_y, int button_id, List<Transform> &transforms) override;
    virtual int onMouseButtonUp(int mouse_x, int mouse_y, int button_id, List<Transform> &transforms) override;
    virtual int onKeyDown(int key_id) override;
    virtual int onKeyUp(int key_id) override;
    virtual int onParentResize() override;
};


class VScrollCanvas : public ScrollAction {
protected:
    Canvas &canvas;

public:
    VScrollCanvas(Canvas &canvas_) : canvas(canvas_) {}


    virtual void operator () (vec_t param) override {
        canvas.texture_offset.y = param * (canvas.getTextureSize().y - canvas.size.y);
    }
};


class HScrollCanvas : public ScrollAction {
protected:
    Canvas &canvas;

public:
    HScrollCanvas(Canvas &canvas_) : canvas(canvas_) {}


    virtual void operator () (vec_t param) override {
        canvas.texture_offset.x = param * (canvas.getTextureSize().x - canvas.size.x);
    }
};
