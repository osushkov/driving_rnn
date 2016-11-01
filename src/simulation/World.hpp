#pragma once

#include "../common/Common.hpp"

namespace simulation {

class World {
public:
  World();

private:
  struct WorldImpl;
  uptr<WorldImpl> impl;
};
}
