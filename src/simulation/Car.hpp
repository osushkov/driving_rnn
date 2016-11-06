#pragma once

#include "../common/ColorRGB.hpp"
#include "../common/Common.hpp"
#include "../math/Vector2.hpp"
#include "../renderer/Renderer.hpp"
#include "Track.hpp"

#include <utility>
#include <vector>

namespace simulation {

struct CarDef {
  float size;
  float eyeSeparation;

  float turnRate;
  float accelRate;

  CarDef(float size, float eyeSeparation, float turnRate, float accelRate)
      : size(size), eyeSeparation(eyeSeparation), turnRate(turnRate), accelRate(accelRate) {}
};

class Car {
public:
  Car(const CarDef &def, Vector2 startPos, Vector2 startOrientation);
  ~Car();

  void Render(renderer::Renderer *renderer) const;

  void SetAcceleration(float amount);
  void SetTurn(float amount);
  void Update(float seconds, Track *track);

  Vector2 GetPos(void) const;
  float MaxSpeed(void) const;
  Vector2 RelVelocity(void) const;
  float RelHeading(const Vector2 &target) const;

  pair<vector<ColorRGB>, vector<ColorRGB>> EyeView(Track *track);
  vector<double> SonarView(Track *track);

private:
  struct CarImpl;
  uptr<CarImpl> impl;
};
}
