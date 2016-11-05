
#include "common/Common.hpp"
#include "common/Timer.hpp"
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
  srand(time(NULL));

  std::cout << "hello world" << std::endl;

  uptr<SFMLRenderer> renderer = make_unique<SFMLRenderer>(800, 800, "Hello world");
  sptr<Track> track = make_shared<Track>(TrackSpec(20.0f, 1.5f, 4.0f, 100, 20, 5.0));
  uptr<World> world = make_unique<World>(track, CarDef(0.5f, 0.4, 3.0f, 5.0f));

  Timer timer;
  timer.Start();

  const float updateIntervalSeconds = 1.0f / 20.0f;
  float lastFrameTime = timer.GetElapsedSeconds();

  while (true) {
    float curTime = timer.GetElapsedSeconds();
    float gap = curTime - lastFrameTime;
    if (gap < updateIntervalSeconds) {
      timer.Sleep(updateIntervalSeconds - gap);
    }

    world->GetCar()->SetTurn(0.0f);
    world->GetCar()->SetAcceleration(0.0f);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
      world->GetCar()->SetTurn(-1.0f);
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
      world->GetCar()->SetTurn(1.0f);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
      world->GetCar()->SetAcceleration(1.0f);
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
      world->GetCar()->SetAcceleration(-1.0f);
    }

    lastFrameTime = timer.GetElapsedSeconds();
    world->Update(updateIntervalSeconds);

    renderer->Focus(world->GetCar()->GetPos(), 20.0f);
    world->Render(renderer.get());
    renderer->SwapBuffers();
  }

  return 0;
}
