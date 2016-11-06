#pragma once

#include "../common/Common.hpp"
#include "../simulation/Action.hpp"
#include "../simulation/State.hpp"
#include "Agent.hpp"
#include "Experience.hpp"

#include <iostream>

using namespace simulation;

namespace learning {

class LearningAgent : public Agent {
public:
  LearningAgent(unsigned inputDim);
  virtual ~LearningAgent();

  Action SelectAction(const State *state) override;
  void ResetMemory(void) override;

  void SetPRandom(float pRandom);
  void SetTemperature(float temperature);

  Action SelectLearningAction(const State *state);
  void Learn(const vector<Experience> &experiences, float learnRate);

  void Finalise(void);

private:
  struct LearningAgentImpl;
  uptr<LearningAgentImpl> impl;
};
}
