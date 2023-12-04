/**
 * \file
 * \brief Contains my texture and render target inteface
*/


#ifndef _RENDER_TARGET_H_
#define _RENDER_TARGET_H_


#include "SFML/Graphics.hpp"
#include "widget/vertex.hpp"


// Stores texture as width, height and color buffer
struct Texture {
    Texture(size_t width_, size_t height_, const Color* data_);

    Texture(size_t width_, size_t height_);

    Texture(const Texture &texture_);

    Texture &operator = (const Texture &texture_) = delete;

    void setPixel(size_t x, size_t y, Color color);

    Color getPixel(size_t x, size_t y) const;

    ~Texture();

    Color *const data;      ///< Color buffer
    const size_t width;     ///< Texture width in pixels
    const size_t height;    ///< Texture height in pixels
};


/**
 * \brief Loads texture from file
 * \param [out] texture_ptr     Points to allocated texture
 * \param [in]  filename        Path to image
 * \warning Texture is allocated using new, do not forget to delete it
*/
void loadTexture(Texture **texture_ptr, const char *filename);


/// Base class for drawing
class RenderTarget {
public:
    virtual void draw(const VertexArray& array) = 0;

    virtual void draw(const VertexArray& array, const Texture& texture) = 0;

    virtual void clear(Color color) = 0;

    virtual ~RenderTarget() = default;
};


/// Texture for drawing on
class RenderTexture : public RenderTarget {
public:
    RenderTexture();

    RenderTexture(const RenderTexture&) = delete;
    
    RenderTexture &operator = (const RenderTexture&) = delete;

    void create(size_t width, size_t height);

    const Texture &getTexture() const;

    virtual void draw(const VertexArray& array) override;

    virtual void draw(const VertexArray& array, const Texture& texture) override;

    virtual void clear(Color color) override;

    /**
     * \brief Returns texture size
    */
    Vec2d getSize() const;

    const sf::Texture &getSFMLTexture() const;

    virtual ~RenderTexture() override;

private:
    void setChanged(bool is_changed_) const;

    bool isChanged() const;

    sf::RenderTexture render_texture;
    Texture *inner_texture;
    bool *const is_changed;
};


#endif
