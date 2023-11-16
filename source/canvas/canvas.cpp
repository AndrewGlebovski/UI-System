/**
 * \file
 * \brief Contains sources of canvas and his tools classes functions
*/


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wfloat-conversion"


#include <SFML/Graphics.hpp>
#include "canvas/canvas.hpp"


const sf::Color PREVIEW_COLOR = sf::Color::Blue;        ///< Standart color for all previews
const float RECT_PREVIEW_OUTLINE = -1;                  ///< Rect preview outline thickness
const int ERASER_STEP = 100;                            ///< Amount of spheres that drawn between two points
const float ERASER_RADIUS = 25;                         ///< Eraser sphere radius
const sf::Color CANVAS_BACKGROUND = sf::Color::White;   ///< Canvas background color
const float POLYGON_EPSILON = 25;                       ///< Maximal distance for points of polygon to form it
const unsigned TEXT_SIZE = 20;                          ///< Text tool font size
const size_t TEXT_MAX_LENGTH = 256;                     ///< Text tool text max length


// ============================================================================


/// Draws line on the texture
void drawLine(const Transform &trans, const Vec2d &p1, const Vec2d &p2, const sf::Color &color, sf::RenderTarget &result);


/// Draws polygon or polyline based on the primitive type
void drawPolygon(const Transform &trans, const List<Vec2d> &points, const sf::Color &color, sf::RenderTarget &result, sf::PrimitiveType type);


// ============================================================================


void drawLine(const Transform &trans, const Vec2d &p1, const Vec2d &p2, const sf::Color &color, sf::RenderTarget &result) {
    sf::Vertex line[] = {
        sf::Vertex(trans.apply(p1), color),
        sf::Vertex(trans.apply(p2), color)
    };

    result.draw(line, 2, sf::Lines);
}


void drawPolygon(const Transform &trans, const List<Vec2d> &points, const sf::Color &color, sf::RenderTarget &result, sf::PrimitiveType type) {
    sf::Vertex *polygon = new sf::Vertex[points.size()];

    for (size_t i = 0; i < points.size(); i++)
        polygon[i] = sf::Vertex(trans.apply(points[i]), color);

    result.draw(polygon, points.size(), type);

    delete polygon;
}


// ============================================================================


PencilTool::PencilTool() : prev_position() {}


void PencilTool::onMainButton(ButtonState state, const Vec2d &mouse, Canvas &canvas) {
    switch (state) {
        case PRESSED:
            is_drawing = true;
            prev_position = mouse;
            break;
        case REALEASED:
            is_drawing = false;
        default:
            break;
    }
}


void PencilTool::onMove(const Vec2d &mouse, Canvas &canvas) {
    if (is_drawing) {
        sf::Vertex line[] = {
            sf::Vertex(prev_position, canvas.getPalette()->getCurrentColor()),
            sf::Vertex(mouse, canvas.getPalette()->getCurrentColor()),
        };

        prev_position = mouse;
        canvas.getTexture().draw(line, 2, sf::Lines);
    }
}


// ============================================================================


/// Draws preview of the rectangle
class RectPreview : public Widget {
private:
    RectTool &tool;     ///< Tool that holds this object

public:
    RectPreview(RectTool &tool_) :
        Widget(1, LazyLayoutBox()), tool(tool_) {}
    

    virtual void draw(sf::RenderTarget &result, TransformStack &stack) override {
        Vec2d global_position = stack.apply(layout->getPosition());
        Vec2d global_size = stack.apply_size(layout->getSize());

        sf::RectangleShape rect(global_size);
        rect.setPosition(global_position);
        rect.setFillColor(sf::Color(0));
        rect.setOutlineThickness(RECT_PREVIEW_OUTLINE);
        rect.setOutlineColor(PREVIEW_COLOR);
        result.draw(rect);
    }
};


RectTool::RectTool() : draw_start(), rect_preview(nullptr) {
    rect_preview = new RectPreview(*this);
}


sf::RectangleShape RectTool::createRect(const Vec2d &p1, const Vec2d &p2) const {
    Vec2d position = p1;
    Vec2d size = p2 - p1;
    if (position.x > p2.x) position.x = p2.x;
    if (position.y > p2.y) position.y = p2.y;
    if (size.x < 0) size.x *= -1;
    if (size.y < 0) size.y *= -1;

    sf::RectangleShape rect(size);
    rect.setPosition(position);

    return rect;
}


