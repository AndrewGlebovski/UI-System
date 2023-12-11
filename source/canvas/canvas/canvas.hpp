/**
 * \file
 * \brief Contains canvas interface and implementation
*/


#ifndef _CANVAS_H_
#define _CANVAS_H_


#include <widget/render_target.hpp>
#include <canvas/canvas/selection_mask.hpp>
#include "standart/Canvas.h"


class SFMLCanvas  : public plug::Canvas {
public:
    SFMLCanvas();

    SFMLCanvas(const SFMLCanvas&) = delete;
    
    SFMLCanvas &operator = (const SFMLCanvas&) = delete;

    virtual void draw(const plug::VertexArray& vertex_array) override;

    virtual void draw(const plug::VertexArray& vertex_array, const plug::Texture& texture) override;

    virtual plug::Vec2d getSize() const override;

    virtual void setSize(const plug::Vec2d& size) override;

    virtual plug::SelectionMask& getSelectionMask() override;

    virtual plug::Color getPixel(size_t x, size_t y) const override;

    virtual void setPixel(size_t x, size_t y, const plug::Color& color) override;

    virtual const plug::Texture &getTexture() const override;

    virtual ~SFMLCanvas() override;

private:
    RenderTexture render_texture;           ///< Texture to draw on
    plug::SelectionMask *selection_mask;    ///< Canvas selection mask
    plug::Texture *buffer_texture;          ///< Texture for boosting getPixel
};


#endif
