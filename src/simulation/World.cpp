#include "World.hpp"

using namespace simulation;

struct World::WorldImpl {
  sptr<Track> track;
  uptr<Car> car;

  WorldImpl(const sptr<Track> &track, const CarDef &carDef) : track(track) {
    auto startState = track->StartPosAndOrientation();
    car = make_unique<Car>(carDef, startState.first, startState.second);
  }

  void Render(renderer::Renderer *renderer) const {
    track->Render(renderer);
    car->Render(renderer);
  }

  void Update(float seconds) { car->Update(seconds, track.get()); }

  Car *GetCar(void) { return car.get(); }
};

World::World(const sptr<Track> &track, const CarDef &carDef) : impl(new WorldImpl(track, carDef)) {}
World::~World() = default;

void World::Render(renderer::Renderer *renderer) const { impl->Render(renderer); }

void World::Update(float seconds) { impl->Update(seconds); }

Car *World::GetCar(void) { return impl->GetCar(); }
