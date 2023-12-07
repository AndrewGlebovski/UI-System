/**
 * \file
 * \brief Contains implementation of some canvas tools
*/


#include <cstring>
#include "canvas/tools/tools.hpp"
#include "common/utils.hpp"


// ============================================================================


const plug::Color PREVIEW_COLOR = Blue;                       ///< Standart color for all previews
const float RECT_PREVIEW_OUTLINE = 1;                   ///< Rect preview outline thickness
const int ERASER_STEP = 100;                            ///< Amount of spheres that drawn between two points
const float ERASER_RADIUS = 25;                         ///< Eraser sphere radius
const float POLYGON_EPSILON = 25;                       ///< Maximal distance for points of polygon to form it
const unsigned TEXT_SIZE = 20;                          ///< Text tool font size
const size_t TEXT_MAX_LENGTH = 256;                     ///< Text tool text max length


// ============================================================================


/// Creates vertex array for polygon
plug::VertexArray getPolygonArray(
    const plug::Transform &trans,
    const List<plug::Vec2d> &points,
    plug::Color color,
    plug::PrimitiveType type
);


// ============================================================================


plug::VertexArray getPolygonArray(
    const plug::Transform &trans,
    const List<plug::Vec2d> &points,
    plug::Color color,
    plug::PrimitiveType type
) {
    plug::VertexArray array(type, points.size());

    for (size_t i = 0; i < points.size(); i++)
        array[i] = plug::Vertex(trans.apply(points[i]), color);

    return array;
}


// ============================================================================


PencilTool::PencilTool() : prev_position() {}


void PencilTool::onMainButton(const plug::ControlState &state, const plug::Vec2d &mouse) {
    switch (state.state) {
        case plug::State::Pressed:
            is_drawing = true;
            prev_position = mouse;
            break;
        case plug::State::Released:
            is_drawing = false;
        default:
            break;
    }
}


