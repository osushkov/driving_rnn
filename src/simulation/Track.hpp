#pragma once

#include "../common/Common.hpp"
#include "../renderer/Renderer.hpp"

namespace simulation {

struct TrackSpec {
  float radius;
  float trackWidth;

  int numLinePoints;
  int numMods;
  float maxModStrength;

  TrackSpec(float radius, float trackWidth, int numLinePoints, int numMods, float maxModStrength)
      : radius(radius), trackWidth(trackWidth), numLinePoints(numLinePoints), numMods(numMods),
        maxModStrength(maxModStrength) {

    assert(radius > 0.0f);
    assert(trackWidth > 0.0f);
    assert(numLinePoints >= 3);
  }
};

class Track {
public:
  Track(const TrackSpec &spec);
  ~Track();

  void Render(renderer::Renderer *renderer);

private:
  struct TrackImpl;
  uptr<TrackImpl> impl;
};
}
