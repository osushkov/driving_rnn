
#include "State.hpp"
#include "Action.hpp"
#include <cassert>
#include <cmath>
#include <ostream>

using namespace simulation;

State::State() {}

bool State::operator==(const State &other) const { return true; }

vector<unsigned> State::AvailableActions(void) const {
  vector<unsigned> result(Action::NUM_ACTIONS());
  for (unsigned i = 0; i < result.size(); i++) {
    result[i] = i;
  }
  return result;
}

EVector State::Encode(void) const {
  EVector result(1);
  return result;
}

std::ostream &operator<<(std::ostream &stream, const simulation::State &gs) { return stream; }
