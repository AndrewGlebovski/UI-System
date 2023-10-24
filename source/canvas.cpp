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
#include "button.hpp"
#include "scrollbar.hpp"
#include "ui-system.hpp"
#include "canvas.hpp"


const sf::Color PREVIEW_COLOR = sf::Color::Blue;        ///< Standart color for all previews
const float RECT_PREVIEW_OUTLINE = -1;                  ///< Rect preview outline thickness
const int ERASER_STEP = 100;                            ///< Amount of spheres that drawn between two points
const float ERASER_RADIUS = 25;                         ///< Eraser sphere radius
const sf::Color CANVAS_BACKGROUND = sf::Color::White;   ///< Canvas background color
const float POLYGON_EPSILON = 25;                       ///< Maximal distance for points of polygon to form it


/// Draws line on the texture
void drawLine(const Vector2D &p1, const Vector2D &p2, const sf::Color &color, sf::RenderTexture &result);


/// Draws polygon or polyline based on the primitive type
void drawPolygon(const Vector2D &offset, const List<Vector2D> &points, const sf::Color &color, sf::RenderTexture &result, sf::PrimitiveType type);


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


void ColorPicker::onMainButton(ButtonState state, const Vector2D &mouse, Canvas &canvas) {
    if (state == PRESSED) {
        // VERY SLOW (TEXTURE PIXELS COPIES TO AN IMAGE)
        canvas.getPalette()->setCurrentColor(canvas.getTexture().getTexture().copyToImage().getPixel(mouse.x, mouse.y));
    }
}


void BucketTool::onMainButton(ButtonState state, const Vector2D &mouse, Canvas &canvas) {
    if (state == PRESSED) {
        sf::Image image = canvas.getTexture().getTexture().copyToImage();
        sf::Color color = canvas.getPalette()->getCurrentColor();
        sf::Color origin = image.getPixel(mouse.x, mouse.y);

        List<Vector2D> dfs;
        dfs.push_back(mouse);
        image.setPixel(mouse.x, mouse.y, color);
        
        while (dfs.size()) {
            Vector2D pixel = dfs[dfs.size() - 1];
            dfs.remove(dfs.size() - 1);

            if (pixel.x > 0.5 && image.getPixel(pixel.x - 1, pixel.y) == origin) {
                dfs.push_back(Vector2D(pixel.x - 1, pixel.y));
                image.setPixel(pixel.x - 1, pixel.y, color);
            }

            if (pixel.x < canvas.getTextureSize().x - 0.5 && image.getPixel(pixel.x + 1, pixel.y) == origin) {
                dfs.push_back(Vector2D(pixel.x + 1, pixel.y));
                image.setPixel(pixel.x + 1, pixel.y, color);
            }

            if (pixel.y > 0.5 && image.getPixel(pixel.x, pixel.y - 1) == origin) {
                dfs.push_back(Vector2D(pixel.x, pixel.y - 1));
                image.setPixel(pixel.x, pixel.y - 1, color);
            }

            if (pixel.y < canvas.getTextureSize().y - 0.5 && image.getPixel(pixel.x, pixel.y + 1) == origin) {
                dfs.push_back(Vector2D(pixel.x, pixel.y + 1));
                image.setPixel(pixel.x, pixel.y + 1, color);
            }
        }

        sf::Texture tool_texture;
        tool_texture.loadFromImage(image);

        sf::Sprite tool_sprite(tool_texture);
        canvas.getTexture().draw(tool_sprite);
    }
}


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


Palette::Palette() : tools(TOOLS_SIZE, nullptr), current_tool(PENCIL_TOOL), current_color(sf::Color::Red) {
    tools[PENCIL_TOOL] = new PencilTool();
    tools[RECT_TOOL] = new RectTool();
    tools[LINE_TOOL] = new LineTool();
    tools[ERASER_TOOL] = new EraserTool();
    tools[COLOR_PICKER] = new ColorPicker();
    tools[BUCKET_TOOL] = new BucketTool();
    tools[POLYGON_TOOL] = new PolygonTool();
}


CanvasTool *Palette::getCurrentTool() {
    return tools[current_tool];
}


void Palette::setCurrentTool(size_t index) {
    if (index < TOOLS_SIZE) {
        tools[current_tool]->onCancel();
        current_tool = index;
    }
}


