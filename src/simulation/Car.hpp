#pragma once

#include "../common/ColorRGB.hpp"
#include "../common/Common.hpp"
#include "../math/Vector2.hpp"
#include "../renderer/Renderer.hpp"
#include "Track.hpp"

#include <utility>
#include <vector>

namespace simulation {

class Car {
public:
  Car(float size, float eyeSeparation, Vector2 startPos, Vector2 startOrientation);

  void Render(renderer::Renderer *renderer);

  void SetAcceleration(float amount);
  void SetTurnRate(float amount);
  void Update(float seconds);

  pair<vector<ColorRGB>, vector<ColorRGB>> EyeView(Track *track);

private:
  struct CarImpl;
  uptr<CarImpl> impl;
};
}
