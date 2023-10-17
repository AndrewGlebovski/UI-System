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
#include "style.hpp"
#include "ui-base.hpp"
#include "button.hpp"
#include "scrollbar.hpp"
#include "ui-system.hpp"
#include "canvas.hpp"


const sf::Color PREVIEW_COLOR = sf::Color::Blue;        ///< Standart color for all previews
const float RECT_PREVIEW_OUTLINE = -1;                  ///< Rect preview outline thickness
const int ERASER_STEP = 100;                            ///< Amount of spheres that drawn between two points
const float ERASER_RADIUS = 25;                         ///< Eraser sphere radius
const sf::Color CANVAS_BACKGROUND = sf::Color::White;   ///< Canvas background color


/// Draws line on the texture
void drawLine(const Vector2D &p1, const Vector2D &p2, const sf::Color &color, sf::RenderTexture &result);


void drawLine(const Vector2D &p1, const Vector2D &p2, const sf::Color &color, sf::RenderTexture &result) {
    sf::Vertex line[] = {
        sf::Vertex(p1, color),
        sf::Vertex(p2, color)
    };

    result.draw(line, 2, sf::Lines);
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
class RectPreview : public BaseUI {
private:
    RectTool &tool;     ///< Tool that holds this object

public:
    RectPreview(
        const Vector2D &position_, const Vector2D &size_, int z_index_, BaseUI *parent_,
        RectTool &tool_
    ) :
        BaseUI(position_, size_, z_index_, parent_), tool(tool_)
    {}
    

    virtual void draw(sf::RenderTexture &result, List<Vector2D> &transforms) override {
        TransformApplier add_transform(transforms, position);

        sf::RectangleShape rect(size);
        rect.setPosition(transforms[0]);
        rect.setFillColor(sf::Color(0));
        rect.setOutlineThickness(RECT_PREVIEW_OUTLINE);
        rect.setOutlineColor(PREVIEW_COLOR);
        result.draw(rect);
    }
};


RectTool::RectTool() : draw_start(), rect_preview(nullptr) {
    rect_preview = new RectPreview(Vector2D(), Vector2D(), 1, nullptr, *this);
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
            rect_preview->position = mouse;
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
    rect_preview->position = rect.getPosition();
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


BaseUI *RectTool::getWidget() {
    return (is_drawing) ? rect_preview : nullptr;
}


RectTool::~RectTool() {
    delete rect_preview;
}


/// Draws preview of the rectangle
class LinePreview : public BaseUI {
private:
    LineTool &tool;     ///< Tool that holds this object

public:
    /**
     * \note Position used as line's first point and size as second
    */
    LinePreview(
        const Vector2D &point1_, const Vector2D &point2_, int z_index_, BaseUI *parent_,
        LineTool &tool_
    ) :
        BaseUI(point1_, point2_, z_index_, parent_), tool(tool_)
    {}
    

    virtual void draw(sf::RenderTexture &result, List<Vector2D> &transforms) override {
        // REMAINDER: position = point1, size = point2
        drawLine(transforms[0] + position, transforms[0] + size, PREVIEW_COLOR, result);
    }
};


LineTool::LineTool() : draw_start(), line_preview(nullptr) {
    line_preview = new LinePreview(Vector2D(), Vector2D(), 1, nullptr, *this);
}


void LineTool::onMainButton(ButtonState state, const Vector2D &mouse, Canvas &canvas) {
    switch (state) {
        case PRESSED:
            is_drawing = true;
            draw_start = mouse;
            line_preview->position = mouse;
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


BaseUI *LineTool::getWidget() {
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


Palette::Palette() : tools(), current_tool(0), current_color(sf::Color::Red) {
    tools.push_back(new PencilTool());
    tools.push_back(new RectTool());
    tools.push_back(new LineTool());
    tools.push_back(new EraserTool());
    tools.push_back(new ColorPicker());
}


CanvasTool *Palette::getCurrentTool() {
    return tools[current_tool];
}


void Palette::setCurrentTool(size_t index) {
    // NEED TO CANCEL CURRENT TOOL BEFORE SWITCHING
    current_tool = index;
}


const sf::Color &Palette::getCurrentColor() const {
    return current_color;
}


void Palette::setCurrentColor(const sf::Color &color)  {
    current_color = color;
}


Palette::~Palette() {
    for (size_t i = 0; i < tools.getSize(); i++)
        delete tools[i];
}


class PaletteAction : public ButtonAction {
protected:
    Palette &palette;
    int tool_id;

public:
    PaletteAction(Palette &palette_, int tool_id_) : palette(palette_), tool_id(tool_id_) {}


    void operator () () {
        palette.setCurrentTool(tool_id);
    }
};


PaletteView::PaletteView(
    const Vector2D &position_, const Vector2D &size_, int z_index_, BaseUI *parent_,
    Palette *palette_, const PaletteViewAsset &asset_
) : 
    BaseUI(position_, size_, z_index_, parent_), 
    buttons(Vector2D(), size, 0, this), palette(palette_), asset(asset_)
{
    buttons.addElement(new TextureButton(
        Vector2D(),
        0,
        nullptr,
        new PaletteAction(*palette, 0),
        asset[PaletteViewAsset::PENCIL],
        asset[PaletteViewAsset::PENCIL]
    ));

    buttons.addElement(new TextureButton(
        Vector2D(94, 0),
        0,
        nullptr,
        new PaletteAction(*palette, 1),
        asset[PaletteViewAsset::RECT],
        asset[PaletteViewAsset::RECT]
    ));

    buttons.addElement(new TextureButton(
        Vector2D(0, 94),
        0,
        nullptr,
        new PaletteAction(*palette, 2),
        asset[PaletteViewAsset::LINE],
        asset[PaletteViewAsset::LINE]
    ));

    buttons.addElement(new TextureButton(
        Vector2D(94, 94),
        0,
        nullptr,
        new PaletteAction(*palette, 3),
        asset[PaletteViewAsset::ERASER],
        asset[PaletteViewAsset::ERASER]
    ));

    buttons.addElement(new TextureButton(
        Vector2D(0, 188),
        0,
        nullptr,
        new PaletteAction(*palette, 4),
        asset[PaletteViewAsset::PICKER],
        asset[PaletteViewAsset::PICKER]
    ));
}


void PaletteView::draw(sf::RenderTexture &result, List<Vector2D> &transforms) {
    TransformApplier add_transform(transforms, position);

    buttons.draw(result, transforms);
}


int PaletteView::onMouseMove(int mouse_x, int mouse_y, List<Vector2D> &transforms) {
    TransformApplier add_transform(transforms, position);

    return buttons.onMouseMove(mouse_x, mouse_y, transforms);
}


int PaletteView::onMouseButtonDown(int mouse_x, int mouse_y, int button_id, List<Vector2D> &transforms) {
    TransformApplier add_transform(transforms, position);

    return buttons.onMouseButtonDown(mouse_x, mouse_y, button_id, transforms);
}


int PaletteView::onMouseButtonUp(int mouse_x, int mouse_y, int button_id, List<Vector2D> &transforms) {
    TransformApplier add_transform(transforms, position);

    return buttons.onMouseButtonUp(mouse_x, mouse_y, button_id, transforms);
}


void Canvas::clear_canvas() {
    texture.clear(CANVAS_BACKGROUND);
}


Canvas::Canvas(
    const Vector2D &position_, const Vector2D &size_, int z_index_, BaseUI *parent_,
    const char *image_path, Palette *palette_
) :
    BaseUI(position_, size_, z_index_, parent_),
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


void Canvas::draw(sf::RenderTexture &result, List<Vector2D> &transforms) {
    TransformApplier add_transform(transforms, position);

    texture.display();

    sf::Sprite tool_sprite(texture.getTexture());
    sf::Vector2i offset_(texture_offset.x, texture_offset.y);
    sf::Vector2i size_(size.x, size.y);
    tool_sprite.setTextureRect(sf::IntRect(offset_, size_));
    tool_sprite.setPosition(transforms[0]);

    result.draw(tool_sprite);

    if (palette->getCurrentTool()->getWidget()) {
        TransformApplier texture_transform(transforms, texture_offset);
        palette->getCurrentTool()->getWidget()->draw(result, transforms);
    }
}


int Canvas::onMouseMove(int mouse_x, int mouse_y, List<Vector2D> &transforms) {
    TransformApplier add_transform(transforms, position);

    last_position = Vector2D(mouse_x, mouse_y);

    palette->getCurrentTool()->onMove(
        last_position - transforms[0] + texture_offset,
        *this
    );

    if (palette->getCurrentTool()->getWidget())
        palette->getCurrentTool()->getWidget()->onMouseMove(mouse_x, mouse_y, transforms);

    return UNHANDLED;
}


int Canvas::onMouseButtonDown(int mouse_x, int mouse_y, int button_id, List<Vector2D> &transforms) {
    if (button_id != MouseLeft) return UNHANDLED;

    TransformApplier add_transform(transforms, position);

    if (isInsideRect(transforms[0], size, last_position)) {
        palette->getCurrentTool()->onMainButton(
            CanvasTool::PRESSED, 
            last_position - transforms[0] + texture_offset,
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


int Canvas::onMouseButtonUp(int mouse_x, int mouse_y, int button_id, List<Vector2D> &transforms) {
    if (button_id != MouseLeft) return UNHANDLED;

    TransformApplier add_transform(transforms, position);

    palette->getCurrentTool()->onMainButton(
        CanvasTool::REALEASED, 
        last_position - transforms[0] + texture_offset,
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

    setSize(new_size);

    return UNHANDLED;
}


int Canvas::onKeyDown(int key_id) {
    switch (key_id) {
        case Escape: 
            palette->getCurrentTool()->onCancel(last_position, *this); return HANDLED;
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
