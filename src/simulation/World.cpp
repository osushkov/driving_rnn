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

    // Render the pixels seen by each eye in the HUD.
    pair<vector<ColorRGB>, vector<ColorRGB>> eyeView = car->EyeView(track.get());
    float pixelBarSize = 0.6f;
    float pixelSize = pixelBarSize / static_cast<float>(eyeView.first.size());
    for (unsigned i = 0; i < eyeView.first.size(); i++) {
      renderer->DrawHUDCircle(Vector2(-0.75f + i * pixelSize, -0.9f), pixelSize / 2.0f,
                              eyeView.first[i]);
    }
    for (int i = 0; i < eyeView.second.size(); i++) {
      renderer->DrawHUDCircle(Vector2(0.75f + -pixelBarSize + i * pixelSize, -0.9f),
                              pixelSize / 2.0f, eyeView.second[i]);
    }
  }

  void Update(float seconds) { car->Update(seconds, track.get()); }

  Car *GetCar(void) { return car.get(); }
};

World::World(const sptr<Track> &track, const CarDef &carDef) : impl(new WorldImpl(track, carDef)) {}
World::~World() = default;

void World::Render(renderer::Renderer *renderer) const { impl->Render(renderer); }

void World::Update(float seconds) { impl->Update(seconds); }

Car *World::GetCar(void) { return impl->GetCar(); }
