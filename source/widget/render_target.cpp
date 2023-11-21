/**
 * \file
 * \brief Contains my texture and render target implementation
*/


#include "widget/render_target.hpp"


Texture::Texture(size_t width_, size_t height_, const Color* data_) :
    data(data_), width(width_), height(height_) {}


Texture::Texture(size_t width_, size_t height_) :
    data(new Color[width_ * height_]), width(width_), height(height_) {}


Texture::~Texture() {
    delete[] data;
}


RenderTexture::RenderTexture() : sf_texture() {}


void RenderTexture::create(size_t width, size_t height) {
    sf_texture.create(width, height);
}


const sf::Texture &RenderTexture::getTexture() {
    sf_texture.display();
    return sf_texture.getTexture();
}


void RenderTexture::draw(const VertexArray& array) {
    sf::VertexArray vertices(
        sf::PrimitiveType(array.getPrimitive()),
        array.getSize()
    );

    for (size_t i = 0; i < array.getSize(); i++)
        vertices[i] = sf::Vertex(array[i].position, array[i].color, array[i].tex_coords);
    
    sf_texture.draw(vertices);
}


void RenderTexture::draw(const VertexArray& array, const Texture& texture) {
    sf::VertexArray vertices(
        sf::PrimitiveType(array.getPrimitive()),
        array.getSize()
    );

    for (size_t i = 0; i < array.getSize(); i++)
        vertices[i] = sf::Vertex(array[i].position, array[i].color, array[i].tex_coords);
    
    sf::Image image;
    image.create(texture.width, texture.height, reinterpret_cast<const uint8_t*>(texture.data));

    sf::Texture tex;
    tex.loadFromImage(image);

    sf_texture.draw(vertices, &tex);
}


void RenderTexture::clear(Color color) {
    sf_texture.clear(color);
}


unsigned RenderTexture::getNativeHandle() const {
    return sf_texture.getTexture().getNativeHandle();
}
