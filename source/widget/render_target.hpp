/**
 * \file
 * \brief Contains my texture and render target inteface
*/


#ifndef _RENDER_TARGET_H_
#define _RENDER_TARGET_H_


#include "SFML/Graphics.hpp"
#include "standart/Math.h"
#include "standart/Graphics.h"
#include "standart/Color.h"


/**
 * \brief Loads texture from file
 * \param [out] texture_ptr     Points to allocated texture
 * \param [in]  filename        Path to image
 * \warning plug::Texture is allocated using new, do not forget to delete it
*/
bool loadTexture(plug::Texture **texture_ptr, const char *filename);


/// plug::Texture for drawing on
class RenderTexture : public plug::RenderTarget {
public:
    RenderTexture();

    RenderTexture(const RenderTexture&) = delete;
    
    RenderTexture &operator = (const RenderTexture&) = delete;

    void create(size_t width, size_t height);

    const plug::Texture &getTexture() const;

    virtual void draw(const plug::VertexArray& array) override;

    virtual void draw(const plug::VertexArray& array, const plug::Texture& texture) override;

    virtual void clear(plug::Color color) override;

    /**
     * \brief Returns texture size
    */
    plug::Vec2d getSize() const;

    const sf::Texture &getSFMLTexture() const;

    virtual ~RenderTexture() override;

    virtual void setActive(bool active) override {}

private:
    void setChanged(bool is_changed_) const;

    bool isChanged() const;

    sf::RenderTexture render_texture;
    plug::Texture *inner_texture;
    bool *const is_changed;
};


#endif
