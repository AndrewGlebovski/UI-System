/**
 * \file
 * \brief Contains interface basic draw shapes
*/


#ifndef _SHAPE_H_
#define _SHAPE_H_


#include "widget/render_target.hpp"
#include "canvas/canvas/canvas.hpp"


class RectShape {
public:
    RectShape(const plug::Vec2d position_, const plug::Vec2d size_, plug::Color color_);

    void setBorder(double border_thickness_, plug::Color border_color_);

    /**
     * \brief Returns position
    */
    const plug::Vec2d &getPosition() const;
    
    /**
     * \brief Sets position
    */
    void setPosition(const plug::Vec2d &position_);
    
    /**
     * \brief Returns size
    */
    const plug::Vec2d &getSize() const;
    
    /**
     * \brief Sets size
    */
    void setSize(const plug::Vec2d &size_);

    /**
     * \brief Returns color
    */
    plug::Color getColor() const;
    
    /**
     * \brief Sets color
    */
    void setColor(plug::Color color_);

    /**
     * \brief Draws rectangle on plug::RenderTarget
    */
    void draw(plug::RenderTarget &target) const;

    /**
     * \brief Draws rectangle on plug::Canvas
    */
    void draw(plug::Canvas &canvas) const;

private:
    void setCenter(plug::VertexArray &array) const;
    void setTopBorder(plug::VertexArray &array) const;
    void setBottomBorder(plug::VertexArray &array) const;
    void setLeftBorder(plug::VertexArray &array) const;
    void setRightBorder(plug::VertexArray &array) const;

    plug::Vec2d position;
    plug::Vec2d size;
    plug::Color color;
    double border_thickness;
    plug::Color border_color;
};


class CircleShape {
public:
    /**
     * \brief Constructs circle shape
     * \note Position of center
    */
    CircleShape(const plug::Vec2d position_, double radius_, plug::Color color_);

    /**
     * \brief Returns position
     * \note Position of center
    */
    const plug::Vec2d &getPosition() const;
    
    /**
     * \brief Sets position
     * \note Position of center
    */
    void setPosition(const plug::Vec2d &position_);
    
    /**
     * \brief Returns radius
    */
    double getRadius() const;
    
    /**
     * \brief Sets radius
    */
    void setRadius(double radius_);

    /**
     * \brief Returns color
    */
    plug::Color getColor() const;
    
    /**
     * \brief Sets color
    */
    void setColor(plug::Color color_);

    /**
     * \brief Draws circle on plug::RenderTarget
    */
    void draw(plug::RenderTarget &target) const;

    /**
     * \brief Draws circle on plug::Canvas
    */
    void draw(plug::Canvas &canvas) const;

private:
    void setVertexArray(plug::VertexArray &array) const;

    plug::Vec2d position;
    double radius;
    plug::Color color;
};


/// For convenient texture draw
class TextureShape {
public:
    TextureShape(const plug::Texture &texture_);

    TextureShape(const TextureShape&) = default;
    
    TextureShape &operator = (const TextureShape&) = default;

    /**
     * \brief Draws texture on target at specified position with specified size
     * \note plug::Texture scales if size is not equal to texture size
    */
    void draw(plug::RenderTarget &target, const plug::Vec2d &position, const plug::Vec2d &size);

    /**
     * \brief Draws text on canvas at specified position with specified size
     * \note Text scales if size is not equal to texture size
    */
    void draw(plug::Canvas &canvas, const plug::Vec2d &position, const plug::Vec2d &size);

    /**
     * \brief Returns stored texture
    */
    const plug::Texture &getTexture() const;

private:
    plug::VertexArray array;          ///< Vertex array for texture drawing
    const plug::Texture &texture;     ///< plug::Texture reference
};


/// For convenient text draw
class TextShape {
public:
    /**
     * \brief Constructs text shape
     * \note plug::Texture should be big enough to hold the text
    */
    TextShape(const sf::Text &text_, const plug::Vec2d &texture_size_);

    /**
     * \brief Constructs text shape
     * \note plug::Texture size is set to text local bounds
    */
    TextShape(const sf::Text &text_);

    /**
     * \brief Sets text offset from internal texture
    */
    void setTextOffset(const plug::Vec2d &offset);

    /**
     * \brief Returns text offset from internal texture
    */
    plug::Vec2d getTextOffset() const;

    /**
     * \brief Returns text reference
    */
    const sf::Text &getText() const;

    TextShape(const TextShape&) = delete;

    TextShape &operator = (const TextShape&) = delete;

    /**
     * \brief Returns size of internal texture
    */
    plug::Vec2d getTextureSize() const;

    /**
     * \brief Sets text color
    */
    void setColor(plug::Color color);

    /**
     * \brief Set text string
    */
    void setText(const char *str);

    /**
     * \brief Draws text on target at specified position with specified size
     * \note Text scales if size is not equal to texture size
    */
    void draw(plug::RenderTarget &target, const plug::Vec2d &position, const plug::Vec2d &size);

    /**
     * \brief Draws text on target at specified position with specified size
     * \note Text scales if size is not equal to texture size
    */
    void draw(plug::Canvas &canvas, const plug::Vec2d &position, const plug::Vec2d &size);

private:
    /**
     * \brief Redraws text on internal texture
    */
    void render();

    sf::Text sf_text;                   ///< Text itself
    plug::Texture texture;                    ///< Internal texture for handling text image
    TextureShape shape;                 ///< Shape for convenient texture draw
    sf::RenderTexture render_texture;   ///< Tool texture for text drawing
};


#endif
