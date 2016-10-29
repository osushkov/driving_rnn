
#include "common/Common.hpp"
#include "renderer/SFMLRenderer.hpp"
#include "simulation/Track.hpp"
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>

using namespace renderer;
using namespace simulation;

int main(int argc, char **argv) {
  srand(time(NULL));

  std::cout << "hello world" << std::endl;

  uptr<SFMLRenderer> renderer = make_unique<SFMLRenderer>(640, 640, "Hello world");
  uptr<Track> track = make_unique<Track>(TrackSpec(20.0f, 4.0f, 50, 10, 5.0f));

  while (true) {
    track->Render(renderer.get());
    renderer->SwapBuffers();
  }

  return 0;
}
