/**
 * \file
 * \brief Contains implementation of some canvas tools
*/


#include <SFML/Graphics.hpp>
#include "canvas/tool.hpp"


// ============================================================================


const sf::Color PREVIEW_COLOR = sf::Color::Blue;        ///< Standart color for all previews
const float RECT_PREVIEW_OUTLINE = -1;                  ///< Rect preview outline thickness
const int ERASER_STEP = 100;                            ///< Amount of spheres that drawn between two points
const float ERASER_RADIUS = 25;                         ///< Eraser sphere radius
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