void RectTool::onMainButton(ButtonState state, const Vec2d &mouse, Canvas &canvas) {
    switch (state) {
        case PRESSED:
            is_drawing = true;
            draw_start = mouse;
            rect_preview->getLayoutBox().setPosition(mouse);
            rect_preview->getLayoutBox().setSize(Vec2d());
            break;
        case REALEASED:
            onConfirm(mouse, canvas);
        default:
            break;
    }
}


void RectTool::onMove(const Vec2d &mouse, Canvas &canvas) {
    sf::RectangleShape rect = createRect(draw_start, mouse);
    rect_preview->getLayoutBox().setPosition(rect.getPosition());
    rect_preview->getLayoutBox().setSize(rect.getSize());
}


void RectTool::onConfirm(const Vec2d &mouse, Canvas &canvas) {
    if (is_drawing) {
        sf::RectangleShape rect = createRect(draw_start, mouse);
        rect.setFillColor(canvas.getPalette()->getCurrentColor());

        canvas.getTexture().draw(rect);

        is_drawing = false;
    }
}


Widget *RectTool::getWidget() {
    return (is_drawing) ? rect_preview : nullptr;
}


RectTool::~RectTool() {
    delete rect_preview;
}


// ============================================================================


/// Draws preview of the line
class LinePreview : public Widget {
private:
    LineTool &tool;     ///< Tool that holds this object

public:
    /**
     * \note Transform offset used as line's first point and size as second
    */
    LinePreview(LineTool &tool_) :
        Widget(1, LazyLayoutBox()), tool(tool_) {}
    

    /**
     * \note Transform offset used as line's first point and size as second
    */
    virtual void draw(sf::RenderTarget &result, TransformStack &stack) override {
        // REMAINDER: transform.offset = point1, size = point2
        drawLine(stack.top(), layout->getPosition(), layout->getSize(), PREVIEW_COLOR, result);
    }
};


LineTool::LineTool() : draw_start(), line_preview(nullptr) {
    line_preview = new LinePreview(*this);
}


void LineTool::onMainButton(ButtonState state, const Vec2d &mouse, Canvas &canvas) {
    switch (state) {
        case PRESSED:
            is_drawing = true;
            draw_start = mouse;
            line_preview->getLayoutBox().setPosition(mouse);
            line_preview->getLayoutBox().setSize(mouse);
            break;
        case REALEASED:
            onConfirm(mouse, canvas);
        default:
            break;
    }
}


void LineTool::onMove(const Vec2d &mouse, Canvas &canvas) {
    line_preview->getLayoutBox().setSize(mouse);
}


void LineTool::onConfirm(const Vec2d &mouse, Canvas &canvas) {
    if (is_drawing) {
        drawLine(Transform(), draw_start, mouse, canvas.getPalette()->getCurrentColor(), canvas.getTexture());
        is_drawing = false;
    }
}


Widget *LineTool::getWidget() {
    return (is_drawing) ? line_preview : nullptr;
}


LineTool::~LineTool() {
    delete line_preview;
}


// ============================================================================


EraserTool::EraserTool() : prev_position() {}


void EraserTool::onMainButton(ButtonState state, const Vec2d &mouse, Canvas &canvas) {
    switch (state) {
        case PRESSED: {
            is_drawing = true;
            prev_position = mouse;

            sf::CircleShape circle(ERASER_RADIUS);
            circle.setFillColor(CANVAS_BACKGROUND);
            circle.setPosition(mouse - Vec2d(ERASER_RADIUS, ERASER_RADIUS));
            canvas.getTexture().draw(circle);

            break;
        }
        case REALEASED:
            is_drawing = false;
        default:
            break;
    }
}


void EraserTool::onMove(const Vec2d &mouse, Canvas &canvas) {
    if (is_drawing) {
        Vec2d step(
            (mouse.x - prev_position.x) / ERASER_STEP, 
            (mouse.y - prev_position.y) / ERASER_STEP
        );

        sf::CircleShape circle(ERASER_RADIUS);
        circle.setFillColor(CANVAS_BACKGROUND);
        circle.setPosition(mouse - Vec2d(ERASER_RADIUS, ERASER_RADIUS));

        for (int i = 0; i < ERASER_STEP; i++) {
            canvas.getTexture().draw(circle);
            circle.setPosition(circle.getPosition() + step);
        }

        prev_position = mouse;
    }
}


// ============================================================================


