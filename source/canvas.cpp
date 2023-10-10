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
#include "scrollbar.hpp"
#include "canvas.hpp"


PencilTool::PencilTool(const sf::Color &color_) : mouse_prev(), color(color_) {}


void PencilTool::setColor(const sf::Color &new_color) { color = new_color; }


void PencilTool::onMouseMove(int mouse_x, int mouse_y, sf::RenderTexture &result) {
    if (is_drawing) {
        Vector2D mouse_cur(mouse_x, mouse_y);

        sf::Vertex line[] = {
            sf::Vertex(mouse_prev, color),
            sf::Vertex(mouse_cur, color),
        };

        mouse_prev = mouse_cur;

        result.draw(line, 2, sf::Lines);
    }
}


void PencilTool::onMouseButtonDown(int mouse_x, int mouse_y, sf::RenderTexture &result) {
    is_drawing = true;
    mouse_prev = Vector2D(mouse_x, mouse_y);

    sf::Vertex point[] = {
        sf::Vertex(mouse_prev, color),
    };

    result.draw(point, 1, sf::Lines);
}


void PencilTool::onMouseButtonUp(int mouse_x, int mouse_y, sf::RenderTexture &result) {
    is_drawing = false;
}


EraserTool::EraserTool(vec_t radius_) : mouse_prev(), radius(radius_) {}


void EraserTool::setRadius(vec_t new_radius) { radius = new_radius; }


void EraserTool::onMouseMove(int mouse_x, int mouse_y, sf::RenderTexture &result) {
    if (is_drawing) {
        mouse_prev = Vector2D(mouse_x, mouse_y);

        sf::CircleShape circle(radius);
        circle.setPosition(mouse_prev - Vector2D(1, 1) * radius);

        result.draw(circle);
    }
}


void EraserTool::onMouseButtonDown(int mouse_x, int mouse_y, sf::RenderTexture &result) {
    is_drawing = true;
    mouse_prev = Vector2D(mouse_x, mouse_y);

    sf::CircleShape circle(radius);
    circle.setPosition(mouse_prev - Vector2D(1, 1) * radius);

    result.draw(circle);
}


void EraserTool::onMouseButtonUp(int mouse_x, int mouse_y, sf::RenderTexture &result) {
    is_drawing = false;
}


void Canvas::clear_canvas() {
    texture.clear(sf::Color::White);
}


Canvas::Canvas(
    const Vector2D &position_, const Vector2D &size_, int z_index_, BaseUI *parent_,
    const char *image_path
) :
    BaseUI(position_, size_, z_index_, parent_),
    texture(), texture_offset(Vector2D(0, 0)),
    tools(), current_tool(0)
{
    tools.push_back(new PencilTool(sf::Color::Red));
    tools.push_back(new EraserTool(25));

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


void Canvas::draw(sf::RenderTexture &result, List<Vector2D> &transforms) {
    TransformApplier add_transform(transforms, position);

    texture.display();

    sf::Sprite tool_sprite(texture.getTexture());
    sf::Vector2i offset_(texture_offset.x, texture_offset.y);
    sf::Vector2i size_(size.x, size.y);
    tool_sprite.setTextureRect(sf::IntRect(offset_, size_));
    tool_sprite.setPosition(transforms[0]);

    result.draw(tool_sprite);
}


int Canvas::onMouseMove(int mouse_x, int mouse_y, List<Vector2D> &transforms) {
    TransformApplier add_transform(transforms, position);

    Vector2D mouse_cur = Vector2D(mouse_x, mouse_y);

    if (isInsideRect(transforms[0], size, mouse_cur)) {
        tools[current_tool]->onMouseMove(
            mouse_cur.x - transforms[0].x + texture_offset.x,
            mouse_cur.y - transforms[0].y + texture_offset.y,
            texture
        );
        return HANDLED;
    }
    else
        tools[current_tool]->setDrawing(false);

    return UNHANDLED;
}


int Canvas::onMouseButtonDown(int mouse_x, int mouse_y, int button_id, List<Vector2D> &transforms) {
    if (button_id != MouseLeft) return UNHANDLED;

    TransformApplier add_transform(transforms, position);

    Vector2D mouse_cur = Vector2D(mouse_x, mouse_y);

    if (isInsideRect(transforms[0], size, mouse_cur)) {
        tools[current_tool]->onMouseButtonDown(
            mouse_cur.x - transforms[0].x + texture_offset.x,
            mouse_cur.y - transforms[0].y + texture_offset.y,
            texture
        );
        return HANDLED;
    }

    return UNHANDLED;
}


int Canvas::onMouseButtonUp(int mouse_x, int mouse_y, int button_id, List<Vector2D> &transforms) {
    if (button_id != MouseLeft) return UNHANDLED;

    TransformApplier add_transform(transforms, position);

    Vector2D mouse_cur = Vector2D(mouse_x, mouse_y);

    if (isInsideRect(transforms[0], size, mouse_cur)) {
        tools[current_tool]->onMouseButtonUp(
            mouse_cur.x - transforms[0].x + texture_offset.x,
            mouse_cur.y - transforms[0].y + texture_offset.y,
            texture
        );
        return HANDLED;
    }

    return UNHANDLED;
}


int Canvas::onKeyDown(int key_id) {
    switch (key_id) {
        case Num1: current_tool = 0; return HANDLED;
        case Num2: current_tool = 1; return HANDLED;
        default: return UNHANDLED;
    }
}


Canvas::~Canvas() {
    for (size_t i = 0; i < tools.getSize(); i++) {
        ASSERT(tools[i], "Pointer to tool is nullptr!\n");
        delete tools[i];
    }
}


#pragma GCC diagnostic pop
