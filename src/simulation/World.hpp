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

  void Update(float seconds);
  Car* GetCar(void);

private:
  struct WorldImpl;
  uptr<WorldImpl> impl;
};
}
