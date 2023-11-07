/**
 * \file
 * \brief Contains sources of canvas and his tools classes functions
*/


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wfloat-conversion"


#include <SFML/Graphics.hpp>
#include "vector.hpp"
#include "list.hpp"
#include "configs.hpp"
#include "key-id.hpp"
#include "asset.hpp"
#include "widget.hpp"
#include "container.hpp"
#include "button.hpp"
#include "scrollbar.hpp"
#include "line-edit.hpp"
#include "canvas.hpp"


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
void drawLine(const Vector2D &p1, const Vector2D &p2, const sf::Color &color, sf::RenderTexture &result);


/// Draws polygon or polyline based on the primitive type
void drawPolygon(const Vector2D &offset, const List<Vector2D> &points, const sf::Color &color, sf::RenderTexture &result, sf::PrimitiveType type);


// ============================================================================


void drawLine(const Vector2D &p1, const Vector2D &p2, const sf::Color &color, sf::RenderTexture &result) {
    sf::Vertex line[] = {
        sf::Vertex(p1, color),
        sf::Vertex(p2, color)
    };

    result.draw(line, 2, sf::Lines);
}


void drawPolygon(const Vector2D &offset, const List<Vector2D> &points, const sf::Color &color, sf::RenderTexture &result, sf::PrimitiveType type) {
    sf::Vertex *polygon = new sf::Vertex[points.size()];

    for (size_t i = 0; i < points.size(); i++)
        polygon[i] = sf::Vertex(points[i] + offset, color);

    result.draw(polygon, points.size(), type);

    delete polygon;
}


// ============================================================================


PencilTool::PencilTool() : prev_position() {}


