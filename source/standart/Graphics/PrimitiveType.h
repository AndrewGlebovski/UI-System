/**
 * @file PrimitiveType.h
 * @author MeerkatBoss (solodovnikov.ia@phystech.edu)
 *
 * @brief Primitive type enum
 *
 * @version 0.1
 * @date 2023-11-16
 *
 * @copyright Copyright MeerkatBoss (c) 2023
 */
#ifndef __PLUG_GRAPHICS_PRIMITIVE_TYPE_H
#define __PLUG_GRAPHICS_PRIMITIVE_TYPE_H

namespace plug {

/**
 * @brief Rendering primitive (defines how to interpret sequence of vertices)
 *
 */
enum PrimitiveType {
  /**
   * @brief Draw vertices as individual points
   *
   */
  Points,

  /**
   * @brief Draw set of unconnected lines (f.e. [0]---[1]   [2]---[3] [4]---[5]
   * ...)
   *
   */
  Lines,

  /**
   * @brief Draw set of connected lines ([0]---[1]---[2]---[3]---[4]-...)
   *
   */
  LineStrip,

  /**
   * @brief Draw set of unconnected triangles (<0 1 2> <3 4 5> <6 7 8> ...)
   *
   */
  Triangles,

  /**
   * @brief Draw a set of connected triangles where each triangle shares its two
   * last vertices with the next one (<0 1 2> <1 2 3> <2 3 4> <3 4 5> ...)
   *
   */
  TriangleStrip,

  /**
   * @brief Draw a set of triangles connected to a central point (<0 1 2> <0 2
   * 3> <0 3 4> <0 4 5> ...)
   *
   */
  TriangleFan,

  /**
   * @brief Draw a set of unconnected quads ([0 1 2 3] [4 5 6 7] [8 9 10 11]
   * ...)
   *
   */
  Quads,

  /**
   * @brief [deprecated] Replaced with LineStrip
   *
   */
  LinesStrip = LineStrip,

  /**
   * @brief [deprecated] Replaced with TriangleStrip
   *
   */
  TrianglesStrip = TriangleStrip,

  /**
   * @brief [deprecated] Replaced with TriangleFan
   *
   */
  TrianglesFan = TriangleFan,
};

} // namespace plug

#endif /* PrimitiveType.h */
