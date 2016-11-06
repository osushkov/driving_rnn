
#include "State.hpp"
#include "Action.hpp"
#include <cassert>
#include <cmath>
#include <ostream>

using namespace simulation;

State::State() {}

// State::State(const vector<ColorRGB> &leftEye, const vector<ColorRGB> &rightEye)
//     : leftEye(leftEye), rightEye(rightEye) {
//   assert(leftEye.size() == rightEye.size());
// }

State::State(const vector<double> &sonar, float curProgress, const Vector2 &relVelocity,
             double forwardAngle)
    : sonar(sonar), curProgress(curProgress), relVelocity(relVelocity), forwardAngle(forwardAngle) {}

bool State::operator==(const State &other) const {
  // if (leftEye.size() != other.leftEye.size()) {
  //   return false;
  // }
  // if (rightEye.size() != other.rightEye.size()) {
  //   return false;
  // }
  //
  // for (unsigned i = 0; i < leftEye.size(); i++) {
  //   if (fabsf(leftEye[i].r - other.leftEye[i].r) > 0.0001f ||
  //       fabsf(leftEye[i].g - other.leftEye[i].g) > 0.0001f ||
  //       fabsf(leftEye[i].b - other.leftEye[i].b) > 0.0001f) {
  //     return false;
  //   }
  // }
  //
  // for (unsigned i = 0; i < rightEye.size(); i++) {
  //   if (fabsf(rightEye[i].r - other.rightEye[i].r) > 0.0001f ||
  //       fabsf(rightEye[i].g - other.rightEye[i].g) > 0.0001f ||
  //       fabsf(rightEye[i].b - other.rightEye[i].b) > 0.0001f) {
  //     return false;
  //   }
  // }

  if (fabsf(curProgress - other.curProgress) > 0.0001f || sonar.size() != other.sonar.size() ||
      relVelocity.distnaceTo(other.relVelocity) > 0.0001f ||
      fabsf(forwardAngle - other.forwardAngle) > 0.0001f) {
    return false;
  }

  for (unsigned i = 0; i < sonar.size(); i++) {
    if (fabsf(sonar[i] - other.sonar[i]) > 0.0001f) {
      return false;
    }
  }

  return true;
}

vector<unsigned> State::AvailableActions(void) const {
  vector<unsigned> result(Action::NUM_ACTIONS());
  for (unsigned i = 0; i < result.size(); i++) {
    result[i] = i;
  }
  return result;
}

EVector State::Encode(void) const {
  // EVector result(leftEye.size() * 3 + rightEye.size() * 3);
  //
  // unsigned vi = 0;
  // for (const auto &c : leftEye) {
  //   result(vi++) = c.r;
  //   result(vi++) = c.g;
  //   result(vi++) = c.b;
  // }
  //
  // for (const auto &c : rightEye) {
  //   result(vi++) = c.r;
  //   result(vi++) = c.g;
  //   result(vi++) = c.b;
  // }
  //
  // assert(vi == result.rows());
  EVector result(sonar.size() + 4);
  result(0) = curProgress;
  result(1) = relVelocity.x;
  result(2) = relVelocity.y;
  result(3) = forwardAngle;
  for (unsigned i = 0; i < sonar.size(); i++) {
    result(i + 4) = sonar[i];
  }
  return result;
}

std::ostream &operator<<(std::ostream &stream, const simulation::State &gs) {
  stream << gs.curProgress << endl;
  for (const auto &s : gs.sonar) {
    stream << s << " ";
  }
  stream << endl;
  return stream;
}
