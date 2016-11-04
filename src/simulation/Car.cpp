
#include "Car.hpp"
#include "../math/Geometry.hpp"
#include "../math/Math.hpp"
#include <cmath>

using namespace simulation;

static const ColorRGB CAR_CIRCLE_COLOR = ColorRGB(1.0f, 0.5f, 0.5f);
static const ColorRGB CAR_ARROW_COLOR = ColorRGB::White();
static const ColorRGB CAR_ACCEL_COLOR = ColorRGB::Red();
static const ColorRGB CAR_TURN_COLOR = ColorRGB::Green();

static constexpr float CAR_VELOCITY_DECAY = 0.9f; // per second
static constexpr float CAR_VELOCITY_COLLISION_DECAY = 0.3f;

static constexpr float EYE_FOV = 90.0f * static_cast<float>(M_PI) / 180.0f;
static constexpr unsigned PIXELS_PER_EYE = 15;
static constexpr unsigned SAMPLER_PER_PIXEL = 5;
static constexpr float FOV_PER_PIXEL = EYE_FOV / static_cast<float>(PIXELS_PER_EYE);
static constexpr float FOV_PER_SAMPLE = FOV_PER_PIXEL / static_cast<float>(SAMPLER_PER_PIXEL);

struct Car::CarImpl {
  CarDef def;

  Vector2 pos;
  Vector2 velocity;

  Vector2 forward;
  Vector2 left; // useful for computing eye positions.

  float turnFrac;
  float accelFrac;

  pair<Vector2, vector<TrackRayIntersection>> leftEyeRays;
  pair<Vector2, vector<TrackRayIntersection>> rightEyeRays;

  CarImpl(const CarDef &def, Vector2 startPos, Vector2 startOrientation)
      : def(def), pos(startPos), velocity(0.0f, 0.0f), forward(startOrientation), turnFrac(0.0f),
        accelFrac(0.0f) {
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

    // Draw the acceleration indicator.
    float sign = accelFrac > 0.0f ? 1.0f : -1.0f;
    Vector2 accelIndicatorStart = pos + forward * (sign * def.size / 2.0f);
    Vector2 accelIndicatorEnd = accelIndicatorStart + forward * (accelFrac * def.size * 0.7f);
    renderer->DrawLine(make_pair(accelIndicatorStart, CAR_ACCEL_COLOR),
                       make_pair(accelIndicatorEnd, CAR_ACCEL_COLOR));

    // Draw the turn indicator.
    sign = turnFrac > 0.0f ? 1.0f : -1.0f;
    Vector2 turnIndicatorStart = pos + left * (sign * def.size / 2.0f);
    Vector2 turnIndicatorEnd = turnIndicatorStart + left * (turnFrac * def.size * 0.7f);
    renderer->DrawLine(make_pair(turnIndicatorStart, CAR_TURN_COLOR),
                       make_pair(turnIndicatorEnd, CAR_TURN_COLOR));

    // Draw the eye rays.
    for (const auto& ler : leftEyeRays.second) {
      renderer->DrawLine(make_pair(leftEyeRays.first, ColorRGB::White()),
                         make_pair(ler.pos, ColorRGB::White()));
    }
  }

  void SetAcceleration(float amount) { accelFrac = amount; }

  void SetTurn(float amount) { turnFrac = amount; }

  void Update(float seconds, Track *track) {
    forward.rotate(seconds * turnFrac * def.turnRate);
    left = forward.rotated(static_cast<float>(M_PI) / 2.0f);

    velocity += forward * (seconds * accelFrac * def.accelRate);
    velocity *= powf(CAR_VELOCITY_DECAY, seconds);

    pos += velocity * seconds;

    checkCollisions(seconds, track);
    sampleEyes(track);
  }

