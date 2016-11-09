
#include "Car.hpp"
#include "../Constants.hpp"
#include "../math/Geometry.hpp"
#include "../math/Math.hpp"
#include <cmath>

using namespace simulation;

static const ColorRGB CAR_CIRCLE_COLOR = ColorRGB(1.0f, 0.5f, 0.5f);
static const ColorRGB CAR_ARROW_COLOR = ColorRGB::White();
static const ColorRGB CAR_ACCEL_COLOR = ColorRGB::Red();
static const ColorRGB CAR_TURN_COLOR = ColorRGB::Green();

static constexpr float FOV_PER_PIXEL = EYE_FOV / static_cast<float>(PIXELS_PER_EYE);
static constexpr float FOV_PER_SAMPLE = FOV_PER_PIXEL / static_cast<float>(SAMPLER_PER_PIXEL);

static constexpr float FOV_PER_SONAR_PIXEL = SONAR_FOV / static_cast<float>(SONAR_PIXELS);
static constexpr float FOV_PER_SONAR_SAMPLE =
    FOV_PER_SONAR_PIXEL / static_cast<float>(SAMPLES_PER_SONAR_PIXEL);

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
  pair<Vector2, vector<TrackRayIntersection>> sonarRays;

  CarImpl(const CarDef &def, Vector2 startPos, Vector2 startOrientation)
      : def(def), pos(startPos), velocity(0.0f, 0.0f), forward(startOrientation), turnFrac(0.0f),
        accelFrac(0.0f) {
    left = forward.rotated(static_cast<float>(M_PI) / 2.0f);
  }

  void Render(renderer::Renderer *renderer) const {
    // Draw the eye rays.
    float fade = 0.2f;
    // for (const auto &ler : leftEyeRays.second) {
    //   renderer->DrawLine(make_pair(leftEyeRays.first, ler.color * fade),
    //                      make_pair(ler.pos, ler.color * fade));
    // }
    // for (const auto &rer : rightEyeRays.second) {
    //   renderer->DrawLine(make_pair(rightEyeRays.first, rer.color * fade),
    //                      make_pair(rer.pos, rer.color * fade));
    // }
    // for (const auto &sr : sonarRays.second) {
    //   renderer->DrawLine(make_pair(sonarRays.first, ColorRGB::White()),
    //                      make_pair(sr.pos, ColorRGB::White()));
    // }

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
  }

  void SetAcceleration(float amount) { accelFrac = amount; }

  void SetTurn(float amount) { turnFrac = amount; }

  bool Update(float seconds, Track *track) {
    forward.rotate(seconds * turnFrac * def.turnRate);
    left = forward.rotated(static_cast<float>(M_PI) / 2.0f);

    velocity += forward * (seconds * accelFrac * def.accelRate);
    velocity *= powf(CAR_VELOCITY_DECAY, seconds);

    Vector2 prevPos = pos;
    pos += velocity * seconds;

    return checkCollisions(seconds, track, prevPos);

    // sonarRays.first = pos;
    // sonarRays.second.clear();
    // sampleFromSonarPosition(track, pos, sonarRays.second);
    // sampleEyes(track);
  }

  float MaxSpeed(void) const {
    const float a = powf(CAR_VELOCITY_DECAY, STEP_LENGTH_SECS);
    return a * def.accelRate * STEP_LENGTH_SECS / (1.0f - a);
  }

  Vector2 RelVelocity(void) const {
    float angle = atan2f(forward.y, forward.x);
    return velocity.rotated(-angle);
  }

  float RelHeading(const Vector2 &target) const {
    float angle = atan2f(forward.y, forward.x);
    Vector2 corrected = target.rotated(-angle);
    return atan2f(corrected.y, corrected.x);
  }

  bool checkCollisions(float seconds, Track *track, const Vector2 &prevPos) {
    bool haveCollision = checkCollisionsRayMethod(seconds, track, prevPos);
    haveCollision |= checkCollisionsDisplacementMethod(seconds, track);
    if (haveCollision) {
      velocity *= CAR_VELOCITY_COLLISION_DECAY;
    }
    return haveCollision;
  }

  bool checkCollisionsRayMethod(float seconds, Track *track, const Vector2 &prevPos) {
    Vector2 displacementDir = pos - prevPos;
    if (displacementDir.length2() < Geometry::EPSILON) {
      return false;
    }
    displacementDir.normalise();
    Maybe<TrackRayIntersection> tIntersect = track->IntersectRay(prevPos, displacementDir);

    if (!tIntersect.valid() ||
        Geometry::PointSegmentDist(tIntersect.val().pos, prevPos, pos).second > (def.size / 2.0f)) {
      return false;
    }

    velocity = velocity.reflected(tIntersect.val().normal);

    float h =
        (def.size / 2.0f) / -displacementDir.dotProduct(tIntersect.val().normal) + 0.05 * def.size;
    pos = tIntersect.val().pos - displacementDir * h;

    return true;
  }

  bool checkCollisionsDisplacementMethod(float seconds, Track *track) {
    Vector2 netDisplacement;
    bool haveCollision = false;

    // We do this iteratively, up to N times, in case there is a charp concave pair of walls
    // such that displacing from a wall along its normal actualy makes you intersect the other
    // wall.
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

    if (haveCollision && netDisplacement.length2() > Geometry::EPSILON) {
      netDisplacement.normalise();
      velocity -= netDisplacement * 2.0f * (netDisplacement.dotProduct(velocity));
    }

    return haveCollision;
  }

  void sampleEyes(Track *track) {
    leftEyeRays.first = pos + left * (def.eyeSeparation / 2.0f);
    leftEyeRays.second.clear();
    sampleFromEyePosition(track, CAR_EYE_ROTATION, leftEyeRays.first, leftEyeRays.second);

    rightEyeRays.first = pos - left * (def.eyeSeparation / 2.0f);
    rightEyeRays.second.clear();
    sampleFromEyePosition(track, -CAR_EYE_ROTATION, rightEyeRays.first, rightEyeRays.second);

    sonarRays.first = pos;
    sonarRays.second.clear();
    sampleFromSonarPosition(track, pos, sonarRays.second);
  }

  void sampleFromEyePosition(Track *track, float forwardRot, const Vector2 &eyePos,
                             vector<TrackRayIntersection> &samplesOut) {
    assert(samplesOut.empty());

    Vector2 rForward = forward.rotated(forwardRot);
    Vector2 pixelRay = rForward.rotated(EYE_FOV / 2.0f - FOV_PER_PIXEL / 2.0f);
    for (unsigned pi = 0; pi < PIXELS_PER_EYE; pi++) {
      ColorRGB avrgColor;
      Vector2 avrgNormal;
      Vector2 avrgPosition;
      unsigned numSamples = 0;

      Vector2 sampleRay = pixelRay.rotated(FOV_PER_PIXEL / 2.0f - FOV_PER_SAMPLE / 2.0f);
      for (unsigned si = 0; si < SAMPLER_PER_PIXEL; si++) {
        Maybe<TrackRayIntersection> trackIntersection = track->IntersectRay(eyePos, sampleRay);
        if (trackIntersection.valid()) {
          avrgPosition += trackIntersection.val().pos;
          avrgNormal += trackIntersection.val().normal;
          avrgColor += trackIntersection.val().color;
          numSamples++;
        }
        sampleRay.rotate(-FOV_PER_SAMPLE);
      }

      numSamples = std::max<unsigned>(1, numSamples);
      avrgColor *= 1.0f / static_cast<float>(numSamples);
      avrgPosition *= 1.0f / static_cast<float>(numSamples);
      samplesOut.emplace_back(avrgPosition, avrgNormal, avrgColor);

      pixelRay.rotate(-FOV_PER_PIXEL);
    }
  }

  pair<vector<ColorRGB>, vector<ColorRGB>> EyeView(Track *track) {
    sampleEyes(track);

    pair<vector<ColorRGB>, vector<ColorRGB>> result;
    result.first.reserve(leftEyeRays.second.size());
    result.second.reserve(rightEyeRays.second.size());

    for (const auto &ler : leftEyeRays.second) {
      result.first.push_back(ler.color);
    }
    for (const auto &rer : rightEyeRays.second) {
      result.second.push_back(rer.color);
    }
    return result;
  }

  vector<double> SonarView(Track *track) {
    vector<TrackRayIntersection> samples;
    sampleFromSonarPosition(track, pos, samples);

    vector<double> result;
    result.reserve(samples.size());

    for (const auto &s : samples) {
      result.push_back(std::min(20.0f, s.pos.distanceTo(pos)) / 20.0f);
    }

    return result;
  }

  void sampleFromSonarPosition(Track *track, const Vector2 &eyePos,
                               vector<TrackRayIntersection> &samplesOut) {
    assert(samplesOut.empty());

    Vector2 pixelRay = forward.rotated(SONAR_FOV / 2.0f - FOV_PER_SONAR_PIXEL / 2.0f);
    for (unsigned pi = 0; pi < SONAR_PIXELS; pi++) {
      ColorRGB avrgColor;
      Vector2 avrgNormal;
      Vector2 avrgPosition;
      unsigned numSamples = 0;

      Vector2 sampleRay =
          pixelRay.rotated(FOV_PER_SONAR_PIXEL / 2.0f - FOV_PER_SONAR_SAMPLE / 2.0f);
      for (unsigned si = 0; si < SAMPLES_PER_SONAR_PIXEL; si++) {
        Maybe<TrackRayIntersection> trackIntersection = track->IntersectRay(eyePos, sampleRay);
        if (trackIntersection.valid()) {
          avrgPosition += trackIntersection.val().pos;
          avrgNormal += trackIntersection.val().normal;
          avrgColor += trackIntersection.val().color;
          numSamples++;
        }
        sampleRay.rotate(-FOV_PER_SONAR_SAMPLE);
      }

      numSamples = std::max<unsigned>(1, numSamples);
      avrgColor *= 1.0f / static_cast<float>(numSamples);
      avrgPosition *= 1.0f / static_cast<float>(numSamples);
      samplesOut.emplace_back(avrgPosition, avrgNormal, avrgColor);

      pixelRay.rotate(-FOV_PER_SONAR_PIXEL);
    }
  }
};

Car::Car(const CarDef &def, Vector2 startPos, Vector2 startOrientation)
    : impl(new CarImpl(def, startPos, startOrientation)) {}

Car::~Car() = default;

void Car::Render(renderer::Renderer *renderer) const { impl->Render(renderer); }

void Car::SetAcceleration(float amount) { impl->SetAcceleration(amount); }

void Car::SetTurn(float amount) { impl->SetTurn(amount); }

bool Car::Update(float seconds, Track *track) { return impl->Update(seconds, track); }

Vector2 Car::GetPos(void) const { return impl->pos; }

float Car::MaxSpeed(void) const { return impl->MaxSpeed(); }

Vector2 Car::RelVelocity(void) const { return impl->RelVelocity(); }

float Car::RelHeading(const Vector2 &target) const { return impl->RelHeading(target); }

pair<vector<ColorRGB>, vector<ColorRGB>> Car::EyeView(Track *track) { return impl->EyeView(track); }

vector<double> Car::SonarView(Track *track) { return impl->SonarView(track); }
