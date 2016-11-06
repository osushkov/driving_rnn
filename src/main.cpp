
#include "Constants.hpp"
#include "Evaluator.hpp"
#include "common/Common.hpp"
#include "common/Timer.hpp"
#include "learning/LearningAgent.hpp"
#include "learning/RandomAgent.hpp"
#include "learning/Trainer.hpp"
#include "renderer/SFMLRenderer.hpp"
#include "simulation/Car.hpp"
#include "simulation/Track.hpp"
#include "simulation/World.hpp"
#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
// #include <SFML/Keyboard.hpp>

using namespace renderer;
using namespace simulation;

int main(int argc, char **argv) {
  // srand(time(NULL));

  std::cout << "hello world" << std::endl;

  // uptr<learning::Agent> randomAgent = make_unique<learning::RandomAgent>();
  // cout << "random agent: " << Evaluator::Evaluate(randomAgent.get()) << endl;

  uptr<learning::LearningAgent> learningAgent =
      make_unique<learning::LearningAgent>(SONAR_PIXELS + 1); // PIXELS_PER_EYE * 2 * 3);
  cout << "learning agent start: " << Evaluator::Evaluate(learningAgent.get()) << endl;

  learning::Trainer trainer;
  trainer.TrainAgent(learningAgent.get(), 20000);

  cout << "learning agent end: " << Evaluator::Evaluate(learningAgent.get()) << endl;
  getchar();

  uptr<SFMLRenderer> renderer = make_unique<SFMLRenderer>(800, 800, "Hello world");
  sptr<Track> track =
      make_shared<Track>(TrackSpec(TRACK_RADIUS, TRACK_MIN_WIDTH, TRACK_MAX_WIDTH, TRACK_NUM_POINTS,
                                   TRACK_COLOR_PALETTE, TRACK_MAX_SKEW));
  uptr<World> world = make_unique<World>(
      track, CarDef(CAR_SIZE, CAR_EYE_SEPARATION, CAR_TURN_RATE, CAR_ACCEL_RATE));

  Timer timer;
  timer.Start();

  float lastFrameTime = timer.GetElapsedSeconds();

  learning::LearningAgent *agent = learningAgent.get();
  while (true) {
    // pair<vector<ColorRGB>, vector<ColorRGB>> eyeView =
    // world->GetCar()->EyeView(world->GetTrack());
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

    for (unsigned j = 0; j < STEPS_PER_ACTION; j++) {
      float curTime = timer.GetElapsedSeconds();
      float gap = curTime - lastFrameTime;
      if (gap < STEP_LENGTH_SECS) {
        timer.Sleep(STEP_LENGTH_SECS - gap);
      }

      // if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
      //   world->GetCar()->SetTurn(-1.0f);
      // } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
      //   world->GetCar()->SetTurn(1.0f);
      // }
      //
      // if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
      //   world->GetCar()->SetAcceleration(1.0f);
      // } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
      //   world->GetCar()->SetAcceleration(-1.0f);
      // }

      lastFrameTime = timer.GetElapsedSeconds();
      double r = world->Update(STEP_LENGTH_SECS);
      cout << "reward: " << r << endl;

      renderer->Focus(world->GetCar()->GetPos(), 20.0f);
      world->Render(renderer.get());
      renderer->SwapBuffers();
    }
  }

  return 0;
}