void ColorPicker::onMainButton(ButtonState state, const Vec2d &mouse, Canvas &canvas) {
    if (state == PRESSED) {
        // VERY SLOW (TEXTURE PIXELS COPIES TO AN IMAGE)
        canvas.getPalette()->setCurrentColor(canvas.getTexture().getTexture().copyToImage().getPixel(mouse.x, mouse.y));
    }
}


// ============================================================================


void BucketTool::onMainButton(ButtonState state, const Vec2d &mouse, Canvas &canvas) {
    if (state == PRESSED) {
        sf::Image image = canvas.getTexture().getTexture().copyToImage();
        sf::Color color = canvas.getPalette()->getCurrentColor();
        sf::Color origin = image.getPixel(mouse.x, mouse.y);

        List<sf::Vector2u> dfs;
        dfs.push_back(sf::Vector2u(mouse.x, mouse.y));
        image.setPixel(dfs.back().x, dfs.back().y, color);
        
        while (dfs.size()) {
            sf::Vector2u pixel = dfs[dfs.size() - 1];
            dfs.remove(dfs.size() - 1);

            if (pixel.x > 0 && image.getPixel(pixel.x - 1, pixel.y) == origin) {
                dfs.push_back(sf::Vector2u(pixel.x - 1, pixel.y));
                image.setPixel(pixel.x - 1, pixel.y, color);
            }

            if (pixel.x < image.getSize().x - 1 && image.getPixel(pixel.x + 1, pixel.y) == origin) {
                dfs.push_back(sf::Vector2u(pixel.x + 1, pixel.y));
                image.setPixel(pixel.x + 1, pixel.y, color);
            }

            if (pixel.y > 0 && image.getPixel(pixel.x, pixel.y - 1) == origin) {
                dfs.push_back(sf::Vector2u(pixel.x, pixel.y - 1));
                image.setPixel(pixel.x, pixel.y - 1, color);
            }

            if (pixel.y < image.getSize().y - 1 && image.getPixel(pixel.x, pixel.y + 1) == origin) {
                dfs.push_back(sf::Vector2u(pixel.x, pixel.y + 1));
                image.setPixel(pixel.x, pixel.y + 1, color);
            }
        }

        sf::Texture tool_texture;
        tool_texture.loadFromImage(image);

        sf::Sprite tool_sprite(tool_texture);
        canvas.getTexture().draw(tool_sprite);
    }
}


// ============================================================================


/// Draws preview of the polygon
class PolygonPreview : public Widget {
private:
    PolygonTool &tool;     ///< Tool that holds this object

public:
    PolygonPreview(PolygonTool &tool_) :
        Widget(1, LazyLayoutBox()), tool(tool_) {}
    

    virtual void draw(sf::RenderTarget &result, TransformStack &stack) override {
        drawPolygon(stack.top(), tool.getPoints(), PREVIEW_COLOR, result, sf::PrimitiveType::LineStrip);
    }
};


PolygonTool::PolygonTool() : points(), polygon_preview(nullptr) {
    polygon_preview = new PolygonPreview(*this);
}


List<Vec2d> &PolygonTool::getPoints() {
    return points;
}


void PolygonTool::onMainButton(ButtonState state, const Vec2d &mouse, Canvas &canvas) {
    if (state == PRESSED) {
        is_drawing = true;
        if (points.size() && (points[0] - mouse).length() < POLYGON_EPSILON)
            onConfirm(mouse, canvas);
        else
            points.push_back(mouse);
    }
}


void PolygonTool::onConfirm(const Vec2d &mouse, Canvas &canvas) {
    if (is_drawing) {
        drawPolygon(Vec2d(), points, canvas.getPalette()->getCurrentColor(), canvas.getTexture(), sf::PrimitiveType::TriangleFan);
        is_drawing = false;
    }

    points.resize(0, Vec2d());
}


void PolygonTool::onCancel() {
    is_drawing = false;
    points.resize(0, Vec2d());
}


Widget *PolygonTool::getWidget() {
    return (is_drawing) ? polygon_preview : nullptr;
}


PolygonTool::~PolygonTool() {
    delete polygon_preview;
}


// ============================================================================


TextTool::TextTool() : text_font(), text_preview(nullptr) {
    ASSERT(text_font.loadFromFile(FONT_FILE), "Failed to load font!\n");

    LineEditStyle style(
        text_font,
        TEXT_SIZE,
        PREVIEW_COLOR,
        sf::Color(0),
        PREVIEW_COLOR,
        sf::Color(0),
        0
    );

    text_preview = new LineEdit(
        Widget::AUTO_ID,
        LazyLayoutBox(Vec2d(), Vec2d(SCREEN_W, TEXT_SIZE + TEXT_OFFSET * 2)),
        style,
        TEXT_MAX_LENGTH
    );
}


