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
  float size;
  float eyeSeparation;

  Vector2 pos;
  Vector2 velocity;

  Vector2 forward;
  Vector2 left; // useful for computing eye positions.

  float turnRate;
  float accelRate;

public:
  Car(float size, float eyeSeparation, Vector2 startPos, Vector2 startOrientation);

  void Render(renderer::Renderer *renderer);

  void SetAcceleration(float amount);
  void SetTurnRate(float amount);
  void Update(float seconds);

  pair<vector<ColorRGB>, vector<ColorRGB>> EyeView(Track *track);

private:
  Vector2 leftEyePosition(void);
  Vector2 rightEyePosition(void);
};
}
