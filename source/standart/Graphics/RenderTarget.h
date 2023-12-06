/**
 * @file RenderTarget.h
 * @author MeerkatBoss (solodovnikov.ia@phystech.edu)
 *
 * @brief RenderTarget class
 *
 * @version 0.1
 * @date 2023-11-16
 *
 * @copyright Copyright MeerkatBoss (c) 2023
 */
#ifndef __PLUG_GRAPHICS_RENDER_TARGET_H
#define __PLUG_GRAPHICS_RENDER_TARGET_H

#include "Plug/Graphics/Texture.h"
#include "Plug/Graphics/VertexArray.h"

namespace plug {

/**
 * @brief Framebuffer object with single color attachment (as texture) and
 * depth-stencil buffer (as RenderBuffer) (just use it as a window wrapper, both
 * SFML and SDL will perform all the required magic by themselves)
 *
 */
class RenderTarget {
public:
  virtual ~RenderTarget(void) = default;

  /**
   * @brief Draw vertex array with specified vertex colors
   *
   * @param array
   */
  virtual void draw(const VertexArray &array) = 0;

  /**
   * @brief Draw vertex array with texture-based coloring
   *
   * @param array
   * @param texture
   */
  virtual void draw(const VertexArray &array, const Texture &texture) = 0;

  /**
   * @brief Fill color attachment with specified color and reset depth and
   * stencil buffers
   *
   * @param color
   */
  virtual void clear(Color color) = 0;

  /**
   * @brief Prepare current render target for rendering raw OpenGL
   *
   * @param[in] active true: bind related framebuffer and push GL states, false:
   * pop saved GL states
   */
  virtual void setActive(bool active) = 0;
};

} // namespace plug

#endif /* RenderTarget.h */
