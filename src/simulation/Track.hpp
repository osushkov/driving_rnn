#pragma once

#include "../common/Common.hpp"
#include "../common/Maybe.hpp"
#include "../math/CollisionResult.hpp"
#include "../renderer/Renderer.hpp"
#include <utility>

namespace simulation {

struct TrackSpec {
  float radius;
  float trackMinWidth;
  float trackMaxWidth;

  int numLinePoints;
  float maxModStrength;

  TrackSpec(float radius, float trackMinWidth, float trackMaxWidth, int numLinePoints,
            float maxModStrength)
      : radius(radius), trackMinWidth(trackMinWidth), trackMaxWidth(trackMaxWidth),
        numLinePoints(numLinePoints), maxModStrength(maxModStrength) {

    assert(radius > 0.0f);
    assert(trackMinWidth > 0.0f && trackMinWidth <= trackMaxWidth);
    assert(numLinePoints >= 3);
  }
};

struct TrackRayIntersection {
  Vector2 pos;
  ColorRGB color;

  TrackRayIntersection(const Vector2 &pos, const ColorRGB &color) : pos(pos), color(color) {}
};

class Track {
public:
  Track(const TrackSpec &spec);
  ~Track();

  void Render(renderer::Renderer *renderer) const;

  pair<Vector2, Vector2> StartPosAndOrientation(void) const;

  float DistanceAlongTrack(const Vector2 &point) const;

  Maybe<TrackRayIntersection> IntersectRay(const Vector2 &start, const Vector2 &dir) const;
  vector<CollisionResult> IntersectSphere(const Vector2 &pos, float radius) const;

private:
  struct TrackImpl;
  uptr<TrackImpl> impl;
};
}
