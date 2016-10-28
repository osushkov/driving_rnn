
#pragma once

#include <cmath>
#include <ostream>
#include <vector>

namespace simulation {

class Action {
  float turn;
  float acceleration;

public:
  static unsigned NUM_ACTIONS(void);

  static Action ACTION(unsigned index);
  static unsigned ACTION_INDEX(const Action &ga);

  Action() = default; // useful to have a no args constructor
  explicit Action(float turn, float acceleration) : turn(turn), acceleration(acceleration) {}

  inline float GetTurn(void) const { return turn; }
  inline float GetAcceleration(void) const { return acceleration; }

  inline bool operator==(const Action &other) const {
    return fabsf(turn - other.turn) < 0.0001f && fabsf(acceleration - other.acceleration) < 0.0001f;
  }

  inline size_t HashCode(void) const {
    return static_cast<size_t>(turn * 378551 + acceleration * 1999);
  }
};
}

inline std::ostream &operator<<(std::ostream &stream, const simulation::Action &ga) {
  stream << "action turn/accel( " << ga.GetTurn() << " / " << ga.GetAcceleration() << " )";
  return stream;
}
