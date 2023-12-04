/**
 * \file
 * \brief Contains canvas interface and implementation
*/


#ifndef _CANVAS_H_
#define _CANVAS_H_


#include <widget/render_target.hpp>
#include <canvas/canvas/selection_mask.hpp>


class Canvas {
public:
    virtual ~Canvas() = default;

    /**
     * \brief Draw on canvas using vertex array mapping
     */
    virtual void draw(const VertexArray& vertex_array) = 0;

    /**
     * \brief Draw texture on canvas using vertex array mapping
    */
    virtual void draw(const VertexArray& vertex_array, const Texture& texture) = 0;

    /**
     * \brief Return size of canvas
    */
    virtual Vec2d getSize(void) const = 0;

    /**
     * \brief Set size of canvas
    */
    virtual void setSize(const Vec2d& size) = 0;

    /**
     * \brief Get selection mask of canvas
    */
    virtual SelectionMask& getSelectionMask(void) = 0;

    /**
     * \brief Get color of pixel on canvas by coordinates
    */
    virtual Color getPixel(size_t x, size_t y) const = 0;

    /**
     * \brief Set color of pixel on canvas by coordinates
    */
    virtual void setPixel(size_t x, size_t y, const Color& color) = 0;

    /**
     * \brief Returns reference of canvas texture
    */
    virtual const Texture &getTexture() const = 0;
};


class SFMLCanvas  : public Canvas {
public:
    SFMLCanvas();

    SFMLCanvas(const SFMLCanvas&) = delete;
    
    SFMLCanvas &operator = (const SFMLCanvas&) = delete;

    virtual void draw(const VertexArray& vertex_array) override;

    virtual void draw(const VertexArray& vertex_array, const Texture& texture) override;

    virtual Vec2d getSize() const override;

    virtual void setSize(const Vec2d& size) override;

    virtual SelectionMask& getSelectionMask() override;

    virtual Color getPixel(size_t x, size_t y) const override;

    virtual void setPixel(size_t x, size_t y, const Color& color) override;

    virtual const Texture &getTexture() const override;

    virtual ~SFMLCanvas() override;
private:
    RenderTexture render_texture;
    SelectionMask *selection_mask;
};


#endif
