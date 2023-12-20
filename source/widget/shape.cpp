/**
 * \file
 * \brief Contains implementation of basic draw shapes
*/


#include <cmath>
#include <cstring>
#include "common/assert.hpp"
#include "widget/shape.hpp"
#include "common/utils.hpp"


// ============================================================================


const size_t CIRCLE_VERTICES = 30;      ///< Amount of vertices in circle shape


// ============================================================================


RectShape::RectShape(const plug::Vec2d position_, const plug::Vec2d size_, plug::Color color_) :
    position(position_), size(size_), color(color_), border_thickness(0), border_color() {}


void RectShape::setBorder(double border_thickness_, plug::Color border_color_) {
    border_thickness = border_thickness_;
    border_color = border_color_;
}


const plug::Vec2d &RectShape::getPosition() const { return position; }


void RectShape::setPosition(const plug::Vec2d &position_) { position = position_; }


const plug::Vec2d &RectShape::getSize() const { return size; }


void RectShape::setSize(const plug::Vec2d &size_) { size = size_; }


plug::Color RectShape::getColor() const { return color; }


void RectShape::setColor(plug::Color color_) { color = color_; }


void RectShape::draw(plug::RenderTarget &target) const {
    static plug::VertexArray array(plug::TriangleFan, 4);

    setCenter(array);
    target.draw(array);

    target.draw(array);
    
    if (isEqual(border_thickness, 0)) return;

    setTopBorder(array);
    target.draw(array);

    setBottomBorder(array);
    target.draw(array);

    setLeftBorder(array);
    target.draw(array);

    setRightBorder(array);
    target.draw(array);
}


void RectShape::draw(plug::Canvas &canvas) const {
    static plug::VertexArray array(plug::TriangleFan, 4);

    setCenter(array);
    canvas.draw(array);
    
    if (isEqual(border_thickness, 0)) return;

    // ORDER OF FUNCTION CALLS IS IMPORTANT

    setTopBorder(array);
    canvas.draw(array);

    setBottomBorder(array);
    canvas.draw(array);

    setLeftBorder(array);
    canvas.draw(array);

    setRightBorder(array);
    canvas.draw(array);
}


void RectShape::setCenter(plug::VertexArray &array) const {
    array[0] = plug::Vertex(position + plug::Vec2d(border_thickness, border_thickness), color);
    array[1] = plug::Vertex(plug::Vec2d(position.x + border_thickness, position.y + size.y - border_thickness), color);
    array[2] = plug::Vertex(position + size - plug::Vec2d(border_thickness, border_thickness), color);
    array[3] = plug::Vertex(plug::Vec2d(position.x + size.x - border_thickness, position.y + border_thickness), color);
}


void RectShape::setTopBorder(plug::VertexArray &array) const {
    array[0] = plug::Vertex(position, border_color);
    array[1] = plug::Vertex(plug::Vec2d(position.x, position.y + border_thickness), border_color);
    array[2] = plug::Vertex(plug::Vec2d(position.x + size.x, position.y + border_thickness), border_color);
    array[3] = plug::Vertex(plug::Vec2d(position.x + size.x, position.y), border_color);
}


void RectShape::setBottomBorder(plug::VertexArray &array) const {
    array[0].position = position + plug::Vec2d(0, size.y - border_thickness);
    array[1].position = plug::Vec2d(position.x, position.y + border_thickness) + plug::Vec2d(0, size.y - border_thickness);
    array[2].position = plug::Vec2d(position.x + size.x, position.y + border_thickness) + plug::Vec2d(0, size.y - border_thickness);
    array[3].position = plug::Vec2d(position.x + size.x, position.y) + plug::Vec2d(0, size.y - border_thickness);
}


void RectShape::setLeftBorder(plug::VertexArray &array) const {
    array[0].position = position;
    array[1].position = plug::Vec2d(position.x, position.y + size.y);
    array[2].position = plug::Vec2d(position.x + border_thickness, position.y + size.y);
    array[3].position = plug::Vec2d(position.x + border_thickness, position.y);
}


void RectShape::setRightBorder(plug::VertexArray &array) const {
    array[0].position = position + plug::Vec2d(size.x - border_thickness, 0);
    array[1].position = plug::Vec2d(position.x, position.y + size.y) + plug::Vec2d(size.x - border_thickness, 0);
    array[2].position = plug::Vec2d(position.x + border_thickness, position.y + size.y) + plug::Vec2d(size.x - border_thickness, 0);
    array[3].position = plug::Vec2d(position.x + border_thickness, position.y) + plug::Vec2d(size.x - border_thickness, 0);
}


// ============================================================================


CircleShape::CircleShape(const plug::Vec2d position_, double radius_, plug::Color color_) :
    position(position_), radius(radius_), color(color_) {}


const plug::Vec2d &CircleShape::getPosition() const { return position; }


void CircleShape::setPosition(const plug::Vec2d &position_) { position = position_; }


double CircleShape::getRadius() const { return radius; }


void CircleShape::setRadius(double radius_) { radius = radius_; }


