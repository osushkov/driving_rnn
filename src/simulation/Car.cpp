
#include "Car.hpp"
#include <cmath>

using namespace simulation;

Car::Car(float size, float eyeSeparation, Vector2 startPos, Vector2 startOrientation)
    : size(size), eyeSeparation(eyeSeparation), pos(startPos), velocity(0.0f, 0.0f),
      forward(startOrientation), turnRate(0.0f), accelRate(0.0f) {
  left = forward.rotated(static_cast<float>(M_PI) / 2.0f);
}

void Car::Render(renderer::Renderer *renderer) {}

void Car::SetAcceleration(float amount) { accelRate = amount; }

void Car::SetTurnRate(float amount) { turnRate = amount; }

void Car::Update(float seconds) {
  forward.rotate(turnRate * seconds);
  left = forward.rotated(static_cast<float>(M_PI) / 2.0f);

  velocity += forward * (seconds * accelRate);
  pos += velocity * seconds;
}

pair<vector<ColorRGB>, vector<ColorRGB>> Car::EyeView(Track *track) {
  return make_pair(vector<ColorRGB>(), vector<ColorRGB>());
}

Vector2 Car::leftEyePosition(void) { return Vector2(); }

Vector2 Car::rightEyePosition(void) { return Vector2(); }
