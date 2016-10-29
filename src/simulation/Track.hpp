#pragma once

#include "../common/Common.hpp"
#include "../renderer/Renderer.hpp"

namespace simulation {

struct TrackSpec {
  float radius;
  float trackWidth;

  int numMods;
  float maxModStrength;
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