void TextTool::onMainButton(ButtonState state, const Vec2d &mouse, Canvas &canvas) {
    if (state == PRESSED) {
        is_drawing = true;
        text_preview->getLayoutBox().setPosition(mouse);
        text_preview->setKeyboardFocus(true);
    }
}


void TextTool::onConfirm(const Vec2d &mouse, Canvas &canvas) {
    if (is_drawing) {
        sf::Text text(text_preview->getString(), text_font, TEXT_SIZE);
        text.setPosition(text_preview->getLayoutBox().getPosition() + Vec2d(TEXT_OFFSET, TEXT_OFFSET));
        text.setFillColor(canvas.getPalette()->getCurrentColor());
        canvas.getTexture().draw(text);
        is_drawing = false;
    }

    text_preview->setKeyboardFocus(false);
    text_preview->setString("");
}


void TextTool::onCancel() {
    is_drawing = false;
    text_preview->setKeyboardFocus(false);
    text_preview->setString("");
}


Widget *TextTool::getWidget() {
    return (is_drawing) ? text_preview : nullptr;
}


TextTool::~TextTool() {
    delete text_preview;
}


// ============================================================================


ToolPalette::ToolPalette() : tools(TOOLS_SIZE, nullptr), current_tool(PENCIL_TOOL), current_color(sf::Color::Red) {
    tools[PENCIL_TOOL] = new PencilTool();
    tools[RECT_TOOL] = new RectTool();
    tools[LINE_TOOL] = new LineTool();
    tools[ERASER_TOOL] = new EraserTool();
    tools[COLOR_PICKER] = new ColorPicker();
    tools[BUCKET_TOOL] = new BucketTool();
    tools[POLYGON_TOOL] = new PolygonTool();
    tools[TEXT_TOOL] = new TextTool();
}


CanvasTool *ToolPalette::getCurrentTool() {
    return tools[current_tool];
}


void ToolPalette::setCurrentTool(size_t index) {
    if (index < TOOLS_SIZE) {
        tools[current_tool]->onCancel();
        current_tool = index;
    }
}


const sf::Color &ToolPalette::getCurrentColor() const {
    return current_color;
}


void ToolPalette::setCurrentColor(const sf::Color &color)  {
    current_color = color;
}


ToolPalette::~ToolPalette() {
    for (size_t i = 0; i < tools.size(); i++)
        delete tools[i];
}


// ============================================================================


class PaletteAction : public ButtonAction {
protected:
    ToolPalette &palette;
    int tool_id;

public:
    PaletteAction(ToolPalette &palette_, int tool_id_) : palette(palette_), tool_id(tool_id_) {}


    virtual void operator () () override {
        palette.setCurrentTool(tool_id);
    }


    virtual ButtonAction *clone() override {
        return new PaletteAction(palette, tool_id);
    }
};


// ============================================================================


void ToolPaletteView::updateToolButtons() {
    size_t current_tool = palette->getCurrentIndex();
    ActionButton *current_button = (ActionButton*) buttons.findWidget(current_tool + Widget::AUTO_ID + 1);

    if (group->getPressed() != current_button) group->setPressed(current_button);
}


#define ADD_TOOL_BUTTON(TOOL_ID, TOOL_TEXTURE_ID, POSITION)     \
do {                                                            \
    TextureIconButton *btn =  new TextureIconButton(            \
        Widget::AUTO_ID + TOOL_ID + 1,                          \
        LazyLayoutBox(POSITION, Vec2d()),                       \
        new PaletteAction(*palette, TOOL_ID),                   \
        asset[PaletteViewAsset::NORMAL_TEXTURE],                \
        asset[PaletteViewAsset::NORMAL_TEXTURE],                \
        asset[PaletteViewAsset::SELECTED_TEXTURE],              \
        asset[TOOL_TEXTURE_ID]                                  \
    );                                                          \
    btn->setButtonGroup(group);                                 \
    buttons.addChild(btn);                                      \
} while(0)


