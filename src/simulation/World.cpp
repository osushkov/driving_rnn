#include "World.hpp"

using namespace simulation;

struct World::WorldImpl {
  sptr<Track> track;
  uptr<Car> car;

  float prevProgress = 0.0f;
  float curProgress = 0.0f;

  WorldImpl(const sptr<Track> &track, const CarDef &carDef) : track(track) {
    auto startState = track->StartPosAndOrientation();
    car = make_unique<Car>(carDef, startState.first, startState.second);

    curProgress = track->DistanceAlongTrack(car->GetPos());
    prevProgress = prevProgress;
  }

  void Render(renderer::Renderer *renderer) const {
    track->Render(renderer);
    car->Render(renderer);

    // Render the pixels seen by each eye in the HUD.
    // pair<vector<ColorRGB>, vector<ColorRGB>> eyeView = car->EyeView(track.get());
    // float pixelBarSize = 0.6f;
    // float pixelSize = pixelBarSize / static_cast<float>(eyeView.first.size());
    // for (unsigned i = 0; i < eyeView.first.size(); i++) {
    //   renderer->DrawHUDCircle(Vector2(-0.75f + i * pixelSize, -0.9f), pixelSize / 2.0f,
    //                           eyeView.first[i]);
    // }
    // for (unsigned i = 0; i < eyeView.second.size(); i++) {
    //   renderer->DrawHUDCircle(Vector2(0.75f + -pixelBarSize + i * pixelSize, -0.9f),
    //                           pixelSize / 2.0f, eyeView.second[i]);
    // }
  }

  float Update(float seconds) {
    bool collision = car->Update(seconds, track.get());
    float collisionPenalty = 0.0f; // collision ? -0.1f : 0.0f;

    prevProgress = curProgress;
    curProgress = track->DistanceAlongTrack(car->GetPos());

    float progressScale = car->MaxSpeed() * seconds * 2.0f;

    if (fabsf(curProgress - prevProgress) > track->TrackLength() / 2.0f) {
      if (curProgress < prevProgress) {
        return (curProgress - prevProgress + track->TrackLength()) * progressScale +
               collisionPenalty;
      } else {
        return (curProgress - prevProgress - track->TrackLength()) * progressScale +
               collisionPenalty;
      }
    } else {
      return (curProgress - prevProgress) * progressScale + collisionPenalty;
    }
  }
};

World::World(const sptr<Track> &track, const CarDef &carDef) : impl(new WorldImpl(track, carDef)) {}
World::~World() = default;

void World::Render(renderer::Renderer *renderer) const { impl->Render(renderer); }

float World::Update(float seconds) { return impl->Update(seconds); }

float World::GetProgress(void) { return impl->curProgress / impl->track->TrackLength(); }

Car *World::GetCar(void) { return impl->car.get(); }
Track *World::GetTrack(void) { return impl->track.get(); }
