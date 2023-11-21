/**
 * \file
 * \brief Contains interface basic draw shapes
*/


#ifndef _SHAPE_H_
#define _SHAPE_H_


#include "widget/vertex.hpp"


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
    const Color &getColor() const;
    
    /**
     * \brief Sets color
    */
    void setColor(const Color &color_);

    /**
     * \brief Converts rectangle to array of vertices
    */
    VertexArray getVertexArray() const;

private:
    Vec2d position;
    Vec2d size;
    Color color;
    double border_thickness;
    Color border_color;
};


#endif