void PencilTool::onMainButton(ButtonState state, const Vector2D &mouse, Canvas &canvas) {
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


void PencilTool::onMove(const Vector2D &mouse, Canvas &canvas) {
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
        Widget(1, Transform(Vector2D()), Vector2D(), 1, nullptr), tool(tool_) {}
    

    virtual void draw(sf::RenderTexture &result, List<Transform> &transforms) override {
        TransformApplier add_transform(transforms, transform);

        sf::RectangleShape rect(size);
        rect.setPosition(transforms.front().offset);
        rect.setFillColor(sf::Color(0));
        rect.setOutlineThickness(RECT_PREVIEW_OUTLINE);
        rect.setOutlineColor(PREVIEW_COLOR);
        result.draw(rect);
    }
};


RectTool::RectTool() : draw_start(), rect_preview(nullptr) {
    rect_preview = new RectPreview(*this);
}


sf::RectangleShape RectTool::createRect(const Vector2D &p1, const Vector2D &p2) const {
    Vector2D position = p1;
    Vector2D size = p2 - p1;
    if (position.x > p2.x) position.x = p2.x;
    if (position.y > p2.y) position.y = p2.y;
    if (size.x < 0) size.x *= -1;
    if (size.y < 0) size.y *= -1;

    sf::RectangleShape rect(size);
    rect.setPosition(position);

    return rect;
}


void RectTool::onMainButton(ButtonState state, const Vector2D &mouse, Canvas &canvas) {
    switch (state) {
        case PRESSED:
            is_drawing = true;
            draw_start = mouse;
            rect_preview->transform.offset = mouse;
            rect_preview->size = Vector2D();
            break;
        case REALEASED:
            onConfirm(mouse, canvas);
        default:
            break;
    }
}


void RectTool::onMove(const Vector2D &mouse, Canvas &canvas) {
    sf::RectangleShape rect = createRect(draw_start, mouse);
    rect_preview->transform.offset = rect.getPosition();
    rect_preview->size = rect.getSize();
}


void RectTool::onConfirm(const Vector2D &mouse, Canvas &canvas) {
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
        Widget(1, Transform(Vector2D()), Vector2D(), 1, nullptr), tool(tool_) {}
    

    /**
     * \note Transform offset used as line's first point and size as second
    */
    virtual void draw(sf::RenderTexture &result, List<Transform> &transforms) override {
        // REMAINDER: transform.offset = point1, size = point2
        drawLine(transforms.front().offset + transform.offset, transforms.front().offset + size, PREVIEW_COLOR, result);
    }
};


LineTool::LineTool() : draw_start(), line_preview(nullptr) {
    line_preview = new LinePreview(*this);
}


void LineTool::onMainButton(ButtonState state, const Vector2D &mouse, Canvas &canvas) {
    switch (state) {
        case PRESSED:
            is_drawing = true;
            draw_start = mouse;
            line_preview->transform.offset = mouse;
            line_preview->size = mouse;
            break;
        case REALEASED:
            onConfirm(mouse, canvas);
        default:
            break;
    }
}


void LineTool::onMove(const Vector2D &mouse, Canvas &canvas) {
    line_preview->size = mouse;
}


void LineTool::onConfirm(const Vector2D &mouse, Canvas &canvas) {
    if (is_drawing) {
        drawLine(draw_start, mouse, canvas.getPalette()->getCurrentColor(), canvas.getTexture());
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


void EraserTool::onMainButton(ButtonState state, const Vector2D &mouse, Canvas &canvas) {
    switch (state) {
        case PRESSED: {
            is_drawing = true;
            prev_position = mouse;

            sf::CircleShape circle(ERASER_RADIUS);
            circle.setFillColor(CANVAS_BACKGROUND);
            circle.setPosition(mouse - Vector2D(ERASER_RADIUS, ERASER_RADIUS));
            canvas.getTexture().draw(circle);

            break;
        }
        case REALEASED:
            is_drawing = false;
        default:
            break;
    }
}


void EraserTool::onMove(const Vector2D &mouse, Canvas &canvas) {
    if (is_drawing) {
        Vector2D step(
            (mouse.x - prev_position.x) / ERASER_STEP, 
            (mouse.y - prev_position.y) / ERASER_STEP
        );

        sf::CircleShape circle(ERASER_RADIUS);
        circle.setFillColor(CANVAS_BACKGROUND);
        circle.setPosition(mouse - Vector2D(ERASER_RADIUS, ERASER_RADIUS));

        for (int i = 0; i < ERASER_STEP; i++) {
            canvas.getTexture().draw(circle);
            circle.setPosition(circle.getPosition() + step);
        }

        prev_position = mouse;
    }
}


// ============================================================================


void ColorPicker::onMainButton(ButtonState state, const Vector2D &mouse, Canvas &canvas) {
    if (state == PRESSED) {
        // VERY SLOW (TEXTURE PIXELS COPIES TO AN IMAGE)
        canvas.getPalette()->setCurrentColor(canvas.getTexture().getTexture().copyToImage().getPixel(mouse.x, mouse.y));
    }
}


// ============================================================================


void BucketTool::onMainButton(ButtonState state, const Vector2D &mouse, Canvas &canvas) {
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
        Widget(1, Transform(), Vector2D(), 1, nullptr), tool(tool_) {}
    

    virtual void draw(sf::RenderTexture &result, List<Transform> &transforms) override {
        drawPolygon(transforms.front().offset, tool.getPoints(), PREVIEW_COLOR, result, sf::PrimitiveType::LineStrip);
    }
};


PolygonTool::PolygonTool() : points(), polygon_preview(nullptr) {
    polygon_preview = new PolygonPreview(*this);
}


List<Vector2D> &PolygonTool::getPoints() {
    return points;
}


void PolygonTool::onMainButton(ButtonState state, const Vector2D &mouse, Canvas &canvas) {
    if (state == PRESSED) {
        is_drawing = true;
        if (points.size() && (points[0] - mouse).length() < POLYGON_EPSILON)
            onConfirm(mouse, canvas);
        else
            points.push_back(mouse);
    }
}


void PolygonTool::onConfirm(const Vector2D &mouse, Canvas &canvas) {
    if (is_drawing) {
        drawPolygon(Vector2D(), points, canvas.getPalette()->getCurrentColor(), canvas.getTexture(), sf::PrimitiveType::TriangleFan);
        is_drawing = false;
    }

    points.resize(0, Vector2D());
}


void PolygonTool::onCancel() {
    is_drawing = false;
    points.resize(0, Vector2D());
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
        Transform(),
        Vector2D(SCREEN_W, TEXT_SIZE + TEXT_OFFSET * 2),
        0,
        nullptr,
        style,
        TEXT_MAX_LENGTH
    );
}


void TextTool::onMainButton(ButtonState state, const Vector2D &mouse, Canvas &canvas) {
    if (state == PRESSED) {
        is_drawing = true;
        text_preview->transform.offset = mouse;
        text_preview->setKeyboardFocus(true);
    }
}


void TextTool::onConfirm(const Vector2D &mouse, Canvas &canvas) {
    if (is_drawing) {
        sf::Text text(text_preview->getString(), text_font, TEXT_SIZE);
        text.setPosition(text_preview->transform.offset + Vector2D(TEXT_OFFSET, TEXT_OFFSET));
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
buttons.addChild(new TextureIconButton(                         \
    Widget::AUTO_ID + TOOL_ID + 1,                              \
    Transform(POSITION),                                        \
    0,                                                          \
    nullptr,                                                    \
    new PaletteAction(*palette, TOOL_ID),                       \
    group,                                                      \
    asset[PaletteViewAsset::NORMAL_TEXTURE],                    \
    asset[PaletteViewAsset::NORMAL_TEXTURE],                    \
    asset[PaletteViewAsset::SELECTED_TEXTURE],                  \
    asset[TOOL_TEXTURE_ID]                                      \
))


ToolPaletteView::ToolPaletteView(
    size_t id_, const Transform &transform_, const Vector2D &size_, int z_index_, Widget *parent_,
    ToolPalette *palette_, const PaletteViewAsset &asset_
) :
    Widget(id_, transform_, size_, z_index_, parent_), 
    buttons(1, Transform(), size, 0, this), palette(palette_), asset(asset_), group(nullptr)
{
    group = new ButtonGroup();

    ADD_TOOL_BUTTON(ToolPalette::PENCIL_TOOL,   PaletteViewAsset::PENCIL_TEXTURE,   Vector2D());
    ADD_TOOL_BUTTON(ToolPalette::RECT_TOOL,     PaletteViewAsset::RECT_TEXTURE,     Vector2D(94, 0));
    ADD_TOOL_BUTTON(ToolPalette::LINE_TOOL,     PaletteViewAsset::LINE_TEXTURE,     Vector2D(0, 94));
    ADD_TOOL_BUTTON(ToolPalette::ERASER_TOOL,   PaletteViewAsset::ERASER_TEXTURE,   Vector2D(94, 94));
    ADD_TOOL_BUTTON(ToolPalette::COLOR_PICKER,  PaletteViewAsset::PICKER_TEXTURE,   Vector2D(0, 188));
    ADD_TOOL_BUTTON(ToolPalette::BUCKET_TOOL,   PaletteViewAsset::BUCKET_TEXTURE,   Vector2D(94, 188));
    ADD_TOOL_BUTTON(ToolPalette::POLYGON_TOOL,  PaletteViewAsset::POLYGON_TEXTURE,  Vector2D(0, 282));
    ADD_TOOL_BUTTON(ToolPalette::TEXT_TOOL,     PaletteViewAsset::TEXT_TEXTURE,     Vector2D(94, 282));

    updateToolButtons();
}


#undef ADD_TOOL_BUTTON


void ToolPaletteView::draw(sf::RenderTexture &result, List<Transform> &transforms) {
    TransformApplier add_transform(transforms, transform);

    updateToolButtons();

    buttons.draw(result, transforms);
}


int ToolPaletteView::onMouseMove(int mouse_x, int mouse_y, List<Transform> &transforms) {
    TransformApplier add_transform(transforms, transform);

    return buttons.onMouseMove(mouse_x, mouse_y, transforms);
}


int ToolPaletteView::onMouseButtonDown(int mouse_x, int mouse_y, int button_id, List<Transform> &transforms) {
    TransformApplier add_transform(transforms, transform);

    return buttons.onMouseButtonDown(mouse_x, mouse_y, button_id, transforms);
}


int ToolPaletteView::onMouseButtonUp(int mouse_x, int mouse_y, int button_id, List<Transform> &transforms) {
    TransformApplier add_transform(transforms, transform);

    return buttons.onMouseButtonUp(mouse_x, mouse_y, button_id, transforms);
}


int ToolPaletteView::onKeyDown(int key_id) {
    switch(key_id) {
        case Num1: palette->setCurrentTool(ToolPalette::PENCIL_TOOL); return HANDLED;
        case Num2: palette->setCurrentTool(ToolPalette::RECT_TOOL); return HANDLED;
        case Num3: palette->setCurrentTool(ToolPalette::LINE_TOOL); return HANDLED;
        case Num4: palette->setCurrentTool(ToolPalette::ERASER_TOOL); return HANDLED;
        case Num5: palette->setCurrentTool(ToolPalette::COLOR_PICKER); return HANDLED;
        case Num6: palette->setCurrentTool(ToolPalette::BUCKET_TOOL); return HANDLED;
        case Num7: palette->setCurrentTool(ToolPalette::POLYGON_TOOL); return HANDLED;
        case Num8: palette->setCurrentTool(ToolPalette::TEXT_TOOL); return HANDLED;
        default: return UNHANDLED;
    }
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


FilterPalette::FilterPalette() : filters(FILTERS_SIZE, nullptr), last_filter(0) {
    filters[LIGHTEN_FILTER] = new IntensityFilter(20);
    filters[DARKEN_FILTER] = new IntensityFilter(-20);
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
        if (index == active) setActive(canvases[0]);
        canvases.remove(index);
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
    size_t id_, const Transform &transform_, const Vector2D &size_, int z_index_, Widget *parent_,
    ToolPalette &palette_, CanvasGroup &group_
) :
    Widget(id_, transform_, size_, z_index_, parent_),
    texture(), texture_offset(Vector2D(0, 0)),
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
        tool_sprite.setPosition(Vector2D());

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


Vector2D Canvas::getTextureSize() const {
    return Vector2D(texture.getSize().x, texture.getSize().y);
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


void Canvas::draw(sf::RenderTexture &result, List<Transform> &transforms) {
    TransformApplier add_transform(transforms, transform);

    texture.display();

    sf::Sprite tool_sprite(texture.getTexture());
    sf::Vector2i offset_(texture_offset.x, texture_offset.y);
    sf::Vector2i size_(texture.getSize().x, texture.getSize().y);
    if (size_.x > size.x) size_.x = size.x;
    if (size_.y > size.y) size_.y = size.y;
    tool_sprite.setTextureRect(sf::IntRect(offset_, size_));
    tool_sprite.setPosition(transforms.front().offset);

    result.draw(tool_sprite);

    if (isActive() && palette->getCurrentTool()->getWidget()) {
        TransformApplier texture_transform(transforms, Transform(-texture_offset));
        palette->getCurrentTool()->getWidget()->draw(result, transforms);
    }
}


int Canvas::onMouseMove(int mouse_x, int mouse_y, List<Transform> &transforms) {
    TransformApplier add_transform(transforms, transform);

    last_position = Vector2D(mouse_x, mouse_y);

    palette->getCurrentTool()->onMove(
        last_position - transforms.front().offset + texture_offset,
        *this
    );

    if (palette->getCurrentTool()->getWidget()) {
        TransformApplier texture_transform(transforms, -texture_offset);
        return palette->getCurrentTool()->getWidget()->onMouseMove(mouse_x, mouse_y, transforms);
    }

    return UNHANDLED;
}


int Canvas::onMouseButtonDown(int mouse_x, int mouse_y, int button_id, List<Transform> &transforms) {
    if (button_id != MouseLeft) return UNHANDLED;

    TransformApplier add_transform(transforms, transform);

    if (isInsideRect(transforms.front().offset, size, last_position)) {
        ASSERT(group, "Canvas must be assigned to group!\n");
        group->setActive(this);

        palette->getCurrentTool()->onMainButton(
            CanvasTool::PRESSED, 
            last_position - transforms.front().offset + texture_offset,
            *this
        );

        if (palette->getCurrentTool()->getWidget()) {
            TransformApplier texture_transform(transforms, -texture_offset);
            palette->getCurrentTool()->getWidget()->onMouseButtonDown(mouse_x, mouse_y, button_id, transforms);
        }
        
        return HANDLED;
    }

    return UNHANDLED;
}


int Canvas::onMouseButtonUp(int mouse_x, int mouse_y, int button_id, List<Transform> &transforms) {
    if (button_id != MouseLeft) return UNHANDLED;

    TransformApplier add_transform(transforms, transform);

    palette->getCurrentTool()->onMainButton(
        CanvasTool::REALEASED, 
        last_position - transforms.front().offset + texture_offset,
        *this
    );

    if (palette->getCurrentTool()->getWidget()) {
        TransformApplier texture_transform(transforms, -texture_offset);
        return palette->getCurrentTool()->getWidget()->onMouseButtonUp(mouse_x, mouse_y, button_id, transforms);
    }

    return UNHANDLED;
}


int Canvas::onParentResize() {
    Vector2D new_size = texture.getSize();
    if (new_size.x > parent->size.x - 30) new_size.x = parent->size.x - 30;
    if (new_size.y > parent->size.y - 30) new_size.y = parent->size.y - 30;

    tryResize(new_size);

    return UNHANDLED;
}


int Canvas::onKeyDown(int key_id) {
    switch (key_id) {
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

    if (palette->getCurrentTool()->getWidget())
        return palette->getCurrentTool()->getWidget()->onKeyDown(key_id);
    
    return UNHANDLED;
}


int Canvas::onKeyUp(int key_id) {
    switch (key_id) {
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

    if (palette->getCurrentTool()->getWidget())
        return palette->getCurrentTool()->getWidget()->onKeyUp(key_id);
    
    return UNHANDLED;
}


int Canvas::onTimer(float delta_time) {
    if (palette->getCurrentTool()->getWidget())
        palette->getCurrentTool()->getWidget()->onTimer(delta_time);
    
    return UNHANDLED;
}


Canvas::~Canvas() {
    ASSERT(group, "Canvas is not in group!\n");
    group->removeCanvas(this);
}


// ============================================================================


FilterHotkey::FilterHotkey(Widget *parent_, FilterPalette &palette_, CanvasGroup &group_) :
    Widget(AUTO_ID, Transform(), Vector2D(), 0, parent_),
    palette(palette_), group(group_), ctrl_pressed(false)
{}


int FilterHotkey::onKeyDown(int key_id) {
    switch (key_id) {
        case F: 
            if (ctrl_pressed) {
                palette.getLastFilter()->applyFilter(*group.getActive());
                return HANDLED;
            }

            return UNHANDLED;
        case LControl:
        case RControl:
            ctrl_pressed = true; return HANDLED;
        default:
            return UNHANDLED;
    }
}


int FilterHotkey::onKeyUp(int key_id) {
    switch (key_id) {
        case LControl:
        case RControl:
            ctrl_pressed = false; return HANDLED;
        default:
            return UNHANDLED;
    }
}


// ============================================================================


#pragma GCC diagnostic pop
