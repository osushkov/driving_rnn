
#pragma once

#include "../common/ColorRGB.hpp"
#include "../common/Common.hpp"
#include "../math/Math.hpp"
#include "Action.hpp"
#include <array>
#include <iosfwd>
#include <vector>

namespace simulation {

class State {
  // vector<ColorRGB> leftEye;
  // vector<ColorRGB> rightEye;

public:
  vector<double> sonar;
  float curProgress;

  State();
  // State(const vector<ColorRGB> &leftEye, const vector<ColorRGB> &rightEye);
  State(const vector<double> &sonar, float curProgress);

  bool operator==(const State &other) const;

  // Returns indices into the GameAction::ALL_ACTIONS vector.
  vector<unsigned> AvailableActions(void) const;

  EVector Encode(void) const;
};
}

std::ostream &operator<<(std::ostream &stream, const simulation::State &gs);