const sf::Color &Palette::getCurrentColor() const {
    return current_color;
}


void Palette::setCurrentColor(const sf::Color &color)  {
    current_color = color;
}


Palette::~Palette() {
    for (size_t i = 0; i < tools.size(); i++)
        delete tools[i];
}


class PaletteAction : public ButtonAction {
protected:
    Palette &palette;
    int tool_id;

public:
    PaletteAction(Palette &palette_, int tool_id_) : palette(palette_), tool_id(tool_id_) {}


    virtual void operator () () override {
        palette.setCurrentTool(tool_id);
    }


    virtual ButtonAction *clone() override {
        return new PaletteAction(palette, tool_id);
    }
};


void PaletteView::updateToolButtons() {
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


PaletteView::PaletteView(
    size_t id_, const Transform &transform_, const Vector2D &size_, int z_index_, Widget *parent_,
    Palette *palette_, const PaletteViewAsset &asset_
) :
    Widget(id_, transform_, size_, z_index_, parent_), 
    buttons(1, Transform(), size, 0, this), palette(palette_), asset(asset_), group(nullptr)
{
    group = new ButtonGroup();

    ADD_TOOL_BUTTON(Palette::PENCIL_TOOL,   PaletteViewAsset::PENCIL_TEXTURE,   Vector2D());
    ADD_TOOL_BUTTON(Palette::RECT_TOOL,     PaletteViewAsset::RECT_TEXTURE,     Vector2D(94, 0));
    ADD_TOOL_BUTTON(Palette::LINE_TOOL,     PaletteViewAsset::LINE_TEXTURE,     Vector2D(0, 94));
    ADD_TOOL_BUTTON(Palette::ERASER_TOOL,   PaletteViewAsset::ERASER_TEXTURE,   Vector2D(94, 94));
    ADD_TOOL_BUTTON(Palette::COLOR_PICKER,  PaletteViewAsset::PICKER_TEXTURE,   Vector2D(0, 188));
    ADD_TOOL_BUTTON(Palette::BUCKET_TOOL,   PaletteViewAsset::BUCKET_TEXTURE,   Vector2D(94, 188));
    ADD_TOOL_BUTTON(Palette::POLYGON_TOOL,  PaletteViewAsset::POLYGON_TEXTURE,  Vector2D(0, 282));

    updateToolButtons();
}


#undef ADD_TOOL_BUTTON


void PaletteView::draw(sf::RenderTexture &result, List<Transform> &transforms) {
    TransformApplier add_transform(transforms, transform);

    updateToolButtons();

    buttons.draw(result, transforms);
}


int PaletteView::onMouseMove(int mouse_x, int mouse_y, List<Transform> &transforms) {
    TransformApplier add_transform(transforms, transform);

    return buttons.onMouseMove(mouse_x, mouse_y, transforms);
}


int PaletteView::onMouseButtonDown(int mouse_x, int mouse_y, int button_id, List<Transform> &transforms) {
    TransformApplier add_transform(transforms, transform);

    return buttons.onMouseButtonDown(mouse_x, mouse_y, button_id, transforms);
}


int PaletteView::onMouseButtonUp(int mouse_x, int mouse_y, int button_id, List<Transform> &transforms) {
    TransformApplier add_transform(transforms, transform);

    return buttons.onMouseButtonUp(mouse_x, mouse_y, button_id, transforms);
}


int PaletteView::onKeyDown(int key_id) {
    switch(key_id) {
        case Num1: palette->setCurrentTool(Palette::PENCIL_TOOL); return HANDLED;
        case Num2: palette->setCurrentTool(Palette::RECT_TOOL); return HANDLED;
        case Num3: palette->setCurrentTool(Palette::LINE_TOOL); return HANDLED;
        case Num4: palette->setCurrentTool(Palette::ERASER_TOOL); return HANDLED;
        case Num5: palette->setCurrentTool(Palette::COLOR_PICKER); return HANDLED;
        case Num6: palette->setCurrentTool(Palette::BUCKET_TOOL); return HANDLED;
        case Num7: palette->setCurrentTool(Palette::POLYGON_TOOL); return HANDLED;
        default: return UNHANDLED;
    }
}


PaletteView::~PaletteView() {
    if (group) delete group;
}


void Canvas::clear_canvas() {
    texture.clear(CANVAS_BACKGROUND);
}


Canvas::Canvas(
    size_t id_, const Transform &transform_, const Vector2D &size_, int z_index_, Widget *parent_,
    const char *image_path, Palette *palette_
) :
    Widget(id_, transform_, size_, z_index_, parent_),
    texture(), texture_offset(Vector2D(0, 0)),
    palette(palette_), last_position()
{
    Vector2D texture_size = size;

    if (image_path) {
        sf::Texture image;
        if (image.loadFromFile(image_path)) {
            if (texture_size.x < image.getSize().x) texture_size.x = image.getSize().x;
            if (texture_size.y < image.getSize().y) texture_size.y = image.getSize().y;

            texture.create(texture_size.x, texture_size.y);
            clear_canvas();

            sf::Sprite tool_sprite(image);
            tool_sprite.setPosition(Vector2D());

            texture.draw(tool_sprite);

            return;
        }
    }

    texture.create(size.x, size.y);
    clear_canvas();
}


Vector2D Canvas::getTextureSize() const {
    return Vector2D(texture.getSize().x, texture.getSize().y);
}


sf::RenderTexture &Canvas::getTexture() {
    return texture;
}


Palette *Canvas::getPalette() {
    return palette;
}


void Canvas::draw(sf::RenderTexture &result, List<Transform> &transforms) {
    TransformApplier add_transform(transforms, transform);

    texture.display();

    sf::Sprite tool_sprite(texture.getTexture());
    sf::Vector2i offset_(texture_offset.x, texture_offset.y);
    sf::Vector2i size_(size.x, size.y);
    tool_sprite.setTextureRect(sf::IntRect(offset_, size_));
    tool_sprite.setPosition(transforms.front().offset);

    result.draw(tool_sprite);

    if (palette->getCurrentTool()->getWidget()) {
        TransformApplier texture_transform(transforms, texture_offset);
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

    if (palette->getCurrentTool()->getWidget())
        palette->getCurrentTool()->getWidget()->onMouseMove(mouse_x, mouse_y, transforms);

    return UNHANDLED;
}


int Canvas::onMouseButtonDown(int mouse_x, int mouse_y, int button_id, List<Transform> &transforms) {
    if (button_id != MouseLeft) return UNHANDLED;

    TransformApplier add_transform(transforms, transform);

    if (isInsideRect(transforms.front().offset, size, last_position)) {
        palette->getCurrentTool()->onMainButton(
            CanvasTool::PRESSED, 
            last_position - transforms.front().offset + texture_offset,
            *this
        );

        if (palette->getCurrentTool()->getWidget()) {
            TransformApplier texture_transform(transforms, texture_offset);
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
        TransformApplier texture_transform(transforms, texture_offset);
        palette->getCurrentTool()->getWidget()->onMouseButtonUp(mouse_x, mouse_y, button_id, transforms);
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
            palette->getCurrentTool()->onCancel(); return HANDLED;
        case LShift:
        case RShift:
            palette->getCurrentTool()->onModifier1(CanvasTool::PRESSED, last_position, *this); return HANDLED;
        case LControl:
        case RControl:
            palette->getCurrentTool()->onModifier2(CanvasTool::PRESSED, last_position, *this); return HANDLED;
        case LAlt:
        case RAlt:
            palette->getCurrentTool()->onModifier3(CanvasTool::PRESSED, last_position, *this); return HANDLED;
        default:
            if (palette->getCurrentTool()->getWidget())
                return palette->getCurrentTool()->getWidget()->onKeyDown(key_id);
            return UNHANDLED;
    }
}


int Canvas::onKeyUp(int key_id) {
    switch (key_id) {
        case LShift:
        case RShift:
            palette->getCurrentTool()->onModifier1(CanvasTool::REALEASED, last_position, *this); return HANDLED;
        case LControl:
        case RControl:
            palette->getCurrentTool()->onModifier2(CanvasTool::REALEASED, last_position, *this); return HANDLED;
        case LAlt:
        case RAlt:
            palette->getCurrentTool()->onModifier3(CanvasTool::REALEASED, last_position, *this); return HANDLED;
        default:
            if (palette->getCurrentTool()->getWidget())
                return palette->getCurrentTool()->getWidget()->onKeyUp(key_id);
            return UNHANDLED;
    }
}


#pragma GCC diagnostic pop
