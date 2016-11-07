
#include "ExperienceGenerator.hpp"
#include "../Constants.hpp"
#include "../simulation/Action.hpp"
#include "../simulation/Car.hpp"
#include "../simulation/Track.hpp"
#include "../simulation/World.hpp"
#include "Constants.hpp"
#include <vector>

using namespace learning;
using namespace simulation;

static constexpr unsigned NUM_TRACKS = 10;
static constexpr unsigned MAX_TRACE_LENGTH = 50;

struct ExperienceGenerator::ExperienceGeneratorImpl {
  vector<sptr<Track>> tracks;

  ExperienceGeneratorImpl() {
    for (unsigned i = 0; i < NUM_TRACKS; i++) {
      tracks.push_back(
          make_shared<Track>(TrackSpec(TRACK_RADIUS, TRACK_MIN_WIDTH, TRACK_MAX_WIDTH,
                                       TRACK_NUM_POINTS, TRACK_COLOR_PALETTE, TRACK_MAX_SKEW)));
    }
  }

  Experience GenerateExperience(LearningAgent *agent) {
    assert(agent != nullptr);
    agent->ResetMemory();

    Experience result;
    result.moments.reserve(MAX_TRACE_LENGTH);

    const auto &track = tracks[rand() % tracks.size()];
    uptr<World> world = make_unique<World>(
        track, CarDef(CAR_SIZE, CAR_EYE_SEPARATION, CAR_TURN_RATE, CAR_ACCEL_RATE));

    for (unsigned i = 0; i < MAX_TRACE_LENGTH; i++) {
      // pair<vector<ColorRGB>, vector<ColorRGB>> eyeView =
      //     world->GetCar()->EyeView(world->GetTrack());
      // State observedState(eyeView.first, eyeView.second);

      Vector2 nextWaypoint = world->GetTrack()->NextWaypoint(world->GetCar()->GetPos());
      Vector2 toNextWaypoint = (nextWaypoint - world->GetCar()->GetPos()).normalised();

      State observedState(world->GetCar()->SonarView(world->GetTrack()),
                          world->GetProgress(),
                          world->GetCar()->RelVelocity(),
                          world->GetCar()->RelHeading(toNextWaypoint));

      // if (i == 0) {
        // cout << observedState << endl;

      // }
      Action performedAction = agent->SelectLearningAction(&observedState);
      // cout << performedAction << endl;
      // getchar();
      world->GetCar()->SetAcceleration(performedAction.GetAcceleration());
      world->GetCar()->SetTurn(performedAction.GetTurn());

      float reward = 0.0f;
      for (unsigned j = 0; j < STEPS_PER_ACTION; j++) {
        reward += world->Update(STEP_LENGTH_SECS);
      }

      auto encodedState = observedState.Encode();
      // cout << "state: " << encodedState << endl;
      // cout << "action: " << performedAction << " " << Action::ACTION_INDEX(performedAction) <<
      // endl;
      // cout << "reward: " << reward << endl;
      // getchar();

      result.moments.emplace_back(encodedState, performedAction, reward);
    }

    return result;
  }
};

ExperienceGenerator::ExperienceGenerator() : impl(new ExperienceGeneratorImpl()) {}

ExperienceGenerator::~ExperienceGenerator() = default;

Experience ExperienceGenerator::GenerateExperience(LearningAgent *agent) {
  return impl->GenerateExperience(agent);
}
