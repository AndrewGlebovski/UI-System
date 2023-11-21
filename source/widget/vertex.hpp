/**
 * \file
 * \brief Contains interface for my vertex, vertex array, texture and render target
*/


#ifndef _RENDER_TARGET_H_
#define _RENDER_TARGET_H_


#include "common/vector.hpp"
#include "widget/color.hpp"


enum PrimitiveType
{
    Points,
    Lines,
    LineStrip,
    Triangles,
    TriangleStrip,
    TriangleFan,
    Quads
};


struct Vertex {
    Vertex() :
        position(), tex_coords(), color() {}

    Vertex(const Vec2d &position_, const Color color_) :
        position(position_), tex_coords(), color(color_) {}

    Vec2d position;
    Vec2d tex_coords;
    Color color;
};


class VertexArray {
public:
    VertexArray(PrimitiveType type_, size_t size_);

    VertexArray(const VertexArray& other);

    VertexArray& operator=(const VertexArray& other);

    ~VertexArray(void);

    /**
     * \brief Get rendering primitive
    */
    PrimitiveType getPrimitive() const;

    /**
     * \brief Set rendering primitive
    */
    void setPrimitive(PrimitiveType type);

    /**
     * \brief Get number of vertices in the array
    */
    size_t getSize() const;

    /**
     * \brief Resize the array to fit specified number of vertices
    */
    void resize(size_t new_size);

    /**
    * \brief Add vertex to the end of the array
    */
    void appendVertex(const Vertex& vertex);

    Vertex& operator[](size_t index);

    const Vertex& operator[](size_t index) const;

private:
  PrimitiveType type;
  Vertex* data;
  size_t size;
  size_t capacity;
};


#endif
