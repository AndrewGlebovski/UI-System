/**
 * \file
 * \brief Contains canvas implementation
*/


#include "common/assert.hpp"
#include "canvas/canvas/canvas.hpp"


// ============================================================================


SFMLCanvas::SFMLCanvas() : render_texture(), selection_mask(nullptr) {}


void SFMLCanvas::draw(const VertexArray& vertex_array) {
    render_texture.draw(vertex_array);
}


void SFMLCanvas::draw(const VertexArray& vertex_array, const Texture& texture) {
    render_texture.draw(vertex_array, texture);
}


Vec2d SFMLCanvas::getSize() const { return render_texture.getSize(); }


void SFMLCanvas::setSize(const Vec2d& size) {
    render_texture.create(size.x, size.y);

    selection_mask = new SelectionMask(size.x, size.y);
    ASSERT(selection_mask, "Failed to allocate selection mask!\n");

    selection_mask->fill(true);
}


SelectionMask& SFMLCanvas::getSelectionMask() {
    ASSERT(selection_mask, "Init canvas first!\n");
    return *selection_mask;
}


Color SFMLCanvas::getPixel(size_t x, size_t y) const {
    return render_texture.getTexture().getPixel(x, y);
}


void SFMLCanvas::setPixel(size_t x, size_t y, const Color& color) {
    VertexArray array(Points, 1);
    array[0] = Vertex(Vec2d(x, y), color);
    render_texture.draw(array);
}


const Texture &SFMLCanvas::getTexture() const {
    return render_texture.getTexture();
}


SFMLCanvas::~SFMLCanvas() {
    if (selection_mask)
        delete selection_mask;
}