ToolPaletteView::ToolPaletteView(
    size_t id_, const LayoutBox &layout_,
    ToolPalette *palette_, const PaletteViewAsset &asset_
) :
    Widget(id_, layout_), 
    buttons(1, LazyLayoutBox(Vec2d(), layout->getSize()), false),
    palette(palette_), asset(asset_), group(nullptr)
{
    buttons.setParent(this);

    group = new ButtonGroup();

    ADD_TOOL_BUTTON(ToolPalette::PENCIL_TOOL,   PaletteViewAsset::PENCIL_TEXTURE,   Vec2d());
    ADD_TOOL_BUTTON(ToolPalette::RECT_TOOL,     PaletteViewAsset::RECT_TEXTURE,     Vec2d(94, 0));
    ADD_TOOL_BUTTON(ToolPalette::LINE_TOOL,     PaletteViewAsset::LINE_TEXTURE,     Vec2d(0, 94));
    ADD_TOOL_BUTTON(ToolPalette::ERASER_TOOL,   PaletteViewAsset::ERASER_TEXTURE,   Vec2d(94, 94));
    ADD_TOOL_BUTTON(ToolPalette::COLOR_PICKER,  PaletteViewAsset::PICKER_TEXTURE,   Vec2d(0, 188));
    ADD_TOOL_BUTTON(ToolPalette::BUCKET_TOOL,   PaletteViewAsset::BUCKET_TEXTURE,   Vec2d(94, 188));
    ADD_TOOL_BUTTON(ToolPalette::POLYGON_TOOL,  PaletteViewAsset::POLYGON_TEXTURE,  Vec2d(0, 282));
    ADD_TOOL_BUTTON(ToolPalette::TEXT_TOOL,     PaletteViewAsset::TEXT_TEXTURE,     Vec2d(94, 282));

    updateToolButtons();
}


#undef ADD_TOOL_BUTTON


void ToolPaletteView::draw(sf::RenderTarget &result, TransformStack &stack) {
    TransformApplier add_transform(stack, getTransform());

    updateToolButtons();

    buttons.draw(result, stack);
}


void ToolPaletteView::onEvent(const Event &event, EHC &ehc) {
    Widget::onEvent(event, ehc);
    if (ehc.stopped) return;

    TransformApplier canvas_transform(ehc.stack, getTransform());
    buttons.onEvent(event, ehc);
}


void ToolPaletteView::onKeyboardPressed(const KeyboardPressedEvent &event, EHC &ehc) {
    // No modifiers pressed
    if (event.alt || event.ctrl || event.shift) return;

    switch(event.key_id) {
        case P: palette->setCurrentTool(ToolPalette::PENCIL_TOOL); break;
        case R: palette->setCurrentTool(ToolPalette::RECT_TOOL); break;
        case L: palette->setCurrentTool(ToolPalette::LINE_TOOL); break;
        case E: palette->setCurrentTool(ToolPalette::ERASER_TOOL); break;
        case C: palette->setCurrentTool(ToolPalette::COLOR_PICKER); break;
        case F: palette->setCurrentTool(ToolPalette::BUCKET_TOOL); break;
        case O: palette->setCurrentTool(ToolPalette::POLYGON_TOOL); break;
        case T: palette->setCurrentTool(ToolPalette::TEXT_TOOL); break;
        default: return;
    }

    ehc.stopped = true;
}


ToolPaletteView::~ToolPaletteView() {
    if (group) delete group;
}


// ============================================================================


FilterMask::FilterMask() : mask(nullptr), width(0), height(0) {}


void FilterMask::initMask(size_t width_, size_t height_) {
    if (mask) delete[] mask;

    width = width_;
    height = height_;
    mask = new bool[width * height];
    ASSERT(mask, "Failed to allocate mask!\n");
}


bool FilterMask::getPixelMask(size_t x, size_t y) const {
    ASSERT(mask, "Mask is nullptr!\n");
    ASSERT(x < width, "X is out of range!\n");
    ASSERT(y < height, "Y is out of range!\n");
    return mask[y * width + x];
}


size_t FilterMask::getWidth() const { return width; }


size_t FilterMask::getHeight() const { return height; }


void FilterMask::setPixelMask(size_t x, size_t y, bool flag) {
    ASSERT(mask, "Mask is nullptr!\n");
    ASSERT(x < width, "X is out of range!\n");
    ASSERT(y < height, "Y is out of range!\n");
    mask[y * width + x] = flag;
}


void FilterMask::fill(bool value) {
    ASSERT(mask, "Mask is nullptr!\n");

    size_t mask_size = width * height;
    for (size_t i = 0; i < mask_size; i++)
        mask[i] = value;
}


void FilterMask::invert() {
    ASSERT(mask, "Mask is nullptr!\n");

    size_t mask_size = width * height;
    for (size_t i = 0; i < mask_size; i++)
        mask[i] = !mask[i];
}


