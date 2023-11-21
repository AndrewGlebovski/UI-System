/**
 * \file
 * \brief Contains implementation of basic draw shapes
*/


#include "widget/shape.hpp"


RectShape::RectShape(const Vec2d position_, const Vec2d size_, Color color_) :
    position(position_), size(size_), color(color_), border_thickness(0), border_color() {}


void RectShape::setBorder(double border_thickness_, Color border_color_) {
    border_thickness = border_thickness_;
    border_color = border_color_;
}


const Vec2d &RectShape::getPosition() const {
    return position;
}


void RectShape::setPosition(const Vec2d &position_) {
    position = position_;
}


const Vec2d &RectShape::getSize() const {
    return size;
}


void RectShape::setSize(const Vec2d &size_) {
    size = size_;
}


const Color &RectShape::getColor() const {
    return color;
}


void RectShape::setColor(const Color &color_) {
    color = color_;
}


VertexArray RectShape::getVertexArray() const {
    if (isEqual(border_thickness, 0)) {
        VertexArray array(TriangleFan, 4);

        array[0] = Vertex(position, color);
        array[1] = Vertex(Vec2d(position.x, position.y + size.y), color);
        array[2] = Vertex(position + size, color);
        array[3] = Vertex(Vec2d(position.x + size.x, position.y), color);

        return array;
    }
    else {
        VertexArray array(Quads, 8);

        array[0] = Vertex(position, border_color);
        array[1] = Vertex(Vec2d(position.x, position.y + size.y), border_color);
        array[2] = Vertex(position + size, border_color);
        array[3] = Vertex(Vec2d(position.x + size.x, position.y), border_color);
        array[4] = Vertex(position + Vec2d(border_thickness, border_thickness), color);
        array[5] = Vertex(Vec2d(position.x + border_thickness, position.y + size.y - border_thickness), color);
        array[6] = Vertex(position + size - Vec2d(border_thickness, border_thickness), color);
        array[7] = Vertex(Vec2d(position.x + size.x - border_thickness, position.y + border_thickness), color);
        
        return array;
    }
}
