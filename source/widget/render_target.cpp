/**
 * \file
 * \brief Contains my texture and render target implementation
*/


#include <cstring>
#include "common/assert.hpp"
#include "widget/render_target.hpp"


// ============================================================================


Texture::Texture(size_t width_, size_t height_, const Color* data_) :
    data(new Color[width_ * height_]), width(width_), height(height_)
{
    ASSERT(data, "Failed to allocate data!\n");
    ASSERT(memcpy(data, data_, width * height * 4), "Memcpy failed!\n");
}


Texture::Texture(size_t width_, size_t height_) :
    data(new Color[width_ * height_]), width(width_), height(height_) 
{
    ASSERT(data, "Failed to allocate buffer!\n");
}


Texture::Texture(const Texture &texture_) :
    Texture(texture_.width, texture_.height, texture_.data) {}


void Texture::setPixel(size_t x, size_t y, Color color) {
    ASSERT(x < width, "Index is out of range!\n");
    ASSERT(y < height, "index is out of range!\n");

    data[y * width + x] = color;
}


Color Texture::getPixel(size_t x, size_t y) const {
    ASSERT(x < width, "Index is out of range!\n");
    ASSERT(y < height, "index is out of range!\n");

    return data[y * width + x];
}


Texture::~Texture() {
    delete[] data;
}


// ============================================================================


void loadTexture(Texture **texture_ptr, const char *filename) {
    ASSERT(filename, "Filename is nullptr!\n");

    sf::Image image;
    ASSERT(image.loadFromFile(filename), "Failed to load image!\n");

    const uint8_t *src = image.getPixelsPtr();

    *texture_ptr = new Texture(image.getSize().x, image.getSize().y);
    ASSERT(*texture_ptr, "Failed to allocate texture!\n");

    Texture &texture = *(*texture_ptr);

    for (size_t i = 0; i < texture.width * texture.height; i++)
        texture.data[i] = Color(src[i * 4], src[i * 4 + 1], src[i * 4 + 2], src[i * 4 + 3]);
}


// ============================================================================


RenderTexture::RenderTexture() :
    render_texture(), inner_texture(nullptr), is_changed(new bool(false))
{
    ASSERT(is_changed, "Failed to allocate is_changed!\n");
}


void RenderTexture::create(size_t width, size_t height) {
    ASSERT(render_texture.create(width, height), "Failed to create SFML texture!\n");
    
    inner_texture = new Texture(width, height);
    ASSERT(inner_texture, "Failed to allocate texture!\n");

    setChanged(true);
}


const Texture &RenderTexture::getTexture() const {
    if (isChanged()) {
        sf::Image image = render_texture.getTexture().copyToImage();
        
        const uint8_t *src = image.getPixelsPtr();

        for (size_t i = 0; i < inner_texture->width * inner_texture->height; i++)
            inner_texture->data[i] = Color(src[i * 4], src[i * 4 + 1], src[i * 4 + 2], src[i * 4 + 3]);
        
        setChanged(false);
    }

    return *inner_texture;
}


void RenderTexture::draw(const VertexArray& array) {
    sf::VertexArray vertices(
        sf::PrimitiveType(array.getPrimitive()),
        array.getSize()
    );

    for (size_t i = 0; i < array.getSize(); i++)
        vertices[i] = sf::Vertex(array[i].position, array[i].color, array[i].tex_coords);
    
    render_texture.draw(vertices);

    render_texture.display();
    setChanged(true);
}


void RenderTexture::draw(const VertexArray& array, const Texture& texture) {
    static sf::VertexArray vertices;

    vertices.setPrimitiveType(sf::PrimitiveType(array.getPrimitive()));
    vertices.resize(array.getSize());

    for (size_t i = 0; i < array.getSize(); i++)
        vertices[i] = sf::Vertex(array[i].position, array[i].color, array[i].tex_coords);
    
    static sf::Image image;
    image.create(texture.width, texture.height, reinterpret_cast<const uint8_t*>(texture.data));

    static sf::Texture tex;
    tex.loadFromImage(image);

    render_texture.draw(vertices, &tex);

    render_texture.display();
    setChanged(true);
}


void RenderTexture::clear(Color color) {
    render_texture.clear(color);
    setChanged(true);
}


Vec2d RenderTexture::getSize() const {
    return render_texture.getSize();
}


const sf::Texture &RenderTexture::getSFMLTexture() const {
    return render_texture.getTexture();
}


RenderTexture::~RenderTexture() {
    if (inner_texture)
        delete inner_texture;
    
    ASSERT(is_changed, "Is changed is nullptr!\n");
    delete is_changed;
}


void RenderTexture::setChanged(bool is_changed_) const {
    *is_changed = is_changed_;
}


bool RenderTexture::isChanged() const {
    return *is_changed;
}