FilterMask::~FilterMask() {
    if (mask) delete[] mask;
}


// ============================================================================


unsigned char IntensityFilter::clip(int channel) const {
    if (channel + intensity < 0) return 0;
    if (channel + intensity > 255) return 255;
    return channel + intensity;
}


IntensityFilter::IntensityFilter(char intensity_) : intensity(intensity_) {}


void IntensityFilter::applyFilter(Canvas &canvas) const {
    FilterMask &mask = canvas.getFilterMask();
    sf::Image image = canvas.getTexture().getTexture().copyToImage();

    ASSERT(image.getSize().x == mask.getWidth(), "Invalid mask size!\n");
    ASSERT(image.getSize().y == mask.getHeight(), "Invalid mask size!\n");
    
    for (size_t y = 0; y < mask.getHeight(); y++) {
        for (size_t x = 0; x < mask.getWidth(); x++) {
            if (mask.getPixelMask(x, y)) {
                sf::Color origin(image.getPixel(x, y));
                image.setPixel(x, y, sf::Color(clip(origin.r), clip(origin.g), clip(origin.b)));
            }
        }
    }
    
    sf::Texture tool_texture;
    tool_texture.loadFromImage(image);

    sf::Sprite tool_sprite(tool_texture);
    canvas.getTexture().draw(tool_sprite);
}


// ============================================================================


MonochromeFilter::MonochromeFilter() {}


void MonochromeFilter::applyFilter(Canvas &canvas) const {
    FilterMask &mask = canvas.getFilterMask();
    sf::Image image = canvas.getTexture().getTexture().copyToImage();

    ASSERT(image.getSize().x == mask.getWidth(), "Invalid mask size!\n");
    ASSERT(image.getSize().y == mask.getHeight(), "Invalid mask size!\n");
    
    for (size_t y = 0; y < mask.getHeight(); y++) {
        for (size_t x = 0; x < mask.getWidth(); x++) {
            if (mask.getPixelMask(x, y)) {
                sf::Color origin(image.getPixel(x, y));
                unsigned aver = (unsigned(origin.r) + unsigned(origin.g) + unsigned(origin.b)) / 3U;
                image.setPixel(x, y, sf::Color(aver, aver, aver));
            }
        }
    }
    
    sf::Texture tool_texture;
    tool_texture.loadFromImage(image);

    sf::Sprite tool_sprite(tool_texture);
    canvas.getTexture().draw(tool_sprite);
}


// ============================================================================


NegativeFilter::NegativeFilter() {}


void NegativeFilter::applyFilter(Canvas &canvas) const {
    FilterMask &mask = canvas.getFilterMask();
    sf::Image image = canvas.getTexture().getTexture().copyToImage();

    ASSERT(image.getSize().x == mask.getWidth(), "Invalid mask size!\n");
    ASSERT(image.getSize().y == mask.getHeight(), "Invalid mask size!\n");
    
    for (size_t y = 0; y < mask.getHeight(); y++) {
        for (size_t x = 0; x < mask.getWidth(); x++) {
            if (mask.getPixelMask(x, y)) {
                sf::Color origin(image.getPixel(x, y));
                image.setPixel(x, y, sf::Color(255 - origin.r, 255 - origin.g, 255 - origin.b));
            }
        }
    }
    
    sf::Texture tool_texture;
    tool_texture.loadFromImage(image);

    sf::Sprite tool_sprite(tool_texture);
    canvas.getTexture().draw(tool_sprite);
}


// ============================================================================


FilterPalette::FilterPalette() : filters(FILTERS_SIZE, nullptr), last_filter(0) {
    filters[LIGHTEN_FILTER] = new IntensityFilter(20);
    filters[DARKEN_FILTER] = new IntensityFilter(-20);
    filters[MONOCHROME_FILTER] = new MonochromeFilter();
    filters[NEGATIVE_FILTER] = new NegativeFilter();
}


CanvasFilter *FilterPalette::getLastFilter() { return filters[last_filter]; }


void FilterPalette::setLastFilter(FILTERS filter_id) { last_filter = filter_id; }


CanvasFilter *FilterPalette::getFilter(FILTERS filter_id) { return filters[filter_id]; }


size_t FilterPalette::getFilterCount() const { return FILTERS_SIZE; }


FilterPalette::~FilterPalette() {
    for (size_t i = 0; i < filters.size(); i++)
        delete filters[i];
}


