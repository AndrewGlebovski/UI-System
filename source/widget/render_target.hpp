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
    /**
     * \brief Creates invalid texture
     * \warning Call create() first
    */
    RenderTexture();

    RenderTexture(const RenderTexture&) = delete;
    
    RenderTexture &operator = (const RenderTexture&) = delete;

    /**
     * \brief Creates texture to draw on
     * \warning Call this method before doing anything
    */
    void create(size_t width, size_t height);

    /**
     * \brief Returns texture
    */
    const plug::Texture &getTexture() const;

    /**
     * \brief Draws vertex array
    */
    virtual void draw(const plug::VertexArray& array) override;

    /**
     * \brief Draws texture
    */
    virtual void draw(const plug::VertexArray& array, const plug::Texture& texture) override;

    /**
     * \brief Clear texture with specific color
    */
    virtual void clear(plug::Color color) override;

    /**
     * \brief Deprecated method
    */
    virtual void setActive(bool active) override {}

    /**
     * \brief Returns texture size
    */
    plug::Vec2d getSize() const;

    /**
     * \brief Returns SFML texture directly
    */
    const sf::Texture &getSFMLTexture() const;

    /**
     * \brief Delete internal texture
    */
    virtual ~RenderTexture() override;

private:
    /**
     * \brief Sets is_changed value
    */
    void setChanged(bool is_changed_) const;

    /**
     * \brief Shows if texture has changed
    */
    bool isChanged() const;

    sf::RenderTexture render_texture;   ///< Texture to draw on
    plug::Texture *inner_texture;       ///< Buffer for getTexture optimization
    mutable bool is_changed;            ///< True if texture has changed
};


#endif
