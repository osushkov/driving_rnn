
#pragma once

#include "../common/ColorRGB.hpp"
#include "../common/Common.hpp"
#include "../math/Math.hpp"
#include "../math/Vector2.hpp"
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
  Vector2 relVelocity;
  double forwardAngle;

  State();
  // State(const vector<ColorRGB> &leftEye, const vector<ColorRGB> &rightEye);
  State(const vector<double> &sonar, float curProgress, const Vector2 &relVelocity,
        double forwardAngle);

  bool operator==(const State &other) const;

  // Returns indices into the GameAction::ALL_ACTIONS vector.
  vector<unsigned> AvailableActions(void) const;

  EVector Encode(void) const;
};
}

std::ostream &operator<<(std::ostream &stream, const simulation::State &gs);
