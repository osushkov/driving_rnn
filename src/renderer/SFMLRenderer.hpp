#pragma once

#include "../common/ColorRGB.hpp"
#include "../common/Common.hpp"
#include "Renderer.hpp"
#include <string>

namespace renderer {

class SFMLRenderer : public Renderer {
public:
  SFMLRenderer(unsigned width, unsigned height, const string &windowName);
  ~SFMLRenderer();

  void SwapBuffers(void) override;

  void Focus(const Vector2 &point, float viewportWidth) override;

  void DrawCircle(const Vector2 &pos, float radius, const ColorRGB &c) override;
  void DrawRectangle(const Vector2 &halfExtents, const Vector2 &pos, const ColorRGB &c) override;
  void DrawLine(const std::pair<Vector2, ColorRGB> &start,
                const std::pair<Vector2, ColorRGB> &end) override;

  void DrawHUDCircle(const Vector2 &pos, float radius, const ColorRGB &c) override;

private:
  struct SFMLRendererImpl;
  uptr<SFMLRendererImpl> impl;
};
}