// ============================================================================


size_t CanvasGroup::getIndex(Canvas *canvas) const {
    for (size_t i = 0; i < canvases.size(); i++)
        if (canvases[i] == canvas) return i;
    
    return canvases.size();
}


CanvasGroup::CanvasGroup() : canvases(), active(0) {}


void CanvasGroup::setActive(Canvas *new_active) {
    size_t index = getIndex(new_active);
    if (index < canvases.size()) active = index;
}


Canvas *CanvasGroup::getActive() {
    if (canvases.size() == 0) return nullptr;
    return canvases[active];
}


void CanvasGroup::addCanvas(Canvas *new_canvas) {
    if (!isInGroup(new_canvas)) {
        canvases.push_back(new_canvas);
        setActive(new_canvas);
    }
}


void CanvasGroup::removeCanvas(Canvas *canvas) {
    size_t index = getIndex(canvas);
    if (index < canvases.size()) {
        canvases.remove(index);

        if (canvases.size()) {
            if (index < active) {
                active--;
                setActive(canvases[active]);
            }
            else if (index == active) {
                active = 0;
                setActive(canvases[active]);
            }
        }
        else active = 0;
    }
}


bool CanvasGroup::isInGroup(Canvas *canvas) const {
    return (getIndex(canvas) < canvases.size()); 
}


// ============================================================================


FilterAction::FilterAction(FilterPalette::FILTERS filter_id_, FilterPalette &palette_, CanvasGroup &group_) : 
    filter_id(filter_id_), palette(palette_), group(group_) {}


void FilterAction::operator () () {
    if (group.getActive()) {
        palette.getFilter(filter_id)->applyFilter(*group.getActive());
        palette.setLastFilter(filter_id);
    }
}


FilterAction *FilterAction::clone() { return new FilterAction(filter_id, palette, group); }


// ============================================================================


Canvas::Canvas(
    size_t id_, const LayoutBox &layout_,
    ToolPalette &palette_, CanvasGroup &group_
) :
    Widget(id_, layout_),
    texture(), texture_offset(Vec2d(0, 0)),
    palette(&palette_), last_position(), group(&group_), filter_mask(),
    filename("")
{
    group->addCanvas(this);
}


void Canvas::clearCanvas() {
    texture.clear(CANVAS_BACKGROUND);
}


bool Canvas::createImage(size_t width, size_t height) {
    ASSERT(texture.create(width, height), "Failed to create texture!\n");
    clearCanvas();

    filter_mask.initMask(width, height);
    filter_mask.fill(true);

    filename = "";
    return true;
}


bool Canvas::openImage(const char *filename_) {
    sf::Texture image;
    if (image.loadFromFile(filename_)) {
        createImage(image.getSize().x, image.getSize().y);

        sf::Sprite tool_sprite(image);
        tool_sprite.setPosition(Vec2d());

        texture.draw(tool_sprite);

        filename = filename_;
        return true;
    }

    return false;
}


void Canvas::saveImage() {
    ASSERT(isImageOpen(), "File was not specified!\n");
    saveImageAs(filename.data());
}


void Canvas::saveImageAs(const char *filename_) {
    sf::Image image = texture.getTexture().copyToImage();
    image.saveToFile(filename_);
    filename = filename_;
}


const char *Canvas::getFilename() const {
    return (isImageOpen()) ? filename.data() : nullptr;
}


bool Canvas::isImageOpen() const {
    return filename.length() > 0;
}


Vec2d Canvas::getTextureSize() const {
    return Vec2d(texture.getSize().x, texture.getSize().y);
}


sf::RenderTexture &Canvas::getTexture() { return texture; }


ToolPalette *Canvas::getPalette() {
    ASSERT(palette, "Canvas must have palette!\n");
    return palette;
}


FilterMask &Canvas::getFilterMask() { return filter_mask; }


bool Canvas::isActive() const {
    ASSERT(group, "Canvas must be assigned to group!\n");
    return (this == group->getActive());
}


