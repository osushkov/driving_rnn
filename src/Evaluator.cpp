
#include "Evaluator.hpp"
#include "Constants.hpp"
#include "common/Common.hpp"
#include "common/Timer.hpp"
#include "simulation/Car.hpp"
#include "simulation/Track.hpp"
#include "simulation/World.hpp"

using namespace learning;
using namespace simulation;

static constexpr unsigned NUM_EPISODES = 10;
static constexpr unsigned EPISODE_LENGTH = 500;

static vector<sptr<Track>> generateTestTracks(unsigned num) {
  vector<sptr<Track>> result;
  for (unsigned i = 0; i < num; i++) {
    result.push_back(
        make_shared<Track>(TrackSpec(TRACK_RADIUS, TRACK_MIN_WIDTH, TRACK_MAX_WIDTH,
                                     TRACK_NUM_POINTS, TRACK_COLOR_PALETTE, TRACK_MAX_SKEW)));
  }
  return result;
}

float Evaluator::Evaluate(Agent *agent) {
  vector<sptr<Track>> tracks = generateTestTracks(NUM_EPISODES);

  float reward = 0.0f;
  for (const auto &track : tracks) {
    agent->ResetMemory();
    uptr<World> world = make_unique<World>(
        track, CarDef(CAR_SIZE, CAR_EYE_SEPARATION, CAR_TURN_RATE, CAR_ACCEL_RATE));

    for (unsigned j = 0; j < EPISODE_LENGTH; j++) {
      // pair<vector<ColorRGB>, vector<ColorRGB>> eyeView =
      //     world->GetCar()->EyeView(world->GetTrack());
      // State observedState(eyeView.first, eyeView.second);
      Vector2 nextWaypoint = world->GetTrack()->NextWaypoint(world->GetCar()->GetPos());
      Vector2 toNextWaypoint = (nextWaypoint - world->GetCar()->GetPos()).normalised();

      State observedState(world->GetCar()->SonarView(world->GetTrack()),
                          world->GetProgress(),
                          world->GetCar()->RelVelocity(),
                          world->GetCar()->RelHeading(toNextWaypoint));
      Action performedAction = agent->SelectAction(&observedState);

      world->GetCar()->SetAcceleration(performedAction.GetAcceleration());
      world->GetCar()->SetTurn(performedAction.GetTurn());

      for (unsigned k = 0; k < STEPS_PER_ACTION; k++) {
        reward += world->Update(STEP_LENGTH_SECS);
      }
    }
  }

  return reward / NUM_EPISODES;
}
