/**
 * \file
 * \brief Contains implementation of basic draw shapes
*/


#include <cmath>
#include <cstring>
#include "common/assert.hpp"
#include "widget/shape.hpp"


// ============================================================================


const size_t CIRCLE_VERTICES = 30;      ///< Amount of vertices in circle shape


// ============================================================================


RectShape::RectShape(const Vec2d position_, const Vec2d size_, Color color_) :
    position(position_), size(size_), color(color_), border_thickness(0), border_color() {}


void RectShape::setBorder(double border_thickness_, Color border_color_) {
    border_thickness = border_thickness_;
    border_color = border_color_;
}


const Vec2d &RectShape::getPosition() const { return position; }


void RectShape::setPosition(const Vec2d &position_) { position = position_; }


const Vec2d &RectShape::getSize() const { return size; }


void RectShape::setSize(const Vec2d &size_) { size = size_; }


Color RectShape::getColor() const { return color; }


void RectShape::setColor(Color color_) { color = color_; }


void RectShape::draw(RenderTarget &target) const {
    static VertexArray array(TriangleFan, 4);

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


void RectShape::draw(Canvas &canvas) const {
    static VertexArray array(TriangleFan, 4);

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


void RectShape::setCenter(VertexArray &array) const {
    array[0] = Vertex(position + Vec2d(border_thickness, border_thickness), color);
    array[1] = Vertex(Vec2d(position.x + border_thickness, position.y + size.y - border_thickness), color);
    array[2] = Vertex(position + size - Vec2d(border_thickness, border_thickness), color);
    array[3] = Vertex(Vec2d(position.x + size.x - border_thickness, position.y + border_thickness), color);
}


void RectShape::setTopBorder(VertexArray &array) const {
    array[0] = Vertex(position, border_color);
    array[1] = Vertex(Vec2d(position.x, position.y + border_thickness), border_color);
    array[2] = Vertex(Vec2d(position.x + size.x, position.y + border_thickness), border_color);
    array[3] = Vertex(Vec2d(position.x + size.x, position.y), border_color);
}


void RectShape::setBottomBorder(VertexArray &array) const {
    array[0].position = position + Vec2d(0, size.y - border_thickness);
    array[1].position = Vec2d(position.x, position.y + border_thickness) + Vec2d(0, size.y - border_thickness);
    array[2].position = Vec2d(position.x + size.x, position.y + border_thickness) + Vec2d(0, size.y - border_thickness);
    array[3].position = Vec2d(position.x + size.x, position.y) + Vec2d(0, size.y - border_thickness);
}


void RectShape::setLeftBorder(VertexArray &array) const {
    array[0].position = position;
    array[1].position = Vec2d(position.x, position.y + size.y);
    array[2].position = Vec2d(position.x + border_thickness, position.y + size.y);
    array[3].position = Vec2d(position.x + border_thickness, position.y);
}


void RectShape::setRightBorder(VertexArray &array) const {
    array[0].position = position + Vec2d(size.x - border_thickness, 0);
    array[1].position = Vec2d(position.x, position.y + size.y) + Vec2d(size.x - border_thickness, 0);
    array[2].position = Vec2d(position.x + border_thickness, position.y + size.y) + Vec2d(size.x - border_thickness, 0);
    array[3].position = Vec2d(position.x + border_thickness, position.y) + Vec2d(size.x - border_thickness, 0);
}


// ============================================================================


CircleShape::CircleShape(const Vec2d position_, double radius_, Color color_) :
    position(position_), radius(radius_), color(color_) {}


const Vec2d &CircleShape::getPosition() const { return position; }


void CircleShape::setPosition(const Vec2d &position_) { position = position_; }


double CircleShape::getRadius() const { return radius; }


void CircleShape::setRadius(double radius_) { radius = radius_; }


Color CircleShape::getColor() const { return color; }


void CircleShape::setColor(Color color_) { color = color_; }


void CircleShape::draw(RenderTarget &target) const {
    // ONE MORE VERTEX FOR CENTER AND ONE FOR COMPLETING CIRCLE
    static VertexArray array(TriangleFan, CIRCLE_VERTICES + 2);

    setVertexArray(array);

    target.draw(array);
}


void CircleShape::draw(Canvas &canvas) const {
    // ONE MORE VERTEX FOR CENTER AND ONE FOR COMPLETING CIRCLE
    static VertexArray array(TriangleFan, CIRCLE_VERTICES + 2);

    setVertexArray(array);

    canvas.draw(array);
}


void CircleShape::setVertexArray(VertexArray &array) const {
    double phi = 0;

    array[0] = Vertex(position, color);

    for (size_t i = 1; i <= CIRCLE_VERTICES + 1; i++) {
        array[i] = Vertex(position + Vec2d(cos(phi), sin(phi)) * radius, color);
        phi += 6.28319 / CIRCLE_VERTICES;
    }
}


// ============================================================================


TextureShape::TextureShape(const Texture &texture_) :
    array(TriangleFan, 4), texture(texture_)
{
    array[0].tex_coords = Vec2d();
    array[1].tex_coords = Vec2d(0, texture.height);
    array[2].tex_coords = Vec2d(texture.width, texture.height);
    array[3].tex_coords = Vec2d(texture.width, 0);
}


void TextureShape::draw(RenderTarget &target, const Vec2d &position, const Vec2d &size) {
    array[0].position = position;
    array[1].position = Vec2d(position.x, position.y + size.y);
    array[2].position = position + size;
    array[3].position = Vec2d(position.x + size.x, position.y);

    target.draw(array, texture);
}


void TextureShape::draw(Canvas &canvas, const Vec2d &position, const Vec2d &size) {
    array[0].position = position;
    array[1].position = Vec2d(position.x, position.y + size.y);
    array[2].position = position + size;
    array[3].position = Vec2d(position.x + size.x, position.y);

    canvas.draw(array, texture);
}


// ============================================================================


TextShape::TextShape(const sf::Text &text_, const Vec2d &texture_size_) :
    sf_text(text_),
    texture(texture_size_.x, texture_size_.y),
    shape(texture),
    render_texture()
{
    ASSERT(texture.width && texture.height, "Texture has zero size!\n");
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
    ASSERT(texture.width && texture.height, "Texture has zero size!\n");
    ASSERT(render_texture.create(texture.width, texture.height), "Failed to create render texture!\n");

    sf_text.setPosition(-text_.getLocalBounds().left, -text_.getLocalBounds().top);

    render();
}


void TextShape::setTextOffset(const Vec2d &offset) {
    if (offset == sf_text.getPosition()) return;

    sf_text.setPosition(offset);
    render();
}


Vec2d TextShape::getTextOffset() const {
    return sf_text.getPosition();
}


const sf::Text &TextShape::getText() const {
    return sf_text;
}


Vec2d TextShape::getTextureSize() const {
    return Vec2d(texture.width, texture.height);
}


void TextShape::setColor(Color color) {
    if (sf_text.getFillColor() == color) return;

    sf_text.setFillColor(color);
    render();
}


void TextShape::setText(const char *str) {
    // Some fast compare requiered
    
    sf_text.setString(str);
    render();
}


void TextShape::draw(RenderTarget &target, const Vec2d &position, const Vec2d &size) {
    shape.draw(target, position, size);
}


void TextShape::draw(Canvas &canvas, const Vec2d &position, const Vec2d &size) {
    shape.draw(canvas, position, size);
}


void TextShape::render() {
    render_texture.draw(sf_text);
    render_texture.display();

    sf::Image image = render_texture.getTexture().copyToImage();

    const uint8_t *source = image.getPixelsPtr();

    for (size_t i = 0; i < texture.width * texture.height; i++)
        texture.data[i] = Color(source[i * 4], source[i * 4 + 1], source[i * 4 + 2], source[i * 4 + 3]);

    render_texture.clear(Color(0));
}
