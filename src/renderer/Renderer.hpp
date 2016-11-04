#pragma once

#include "../common/ColorRGB.hpp"
#include "../math/Vector2.hpp"
#include <utility>

namespace renderer {

class Renderer {
public:
  virtual void SwapBuffers(void) = 0;

  virtual void Focus(const Vector2 &point, float viewportWidth) = 0;

  virtual void DrawCircle(const Vector2 &pos, float radius, const ColorRGB &c) = 0;
  virtual void DrawRectangle(const Vector2 &halfExtents, const Vector2 &pos, const ColorRGB &c) = 0;
  virtual void DrawLine(const std::pair<Vector2, ColorRGB> &start,
                        const std::pair<Vector2, ColorRGB> &end) = 0;

  virtual void DrawHUDCircle(const Vector2 &pos, float radius, const ColorRGB &c) = 0;
};
}
