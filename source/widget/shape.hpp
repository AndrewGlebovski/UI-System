/**
 * \file
 * \brief Contains interface basic draw shapes
*/


#ifndef _SHAPE_H_
#define _SHAPE_H_


#include "widget/vertex.hpp"
#include "widget/render_target.hpp"
#include "canvas/canvas.hpp"


class RectShape {
public:
    RectShape(const Vec2d position_, const Vec2d size_, Color color_);

    void setBorder(double border_thickness_, Color border_color_);

    /**
     * \brief Returns position
    */
    const Vec2d &getPosition() const;
    
    /**
     * \brief Sets position
    */
    void setPosition(const Vec2d &position_);
    
    /**
     * \brief Returns size
    */
    const Vec2d &getSize() const;
    
    /**
     * \brief Sets size
    */
    void setSize(const Vec2d &size_);

    /**
     * \brief Returns color
    */
    Color getColor() const;
    
    /**
     * \brief Sets color
    */
    void setColor(Color color_);

    /**
     * \brief Draws rectangle on RenderTarget
    */
    void draw(RenderTarget &target) const;

    /**
     * \brief Draws rectangle on Canvas
    */
    void draw(Canvas &canvas) const;

private:
    void setCenter(VertexArray &array) const;
    void setTopBorder(VertexArray &array) const;
    void setBottomBorder(VertexArray &array) const;
    void setLeftBorder(VertexArray &array) const;
    void setRightBorder(VertexArray &array) const;

    Vec2d position;
    Vec2d size;
    Color color;
    double border_thickness;
    Color border_color;
};


class CircleShape {
public:
    /**
     * \brief Constructs circle shape
     * \note Position of center
    */
    CircleShape(const Vec2d position_, double radius_, Color color_);

    /**
     * \brief Returns position
     * \note Position of center
    */
    const Vec2d &getPosition() const;
    
    /**
     * \brief Sets position
     * \note Position of center
    */
    void setPosition(const Vec2d &position_);
    
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
    Color getColor() const;
    
    /**
     * \brief Sets color
    */
    void setColor(Color color_);

    /**
     * \brief Draws circle on RenderTarget
    */
    void draw(RenderTarget &target) const;

    /**
     * \brief Draws circle on Canvas
    */
    void draw(Canvas &canvas) const;

private:
    void setVertexArray(VertexArray &array) const;

    Vec2d position;
    double radius;
    Color color;
};


/// For convenient texture draw
class TextureShape {
public:
    TextureShape(const Texture &texture_);

    TextureShape(const TextureShape&) = default;
    
    TextureShape &operator = (const TextureShape&) = default;

    /**
     * \brief Draws texture on target at specified position with specified size
     * \note Texture scales if size is not equal to texture size
    */
    void draw(RenderTarget &target, const Vec2d &position, const Vec2d &size);

    /**
     * \brief Draws text on canvas at specified position with specified size
     * \note Text scales if size is not equal to texture size
    */
    void draw(Canvas &canvas, const Vec2d &position, const Vec2d &size);

    /**
     * \brief Returns stored texture
    */
    const Texture &getTexture() const;

private:
    VertexArray array;          ///< Vertex array for texture drawing
    const Texture &texture;     ///< Texture reference
};


/// For convenient text draw
class TextShape {
public:
    /**
     * \brief Constructs text shape
     * \note Texture should be big enough to hold the text
    */
    TextShape(const sf::Text &text_, const Vec2d &texture_size_);

    /**
     * \brief Constructs text shape
     * \note Texture size is set to text local bounds
    */
    TextShape(const sf::Text &text_);

    /**
     * \brief Sets text offset from internal texture
    */
    void setTextOffset(const Vec2d &offset);

    /**
     * \brief Returns text offset from internal texture
    */
    Vec2d getTextOffset() const;

    /**
     * \brief Returns text reference
    */
    const sf::Text &getText() const;

    TextShape(const TextShape&) = delete;

    TextShape &operator = (const TextShape&) = delete;

    /**
     * \brief Returns size of internal texture
    */
    Vec2d getTextureSize() const;

    /**
     * \brief Sets text color
    */
    void setColor(Color color);

    /**
     * \brief Set text string
    */
    void setText(const char *str);

    /**
     * \brief Draws text on target at specified position with specified size
     * \note Text scales if size is not equal to texture size
    */
    void draw(RenderTarget &target, const Vec2d &position, const Vec2d &size);

    /**
     * \brief Draws text on target at specified position with specified size
     * \note Text scales if size is not equal to texture size
    */
    void draw(Canvas &canvas, const Vec2d &position, const Vec2d &size);

private:
    /**
     * \brief Redraws text on internal texture
    */
    void render();

    sf::Text sf_text;                   ///< Text itself
    Texture texture;                    ///< Internal texture for handling text image
    TextureShape shape;                 ///< Shape for convenient texture draw
    sf::RenderTexture render_texture;   ///< Tool texture for text drawing
};


#endif
