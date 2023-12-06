/**
 * @file VertexArray.h
 * @author MeerkatBoss (solodovnikov.ia@phystech.edu)
 *
 * @brief Vertex array class
 *
 * @version 0.1
 * @date 2023-11-16
 *
 * @copyright Copyright MeerkatBoss (c) 2023
 */
#ifndef __PLUG_GRAPHICS_VERTEX_ARRAY_H
#define __PLUG_GRAPHICS_VERTEX_ARRAY_H

#include <cassert>
#include <cstddef>

#include "Plug/Color.h"
#include "Plug/Graphics/PrimitiveType.h"
#include "Plug/Math.h"

namespace plug {

/**
 * @brief Geometrical vertex
 *
 */
class Vertex {
public:
  /**
   * @brief Vertex position in piskels (SFML screen units, equal to 1 pixel if
   * the window is not resizable)
   *
   */
  Vec2d position;

  /**
   * @brief Coordinate of the vertex on the texture (from 0 to 1 on each axis)
   *
   */
  Vec2d tex_coords;

  /**
   * @brief Vertex color (can be used to transfer some additional data to the
   * GPU)
   *
   */
  Color color;
};

/**
 * @brief Vertex buffer with defined primitive type and index-based vertex
 * enumeration
 *
 */
class VertexArray {
public:
  VertexArray(PrimitiveType type, size_t size);

  VertexArray(const VertexArray &other);

  VertexArray &operator=(const VertexArray &other);

  ~VertexArray(void);

  /**
   * @brief Get rendering primitive
   *
   * @return PrimitiveType
   */
  PrimitiveType getPrimitive(void) const;

  /**
   * @brief Set rendering primitive
   *
   * @param type
   */
  void setPrimitive(PrimitiveType type);

  /**
   * @brief Get number of vertices in the array
   *
   */
  size_t getSize(void) const;

  /**
   * @brief Resize the array to fit specified number of vertices
   *
   * @param new_size
   */
  void resize(size_t new_size);

  /**
   * @brief Add vertex to the end of the array
   *
   * @param vertex
   */
  void appendVertex(const Vertex &vertex);

  Vertex &operator[](size_t index);

  const Vertex &operator[](size_t index) const;

private:
  PrimitiveType m_type;

  Vertex *m_data;
  size_t m_size;
  size_t m_capacity;

  void reserve(size_t new_capacity);
};

inline VertexArray::VertexArray(PrimitiveType type, size_t size = 0)
    : m_type(type), m_size(size), m_capacity(std::max(size, 1ul)) {
  m_data = new Vertex[m_capacity];
}

inline VertexArray::VertexArray(const VertexArray &other)
    : m_type(other.m_type), m_size(other.m_size), m_capacity(other.m_capacity) {
  m_data = new Vertex[m_capacity];
  for (size_t ind = 0; ind < m_size; ++ind) {
    m_data[ind] = other.m_data[ind];
  }
}

inline VertexArray &VertexArray::operator=(const VertexArray &other) {
  if (this == &other) {
    return *this;
  }

  delete m_data;

  m_type = other.m_type;
  m_size = other.m_size;
  m_capacity = other.m_capacity;

  m_data = new Vertex[m_capacity];
  for (size_t ind = 0; ind < m_size; ++ind) {
    m_data[ind] = other.m_data[ind];
  }

  return *this;
}

inline VertexArray::~VertexArray(void) {
  m_size = m_capacity = -1;
  delete[] m_data;
}

inline PrimitiveType VertexArray::getPrimitive(void) const { return m_type; }

inline void VertexArray::setPrimitive(PrimitiveType type) { m_type = type; }

inline size_t VertexArray::getSize(void) const { return m_size; }

inline void VertexArray::resize(size_t new_size) {
  if (new_size <= m_capacity) {
    m_size = new_size;
    return;
  }

  reserve(new_size);
  m_size = new_size;
}

inline void VertexArray::appendVertex(const Vertex &vertex) {
  if (m_size == m_capacity) {
    reserve(m_capacity * 2);
  }
  assert(m_size < m_capacity);

  m_data[m_size++] = vertex;
}

inline Vertex &VertexArray::operator[](size_t index) {
  assert(index < m_size);
  return m_data[index];
}

inline const Vertex &VertexArray::operator[](size_t index) const {
  assert(index < m_size);
  return m_data[index];
}

inline void VertexArray::reserve(size_t new_capacity) {
  assert(new_capacity > m_capacity);

  Vertex *new_data = new Vertex[new_capacity];
  for (size_t ind = 0; ind < m_size; ++ind) {
    new_data[ind] = m_data[ind];
  }
  delete[] m_data;

  m_data = new_data;
  m_capacity = new_capacity;
}

} // end namespace plug
#endif /* VertexArray.h */
