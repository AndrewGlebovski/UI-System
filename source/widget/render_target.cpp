/**
 * \file
 * \brief Contains my texture and render target implementation
*/


#include <cstring>
#include "common/assert.hpp"
#include "widget/render_target.hpp"
#include "common/utils.hpp"


// ============================================================================


bool loadTexture(plug::Texture **texture_ptr, const char *filename) {
    if (!filename) return false;

    sf::Image image;
    if (!image.loadFromFile(filename)) return false;

    const uint8_t *src = image.getPixelsPtr();

    *texture_ptr = new plug::Texture(image.getSize().x, image.getSize().y);
    ASSERT(*texture_ptr, "Failed to allocate texture!\n");

    plug::Texture &texture = *(*texture_ptr);

    for (size_t i = 0; i < texture.width * texture.height; i++)
        texture.data[i] = plug::Color(src[i * 4], src[i * 4 + 1], src[i * 4 + 2], src[i * 4 + 3]);
    
    return true;
}


// ============================================================================


RenderTexture::RenderTexture() :
    render_texture(), inner_texture(nullptr), is_changed(false) {}


void RenderTexture::create(size_t width, size_t height) {
    ASSERT(render_texture.create(width, height), "Failed to create SFML texture!\n");
    
    inner_texture = new plug::Texture(width, height);
    ASSERT(inner_texture, "Failed to allocate texture!\n");

    setChanged(true);
}


const plug::Texture &RenderTexture::getTexture() const {
    if (isChanged()) {
        sf::Image image = render_texture.getTexture().copyToImage();
        
        const uint8_t *src = image.getPixelsPtr();

        for (size_t i = 0; i < inner_texture->width * inner_texture->height; i++)
            inner_texture->data[i] = plug::Color(src[i * 4], src[i * 4 + 1], src[i * 4 + 2], src[i * 4 + 3]);
        
        setChanged(false);
    }

    return *inner_texture;
}


void RenderTexture::draw(const plug::VertexArray& array) {
    sf::VertexArray vertices(
        sf::PrimitiveType(array.getPrimitive()),
        array.getSize()
    );

    for (size_t i = 0; i < array.getSize(); i++) {
        vertices[i] = sf::Vertex(
            getSfmlVector2f(array[i].position),
            getSfmlColor(array[i].color),
            getSfmlVector2f(array[i].tex_coords)
        );
    }

    render_texture.draw(vertices);

    render_texture.display();
    setChanged(true);
}


void RenderTexture::draw(const plug::VertexArray& array, const plug::Texture& texture) {
    static sf::VertexArray vertices;

    vertices.setPrimitiveType(sf::PrimitiveType(array.getPrimitive()));
    vertices.resize(array.getSize());

    for (size_t i = 0; i < array.getSize(); i++) {
        vertices[i] = sf::Vertex(
            getSfmlVector2f(array[i].position),
            getSfmlColor(array[i].color),
            getSfmlVector2f(array[i].tex_coords)
        );
    }
    
    static sf::Image image;
    image.create(texture.width, texture.height, reinterpret_cast<const uint8_t*>(texture.data));

    static sf::Texture tex;
    tex.loadFromImage(image);

    render_texture.draw(vertices, &tex);

    render_texture.display();
    setChanged(true);
}


void RenderTexture::clear(plug::Color color) {
    render_texture.clear(getSfmlColor(color));
    setChanged(true);
}


plug::Vec2d RenderTexture::getSize() const {
    return getPlugVector(render_texture.getSize());
}


const sf::Texture &RenderTexture::getSFMLTexture() const {
    return render_texture.getTexture();
}


RenderTexture::~RenderTexture() {
    if (inner_texture)
        delete inner_texture;
}


void RenderTexture::setChanged(bool is_changed_) const {
    is_changed = is_changed_;
}


bool RenderTexture::isChanged() const {
    return is_changed;
}