void PencilTool::onMove(const plug::Vec2d &mouse) {
    if (is_drawing) {
        plug::VertexArray array(plug::Lines, 2);

        array[0] = plug::Vertex(prev_position, color_palette->getFGColor());
        array[1] = plug::Vertex(mouse, color_palette->getFGColor());
        
        prev_position = mouse;

        ASSERT(canvas, "Canvas is nullptr!\n");
        canvas->draw(array);
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
    

    virtual void draw(plug::TransformStack &stack, plug::RenderTarget &result) override {
        plug::Vec2d global_position = stack.apply(layout->getPosition());
        plug::Vec2d global_size = applySize(stack, layout->getSize());

        RectShape rect(global_position, global_size, hex2Color(0));
        rect.setBorder(RECT_PREVIEW_OUTLINE, PREVIEW_COLOR);
        rect.draw(result);
    }
};


RectTool::RectTool() : draw_start(), rect_preview(nullptr) {
    rect_preview = new RectPreview(*this);
}


RectShape RectTool::createRect(const plug::Vec2d &p1, const plug::Vec2d &p2) const {
    plug::Vec2d position = p1;
    plug::Vec2d size = p2 - p1;
    if (position.x > p2.x) position.x = p2.x;
    if (position.y > p2.y) position.y = p2.y;
    if (size.x < 0) size.x *= -1;
    if (size.y < 0) size.y *= -1;

    return RectShape(position, size, plug::Color());
}


void RectTool::onMainButton(const plug::ControlState &state, const plug::Vec2d &mouse) {
    switch (state.state) {
        case plug::State::Pressed:
            is_drawing = true;
            draw_start = mouse;
            rect_preview->getLayoutBox().setPosition(mouse);
            rect_preview->getLayoutBox().setSize(plug::Vec2d());
            break;
        case plug::State::Released:
            onConfirm();
        default:
            break;
    }
}


void RectTool::onMove(const plug::Vec2d &mouse) {
    RectShape rect = createRect(draw_start, mouse);
    rect_preview->getLayoutBox().setPosition(rect.getPosition());
    rect_preview->getLayoutBox().setSize(rect.getSize());
}


void RectTool::onConfirm() {
    if (is_drawing) {
        RectShape rect(
            rect_preview->getLayoutBox().getPosition(),
            rect_preview->getLayoutBox().getSize(),
            hex2Color(0)
        );
        rect.setBorder(RECT_PREVIEW_OUTLINE, color_palette->getFGColor());
        rect.draw(*canvas);

        is_drawing = false;
    }
}


plug::Widget *RectTool::getWidget() {
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
     * \note plug::Transform offset used as line's first point and size as second
    */
    LinePreview(LineTool &tool_) :
        Widget(1, LazyLayoutBox()), tool(tool_) {}
    

    /**
     * \note plug::Transform offset used as line's first point and size as second
    */
    virtual void draw(plug::TransformStack &stack, plug::RenderTarget &result) override {
        // REMAINDER: transform.offset = point1, size = point2
        plug::Vec2d p1 = stack.apply(layout->getPosition());
        plug::Vec2d p2 = stack.apply(layout->getSize());

        plug::VertexArray array(plug::Lines, 2);

        array[0] = plug::Vertex(p1, PREVIEW_COLOR);
        array[1] = plug::Vertex(p2, PREVIEW_COLOR);

        result.draw(array);
    }
};


LineTool::LineTool() : line_preview(nullptr) {
    line_preview = new LinePreview(*this);
}


void LineTool::onMainButton(const plug::ControlState &state, const plug::Vec2d &mouse) {
    ASSERT(line_preview, "Line preview is nullptr!\n");

    switch (state.state) {
        case plug::State::Pressed:
            is_drawing = true;
            line_preview->getLayoutBox().setPosition(mouse);
            line_preview->getLayoutBox().setSize(mouse);
            break;
        case plug::State::Released:
            onConfirm();
        default:
            break;
    }
}


void LineTool::onMove(const plug::Vec2d &mouse) {
    ASSERT(line_preview, "Line preview is nullptr!\n");

    line_preview->getLayoutBox().setSize(mouse);
}


void LineTool::onConfirm() {
    if (is_drawing) {
        ASSERT(line_preview, "Line preview is nullptr!\n");

        plug::Vec2d p1 = line_preview->getLayoutBox().getPosition();
        plug::Vec2d p2 = line_preview->getLayoutBox().getSize();

        plug::VertexArray array(plug::Lines, 2);

        array[0] = plug::Vertex(p1, color_palette->getFGColor());
        array[1] = plug::Vertex(p2, color_palette->getFGColor());

        canvas->draw(array);

        is_drawing = false;
    }
}


plug::Widget *LineTool::getWidget() {
    return (is_drawing) ? line_preview : nullptr;
}


LineTool::~LineTool() {
    ASSERT(line_preview, "Line preview is nullptr!\n");

    delete line_preview;
}


// ============================================================================


EraserTool::EraserTool() : prev_position() {}


void EraserTool::onMainButton(const plug::ControlState &state, const plug::Vec2d &mouse) {
    switch (state.state) {
        case plug::State::Pressed: {
            is_drawing = true;
            prev_position = mouse;

            CircleShape circle(mouse, ERASER_RADIUS, color_palette->getBGColor());
            circle.draw(*canvas);

            break;
        }
        case plug::State::Released:
            is_drawing = false;
        default:
            break;
    }
}


void EraserTool::onMove(const plug::Vec2d &mouse) {
    if (is_drawing) {
        plug::Vec2d step(
            (mouse.x - prev_position.x) / ERASER_STEP, 
            (mouse.y - prev_position.y) / ERASER_STEP
        );

        CircleShape circle(mouse, ERASER_RADIUS, color_palette->getBGColor());

        for (int i = 0; i < ERASER_STEP; i++) {
            circle.draw(*canvas);
            circle.setPosition(circle.getPosition() + step);
        }

        prev_position = mouse;
    }
}


// ============================================================================


void ColorPicker::onMainButton(const plug::ControlState &state, const plug::Vec2d &mouse) {
    if (state.state == plug::State::Pressed)
        color_palette->setFGColor(canvas->getPixel(mouse.x, mouse.y));
}


// ============================================================================


void BucketTool::onMainButton(const plug::ControlState &state, const plug::Vec2d &mouse) {
    if (state.state == plug::State::Pressed) {
        plug::Color color = color_palette->getFGColor();
        plug::Color origin = canvas->getPixel(mouse.x, mouse.y);

        plug::Texture texture(canvas->getTexture());
        
        List<sf::Vector2u> dfs;
        dfs.push_back(sf::Vector2u(mouse.x, mouse.y));
        texture.setPixel(dfs.back().x, dfs.back().y, color);
        
        while (dfs.size()) {
            sf::Vector2u pixel = dfs.back();
            dfs.pop_back();

            if (pixel.x > 0 && isEqual(texture.getPixel(pixel.x - 1, pixel.y), origin)) {
                dfs.push_back(sf::Vector2u(pixel.x - 1, pixel.y));
                texture.setPixel(pixel.x - 1, pixel.y, color);
            }

            if (pixel.x < texture.width - 1 && isEqual(texture.getPixel(pixel.x + 1, pixel.y), origin)) {
                dfs.push_back(sf::Vector2u(pixel.x + 1, pixel.y));
                texture.setPixel(pixel.x + 1, pixel.y, color);
            }

            if (pixel.y > 0 && isEqual(texture.getPixel(pixel.x, pixel.y - 1), origin)) {
                dfs.push_back(sf::Vector2u(pixel.x, pixel.y - 1));
                texture.setPixel(pixel.x, pixel.y - 1, color);
            }

            if (pixel.y < texture.height - 1 && isEqual(texture.getPixel(pixel.x, pixel.y + 1), origin)) {
                dfs.push_back(sf::Vector2u(pixel.x, pixel.y + 1));
                texture.setPixel(pixel.x, pixel.y + 1, color);
            }
        }
        
        TextureShape(texture).draw(*canvas, plug::Vec2d(), canvas->getSize());
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
    

    virtual void draw(plug::TransformStack &stack, plug::RenderTarget &result) override {
        result.draw(getPolygonArray(stack.top(), tool.getPoints(), PREVIEW_COLOR, plug::LineStrip));
    }
};


PolygonTool::PolygonTool() : points(), polygon_preview(nullptr) {
    polygon_preview = new PolygonPreview(*this);
}


List<plug::Vec2d> &PolygonTool::getPoints() {
    return points;
}


void PolygonTool::onMainButton(const plug::ControlState &state, const plug::Vec2d &mouse) {
    if (state.state == plug::State::Pressed) {
        is_drawing = true;
        if (points.size() && (points[0] - mouse).length() < POLYGON_EPSILON)
            onConfirm();
        else {
            if (!points.size()) {
                points.push_back(mouse);
                points.push_back(mouse);
            }
            points.insert(points.size() - 1, mouse);
        }
    }
}


void PolygonTool::onMove(const plug::Vec2d &mouse) {
    if (is_drawing) {
        points[points.size() - 2] = mouse;
    }
}


void PolygonTool::onConfirm() {
    if (is_drawing) {
        canvas->draw(getPolygonArray(plug::Transform(), points, color_palette->getFGColor(), plug::LineStrip));
        is_drawing = false;
    }

    points.resize(0, plug::Vec2d());
}


void PolygonTool::onCancel() {
    is_drawing = false;
    points.resize(0, plug::Vec2d());
}


plug::Widget *PolygonTool::getWidget() {
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
        hex2Color(0),
        PREVIEW_COLOR,
        hex2Color(0),
        0
    );

    text_preview = new LineEdit(
        Widget::AUTO_ID,
        LazyLayoutBox(plug::Vec2d(), plug::Vec2d(SCREEN_W, TEXT_SIZE + TEXT_OFFSET * 2)),
        style,
        TEXT_MAX_LENGTH
    );
}


void TextTool::onMainButton(const plug::ControlState &state, const plug::Vec2d &mouse) {
    if (state.state == plug::State::Pressed) {
        is_drawing = true;
        text_preview->getLayoutBox().setPosition(mouse);
        text_preview->setKeyboardFocus(true);
    }
}


void TextTool::onConfirm() {
    TextShape text_shape(sf::Text("", text_font, TEXT_SIZE), text_preview->getLayoutBox().getSize());

    if (is_drawing) {
        text_shape.setColor(color_palette->getFGColor());
        text_shape.setText(text_preview->getString());

        text_shape.draw(
            *canvas,
            text_preview->getLayoutBox().getPosition() + plug::Vec2d(TEXT_OFFSET, TEXT_OFFSET),
            text_preview->getLayoutBox().getSize()
        );

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


plug::Widget *TextTool::getWidget() {
    return (is_drawing) ? text_preview : nullptr;
}


TextTool::~TextTool() {
    delete text_preview;
}
