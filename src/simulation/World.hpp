#pragma once

#include "../common/Common.hpp"
#include "../renderer/Renderer.hpp"
#include "Car.hpp"
#include "Track.hpp"

namespace simulation {

class World {
public:
  World(const sptr<Track> &track, const CarDef &carDef);
  ~World();

  void Render(renderer::Renderer *renderer) const;

  float Update(float seconds);

  float GetProgress(void);
  Car* GetCar(void);
  Track* GetTrack(void);

private:
  struct WorldImpl;
  uptr<WorldImpl> impl;
};
}
