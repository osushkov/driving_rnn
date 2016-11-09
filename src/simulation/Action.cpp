
#include "Action.hpp"
#include <cassert>
#include <mutex>

using namespace simulation;

static std::once_flag stateFlag;
static std::vector<Action> actionSet;

static void initialiseActions(void) {
  actionSet.emplace_back(0.0f, 0.0f);  // do nothing action
  actionSet.emplace_back(0.0f, 1.0f);  // full ahead
  actionSet.emplace_back(0.0f, -0.5f); // full reverse

  actionSet.emplace_back(1.0f, 0.0f); // full turn left
  actionSet.emplace_back(1.0f, 1.0f);
  actionSet.emplace_back(1.0f, -0.5f);

  actionSet.emplace_back(-1.0f, 0.0f); // full turn right
  actionSet.emplace_back(-1.0f, 1.0f);
  actionSet.emplace_back(-1.0f, -0.5f);
}

unsigned Action::NUM_ACTIONS(void) {
  std::call_once(stateFlag, []() { initialiseActions(); });
  return actionSet.size();
}

Action Action::ACTION(unsigned index) {
  std::call_once(stateFlag, []() { initialiseActions(); });
  return actionSet[index];
}

unsigned Action::ACTION_INDEX(const Action &ga) {
  std::call_once(stateFlag, []() { initialiseActions(); });

  for (unsigned i = 0; i < actionSet.size(); i++) {
    if (ga == actionSet[i]) {
      return i;
    }
  }

  assert(false);
  return 0;
}
