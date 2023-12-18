/**
 * \file
 * \brief Contains canvas implementation
*/


#include <cstring>
#include "common/assert.hpp"
#include "canvas/canvas/canvas.hpp"
#include "canvas/palettes/palette_manager.hpp"


// ============================================================================


SFMLCanvas::SFMLCanvas() :
    render_texture(), selection_mask(nullptr), buffer_texture(nullptr) {}


void SFMLCanvas::draw(const plug::VertexArray& vertex_array) {
    render_texture.draw(vertex_array);
}


void SFMLCanvas::draw(const plug::VertexArray& vertex_array, const plug::Texture& texture) {
    render_texture.draw(vertex_array, texture);
}


plug::Vec2d SFMLCanvas::getSize() const { return render_texture.getSize(); }


void SFMLCanvas::setSize(const plug::Vec2d& size) {
    if (selection_mask) delete selection_mask;
    if (buffer_texture) delete buffer_texture;

    render_texture.create(size.x, size.y);

    selection_mask = new SelectionMask(size.x, size.y);
    ASSERT(selection_mask, "Failed to allocate selection mask!\n");

    selection_mask->fill(true);

    buffer_texture = new plug::Texture(size.x, size.y);
    ASSERT(buffer_texture, "Failed to allocate buffer texture!\n");

    for (size_t i = 0; i < buffer_texture->width * buffer_texture->height; i++)
        buffer_texture->data[i] = COLOR_PALETTE.getBGColor();
    
    TextureShape(*buffer_texture).draw(render_texture, plug::Vec2d(), size);
}


plug::SelectionMask& SFMLCanvas::getSelectionMask() {
    ASSERT(selection_mask, "Init canvas first!\n");
    return *selection_mask;
}


plug::Color SFMLCanvas::getPixel(size_t x, size_t y) const {
    return buffer_texture->getPixel(x, y);
}


void SFMLCanvas::setPixel(size_t x, size_t y, const plug::Color& color) {
    plug::VertexArray array(plug::Points, 1);
    array[0] = plug::Vertex(plug::Vec2d(x, y), color);
    render_texture.draw(array);
}


const plug::Texture &SFMLCanvas::getTexture() const {
    const plug::Texture &texture = render_texture.getTexture();

    memcpy(buffer_texture->data, texture.data, texture.width * texture.height * 4);

    return texture;
}


SFMLCanvas::~SFMLCanvas() {
    if (selection_mask)
        delete selection_mask;
    
    if (buffer_texture)
        delete buffer_texture;
}
