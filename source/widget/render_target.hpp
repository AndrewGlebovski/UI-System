/**
 * \file
 * \brief Contains my texture and render target inteface
*/


#ifndef _RENDER_TARGET_H_
#define _RENDER_TARGET_H_


#include "SFML/Graphics.hpp"
#include "widget/vertex.hpp"


struct Texture {
    Texture(size_t width, size_t height, const Color* data);

    Texture(size_t width, size_t height);

    Texture(const Texture &texture_) = delete;
    Texture &operator = (const Texture &texture_) = delete;

    ~Texture();

    const Color *data;
    const size_t width;
    const size_t height;
};


class RenderTarget {
public:
    virtual void draw(const VertexArray& array) = 0;

    virtual void draw(const VertexArray& array, const Texture& texture) = 0;

    virtual void clear(Color color) = 0;

    virtual unsigned getNativeHandle() const = 0;

    virtual ~RenderTarget() = default;
};


class RenderTexture : public RenderTarget {
public:
    RenderTexture();

    void create(size_t width, size_t height);

    const sf::Texture &getTexture();

    virtual void draw(const VertexArray& array) override;

    virtual void draw(const VertexArray& array, const Texture& texture) override;

    virtual void clear(Color color) override;

    virtual unsigned getNativeHandle() const override;

private:
    sf::RenderTexture sf_texture;
};


#endif
