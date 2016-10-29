
#include "Track.hpp"
#include "../math/Vector2.hpp"
#include <cassert>
#include <vector>

using namespace simulation;

struct Track::TrackImpl {
  TrackImpl(const TrackSpec &spec) {}

  void Render(renderer::Renderer *renderer) {}
};

Track::Track(const TrackSpec &spec) : impl(new TrackImpl(spec)) {}

Track::~Track() = default;

void Track::Render(renderer::Renderer *renderer) { impl->Render(renderer); }