plug::Color CircleShape::getColor() const { return color; }


void CircleShape::setColor(plug::Color color_) { color = color_; }


void CircleShape::draw(plug::RenderTarget &target) const {
    // ONE MORE VERTEX FOR CENTER AND ONE FOR COMPLETING CIRCLE
    static plug::VertexArray array(plug::TriangleFan, CIRCLE_VERTICES + 2);

    setVertexArray(array);

    target.draw(array);
}


void CircleShape::draw(plug::Canvas &canvas) const {
    // ONE MORE VERTEX FOR CENTER AND ONE FOR COMPLETING CIRCLE
    static plug::VertexArray array(plug::TriangleFan, CIRCLE_VERTICES + 2);

    setVertexArray(array);

    canvas.draw(array);
}


void CircleShape::setVertexArray(plug::VertexArray &array) const {
    double phi = 0;

    array[0] = plug::Vertex(position, color);

    for (size_t i = 1; i <= CIRCLE_VERTICES + 1; i++) {
        array[i] = plug::Vertex(position + plug::Vec2d(cos(phi), sin(phi)) * radius, color);
        phi += 6.28319 / CIRCLE_VERTICES;
    }
}


// ============================================================================


TextureShape::TextureShape(const plug::Texture &texture_) :
    array(plug::TriangleFan, 4), texture(texture_)
{
    array[0].tex_coords = plug::Vec2d();
    array[1].tex_coords = plug::Vec2d(0, texture.height);
    array[2].tex_coords = plug::Vec2d(texture.width, texture.height);
    array[3].tex_coords = plug::Vec2d(texture.width, 0);
}


void TextureShape::draw(plug::RenderTarget &target, const plug::Vec2d &position, const plug::Vec2d &size) {
    array[0].position = position;
    array[1].position = plug::Vec2d(position.x, position.y + size.y);
    array[2].position = position + size;
    array[3].position = plug::Vec2d(position.x + size.x, position.y);

    target.draw(array, texture);
}


void TextureShape::draw(plug::Canvas &canvas, const plug::Vec2d &position, const plug::Vec2d &size) {
    array[0].position = position;
    array[1].position = plug::Vec2d(position.x, position.y + size.y);
    array[2].position = position + size;
    array[3].position = plug::Vec2d(position.x + size.x, position.y);

    canvas.draw(array, texture);
}


// ============================================================================


TextShape::TextShape(const sf::Text &text_, const plug::Vec2d &texture_size_) :
    sf_text(text_),
    texture(texture_size_.x, texture_size_.y),
    shape(texture),
    render_texture()
{
    ASSERT(texture.width && texture.height, "plug::Texture has zero size!\n");
    ASSERT(render_texture.create(texture.width, texture.height), "Failed to create render texture!\n");

    render();
}


TextShape::TextShape(const sf::Text &text_) :
    sf_text(text_),
    texture(
        sf_text.getGlobalBounds().width - sf_text.getGlobalBounds().left * 2,
        sf_text.getGlobalBounds().height - sf_text.getGlobalBounds().top * 2
    ),
    shape(texture),
    render_texture()
{    
    ASSERT(texture.width && texture.height, "plug::Texture has zero size!\n");
    ASSERT(render_texture.create(texture.width, texture.height), "Failed to create render texture!\n");

    sf_text.setPosition(-text_.getLocalBounds().left, -text_.getLocalBounds().top);

    render();
}


void TextShape::setTextOffset(const plug::Vec2d &offset) {
    if (getSfmlVector2f(offset) == sf_text.getPosition()) return;

    sf_text.setPosition(getSfmlVector2f(offset));
    render();
}


plug::Vec2d TextShape::getTextOffset() const {
    return getPlugVector(sf_text.getPosition());
}


const sf::Text &TextShape::getText() const {
    return sf_text;
}


plug::Vec2d TextShape::getTextureSize() const {
    return plug::Vec2d(texture.width, texture.height);
}


void TextShape::setColor(plug::Color color) {
    if (sf_text.getFillColor() == getSfmlColor(color)) return;

    sf_text.setFillColor(getSfmlColor(color));
    render();
}


void TextShape::setText(const char *str) {
    // Some fast compare requiered
    
    sf_text.setString(str);
    render();
}


void TextShape::draw(plug::RenderTarget &target, const plug::Vec2d &position, const plug::Vec2d &size) {
    shape.draw(target, position, size);
}


void TextShape::draw(plug::Canvas &canvas, const plug::Vec2d &position, const plug::Vec2d &size) {
    shape.draw(canvas, position, size);
}


const plug::Texture TextShape::getTexture() const {
    return texture;
}


void TextShape::render() {
    render_texture.draw(sf_text);
    render_texture.display();

    sf::Image image = render_texture.getTexture().copyToImage();

    const uint8_t *source = image.getPixelsPtr();

    for (size_t i = 0; i < texture.width * texture.height; i++)
        texture.data[i] = plug::Color(source[i * 4], source[i * 4 + 1], source[i * 4 + 2], source[i * 4 + 3]);

    render_texture.clear(sf::Color(0));
}