void Canvas::draw(sf::RenderTarget &result, TransformStack &stack) {
    Vec2d global_position = stack.apply(layout->getPosition());
    Vec2d global_size = stack.apply_size(layout->getSize());

    texture.display();

    sf::Sprite tool_sprite(texture.getTexture());
    sf::Vector2i offset_(texture_offset.x, texture_offset.y);
    sf::Vector2i size_(texture.getSize().x, texture.getSize().y);
    if (size_.x > global_size.x) size_.x = global_size.x;
    if (size_.y > global_size.y) size_.y = global_size.y;
    tool_sprite.setTextureRect(sf::IntRect(offset_, size_));
    tool_sprite.setPosition(global_position);

    result.draw(tool_sprite);

    if (isActive() && palette->getCurrentTool()->getWidget()) {
        TransformApplier canvas_transform(stack, getTransform());
        TransformApplier texture_transform(stack, Transform(-texture_offset));
        palette->getCurrentTool()->getWidget()->draw(result, stack);
    }
}


void Canvas::onEvent(const Event &event, EHC &ehc) {
    if (ehc.overlapped) return;

    Widget::onEvent(event, ehc);

    if (isActive() && palette->getCurrentTool()->getWidget()) {
        TransformApplier canvas_transform(ehc.stack, getTransform());
        TransformApplier texture_transform(ehc.stack, Transform(-texture_offset));
        palette->getCurrentTool()->getWidget()->onEvent(event, ehc);
    }
}


void Canvas::onMouseMove(const MouseMoveEvent &event, EHC &ehc) {
    Vec2d global_position = ehc.stack.apply(layout->getPosition());

    last_position = event.pos;

    palette->getCurrentTool()->onMove(
        event.pos - global_position + texture_offset,
        *this
    );

    ehc.overlapped = true;
}


void Canvas::onMousePressed(const MousePressedEvent &event, EHC &ehc) {
    if (event.button_id != MouseLeft) return;

    Vec2d global_position = ehc.stack.apply(layout->getPosition());
    Vec2d global_size = ehc.stack.apply_size(layout->getSize());

    if (isInsideRect(global_position, global_size, last_position)) {
        ASSERT(group, "Canvas must be assigned to group!\n");
        group->setActive(this);

        palette->getCurrentTool()->onMainButton(
            CanvasTool::PRESSED, 
            event.pos - global_position + texture_offset,
            *this
        );
        
        ehc.stopped = true;
    }
}


void Canvas::onMouseReleased(const MouseReleasedEvent &event, EHC &ehc) {
    if (event.button_id != MouseLeft) return;

    Vec2d global_position = ehc.stack.apply(layout->getPosition());

    palette->getCurrentTool()->onMainButton(
        CanvasTool::REALEASED, 
        event.pos - global_position + texture_offset,
        *this
    );

    ehc.stopped = true;
}


void Canvas::onKeyboardPressed(const KeyboardPressedEvent &event, EHC &ehc) {
    switch (event.key_id) {
        case Escape: 
            palette->getCurrentTool()->onCancel(); break;
        case Enter: 
            palette->getCurrentTool()->onConfirm(last_position, *this); break;
        case LShift:
        case RShift:
            palette->getCurrentTool()->onModifier1(CanvasTool::PRESSED, last_position, *this); break;
        case LControl:
        case RControl:
            palette->getCurrentTool()->onModifier2(CanvasTool::PRESSED, last_position, *this); break;
        case LAlt:
        case RAlt:
            palette->getCurrentTool()->onModifier3(CanvasTool::PRESSED, last_position, *this); break;
        default:
            break;
    }
}


void Canvas::onKeyboardReleased(const KeyboardReleasedEvent &event, EHC &ehc) {
    switch (event.key_id) {
        case LShift:
        case RShift:
            palette->getCurrentTool()->onModifier1(CanvasTool::REALEASED, last_position, *this); break;
        case LControl:
        case RControl:
            palette->getCurrentTool()->onModifier2(CanvasTool::REALEASED, last_position, *this); break;
        case LAlt:
        case RAlt:
            palette->getCurrentTool()->onModifier3(CanvasTool::REALEASED, last_position, *this); break;
        default:
            break;
    }
}


Canvas::~Canvas() {
    ASSERT(group, "Canvas is not in group!\n");
    group->removeCanvas(this);
}


// ============================================================================


FilterHotkey::FilterHotkey(Widget *parent_, FilterPalette &palette_, CanvasGroup &group_) :
    Widget(AUTO_ID, BoundLayoutBox()),
    palette(palette_), group(group_)
{}


void FilterHotkey::onKeyboardPressed(const KeyboardPressedEvent &event, EHC &ehc) {
    switch (event.key_id) {
        case F: 
            if (event.ctrl) {
                palette.getLastFilter()->applyFilter(*group.getActive());
                break;
            }
            return;
        default: return;
    }

    ehc.stopped = true;
}


// ============================================================================


#pragma GCC diagnostic pop
