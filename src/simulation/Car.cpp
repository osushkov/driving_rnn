
#include "Car.hpp"
#include "../math/Math.hpp"
#include <cmath>

using namespace simulation;

static const ColorRGB CAR_CIRCLE_COLOR = ColorRGB(1.0f, 0.5f, 0.5f);
static const ColorRGB CAR_ARROW_COLOR = ColorRGB::White();

static constexpr float CAR_VELOCITY_DECAY = 0.75f; // per second

struct Car::CarImpl {
  CarDef def;

  Vector2 pos;
  Vector2 velocity;

  Vector2 forward;
  Vector2 left; // useful for computing eye positions.

  float turnFrac;
  float accelFrac;

  CarImpl(const CarDef &def, Vector2 startPos, Vector2 startOrientation)
      : def(def), pos(startPos), velocity(0.0f, 0.0f),
        forward(startOrientation), turnFrac(0.0f), accelFrac(0.0f) {
    left = forward.rotated(static_cast<float>(M_PI) / 2.0f);
  }

  void Render(renderer::Renderer *renderer) const {
    renderer->DrawCircle(pos, def.size / 2.0f, CAR_CIRCLE_COLOR);

    float arrowRadius = 0.8f * def.size / 2.0f;
    float arrowAngle = math::Deg2Rad(140.0f);
    Vector2 fPoint = pos + forward * arrowRadius;
    Vector2 lPoint = pos + forward.rotated(arrowAngle) * arrowRadius;
    Vector2 rPoint = pos + forward.rotated(-arrowAngle) * arrowRadius;

    renderer->DrawLine(make_pair(fPoint, CAR_ARROW_COLOR), make_pair(lPoint, CAR_ARROW_COLOR));
    renderer->DrawLine(make_pair(fPoint, CAR_ARROW_COLOR), make_pair(rPoint, CAR_ARROW_COLOR));
  }

  void SetAcceleration(float amount) { accelFrac = amount; }

  void SetTurn(float amount) { turnFrac = amount; }

  void Update(float seconds, Track *track) {
    forward.rotate(seconds * turnFrac * def.turnRate);
    left = forward.rotated(static_cast<float>(M_PI) / 2.0f);

    velocity += forward * (seconds * accelFrac * def.accelRate);
    velocity *= powf(CAR_VELOCITY_DECAY, seconds);

    pos += velocity * seconds;
  }

  Vector2 GetPos(void) const {
    return pos;
  }

  pair<vector<ColorRGB>, vector<ColorRGB>> EyeView(Track *track) {
    return make_pair(vector<ColorRGB>(), vector<ColorRGB>());
  }

  Vector2 leftEyePosition(void) { return pos + left * (def.eyeSeparation / 2.0f); }
  Vector2 rightEyePosition(void) { return pos - left * (def.eyeSeparation / 2.0f); }
};

Car::Car(const CarDef &def, Vector2 startPos, Vector2 startOrientation)
    : impl(new CarImpl(def, startPos, startOrientation)) {}

Car::~Car() = default;

void Car::Render(renderer::Renderer *renderer) const { impl->Render(renderer); }

void Car::SetAcceleration(float amount) { impl->SetAcceleration(amount); }

void Car::SetTurn(float amount) { impl->SetTurn(amount); }

void Car::Update(float seconds, Track *track) { impl->Update(seconds, track); }

Vector2 Car::GetPos(void) const { return impl->GetPos(); }

pair<vector<ColorRGB>, vector<ColorRGB>> Car::EyeView(Track *track) { return impl->EyeView(track); }