  void checkCollisions(float seconds, Track *track) {
    Vector2 netDisplacement;
    bool haveCollision = false;

    for (unsigned i = 0; i < 3; i++) {
      vector<CollisionResult> collisions = track->IntersectSphere(pos, def.size / 2.0f);
      if (collisions.empty()) {
        break;
      }

      CollisionResult &c = collisions.front();

      float normalDist = (pos - c.collisionPoint).dotProduct(c.collisionNormal);
      assert(normalDist <= (def.size / 2.0f + Geometry::EPSILON));

      Vector2 displacement = c.collisionNormal * (def.size / 2.0f - normalDist);
      pos += displacement;

      netDisplacement += displacement;
      haveCollision = true;
    }

    if (haveCollision) {
      netDisplacement.normalise();
      velocity *= CAR_VELOCITY_COLLISION_DECAY;
      velocity -= netDisplacement * 1.3f * (netDisplacement.dotProduct(velocity));
    }
  }

  void sampleEyes(Track *track) {
    leftEyeRays.first = pos + left * (def.eyeSeparation / 2.0f);
    leftEyeRays.second.clear();
    sampleFromEyePosition(track, leftEyeRays.first, leftEyeRays.second);

    rightEyeRays.first = pos - left * (def.eyeSeparation / 2.0f);
    rightEyeRays.second.clear();
    sampleFromEyePosition(track, rightEyeRays.first, rightEyeRays.second);
  }

  void sampleFromEyePosition(Track *track, const Vector2 &eyePos,
                             vector<TrackRayIntersection> &samplesOut) {
    assert(samplesOut.empty());

    Vector2 pixelRay = forward.rotated(EYE_FOV / 2.0f  - FOV_PER_PIXEL / 2.0f);
    for (unsigned pi = 0; pi < PIXELS_PER_EYE; pi++) {
      ColorRGB avrgColor;
      Vector2 avrgPosition;
      unsigned numSamples = 0;

      Vector2 sampleRay = pixelRay.rotated(FOV_PER_PIXEL / 2.0f  - FOV_PER_SAMPLE / 2.0f);
      for (unsigned si = 0; si < SAMPLER_PER_PIXEL; si++) {
        Maybe<TrackRayIntersection> trackIntersection = track->IntersectRay(eyePos, sampleRay);
        if (trackIntersection.valid()) {
          avrgColor += trackIntersection.val().color;
          avrgPosition += trackIntersection.val().pos;
          numSamples++;
        }
        sampleRay.rotate(-FOV_PER_SAMPLE);
      }

      avrgColor *= 1.0f / static_cast<float>(numSamples);
      avrgPosition *= 1.0f / static_cast<float>(numSamples);
      samplesOut.emplace_back(avrgPosition, avrgColor);

      pixelRay.rotate(-FOV_PER_PIXEL);
    }
  }

  pair<vector<ColorRGB>, vector<ColorRGB>> EyeView(Track *track) {
    sampleEyes(track);

    pair<vector<ColorRGB>, vector<ColorRGB>> result;
    result.first.reserve(leftEyeRays.second.size());
    result.second.reserve(rightEyeRays.second.size());

    for (const auto& ler : leftEyeRays.second) {
      result.first.push_back(ler.color);
    }
    for (const auto& rer : rightEyeRays.second) {
      result.second.push_back(rer.color);
    }
    return result;
  }
};

Car::Car(const CarDef &def, Vector2 startPos, Vector2 startOrientation)
    : impl(new CarImpl(def, startPos, startOrientation)) {}

Car::~Car() = default;

void Car::Render(renderer::Renderer *renderer) const { impl->Render(renderer); }

void Car::SetAcceleration(float amount) { impl->SetAcceleration(amount); }

void Car::SetTurn(float amount) { impl->SetTurn(amount); }

void Car::Update(float seconds, Track *track) { impl->Update(seconds, track); }

Vector2 Car::GetPos(void) const { return impl->pos; }

pair<vector<ColorRGB>, vector<ColorRGB>> Car::EyeView(Track *track) { return impl->EyeView(track); }
