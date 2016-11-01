
#include "Car.hpp"
#include "../math/Math.hpp"
#include <cmath>

using namespace simulation;

static const ColorRGB CAR_CIRCLE_COLOR = ColorRGB(1.0f, 0.5f, 0.5f);
static const ColorRGB CAR_ARROW_COLOR = ColorRGB::White();

static constexpr float CAR_VELOCITY_DECAY = 0.75f; // per second

struct Car::CarImpl {
  float size;
  float eyeSeparation;

  Vector2 pos;
  Vector2 velocity;

  Vector2 forward;
  Vector2 left; // useful for computing eye positions.

  float turnRate;
  float accelRate;

  CarImpl(float size, float eyeSeparation, Vector2 startPos, Vector2 startOrientation)
      : size(size), eyeSeparation(eyeSeparation), pos(startPos), velocity(0.0f, 0.0f),
        forward(startOrientation), turnRate(0.0f), accelRate(0.0f) {
    left = forward.rotated(static_cast<float>(M_PI) / 2.0f);
  }

  void Render(renderer::Renderer *renderer) {
    renderer->DrawCircle(pos, size/2.0f, CAR_CIRCLE_COLOR);

    float arrowRadius = 0.8f * size / 2.0f;
    float arrowAngle = math::Deg2Rad(140.0f);
    Vector2 fPoint = pos + forward * arrowRadius;
    Vector2 lPoint = pos + forward.rotated(arrowAngle) * arrowRadius;
    Vector2 rPoint = pos + forward.rotated(-arrowAngle) * arrowRadius;

    renderer->DrawLine(make_pair(fPoint, CAR_ARROW_COLOR), make_pair(lPoint, CAR_ARROW_COLOR));
    renderer->DrawLine(make_pair(fPoint, CAR_ARROW_COLOR), make_pair(rPoint, CAR_ARROW_COLOR));
  }

  void SetAcceleration(float amount) { accelRate = amount; }

  void SetTurnRate(float amount) { turnRate = amount; }

  void Update(float seconds) {
    forward.rotate(turnRate * seconds);
    left = forward.rotated(static_cast<float>(M_PI) / 2.0f);

    velocity += forward * (seconds * accelRate);
    velocity *= powf(CAR_VELOCITY_DECAY, seconds);

    pos += velocity * seconds;
  }

  pair<vector<ColorRGB>, vector<ColorRGB>> EyeView(Track *track) {
    return make_pair(vector<ColorRGB>(), vector<ColorRGB>());
  }

  Vector2 leftEyePosition(void) { return pos + left * (size / 4.0f); }
  Vector2 rightEyePosition(void) { return pos - left * (size / 4.0f); }
};



Car::Car(float size, float eyeSeparation, Vector2 startPos, Vector2 startOrientation)
    : impl(new CarImpl(size, eyeSeparation, startPos, startOrientation)) {}

void Car::Render(renderer::Renderer *renderer) { impl->Render(renderer); }

void Car::SetAcceleration(float amount) { impl->SetAcceleration(amount); }

void Car::SetTurnRate(float amount) { impl->SetTurnRate(amount); }

void Car::Update(float seconds) {
  impl->Update(seconds);
}

pair<vector<ColorRGB>, vector<ColorRGB>> Car::EyeView(Track *track) {
  return impl->EyeView(track);
}
