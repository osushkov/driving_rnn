
#include "LearningAgent.hpp"
#include "../common/Common.hpp"
#include "../rnn/RNN.hpp"
#include "../rnn/RNNSpec.hpp"
#include "Constants.hpp"

#include <boost/thread/shared_mutex.hpp>
#include <cassert>
#include <random>

using namespace learning;

struct LearningAgent::LearningAgentImpl {
  mutable boost::shared_mutex rwMutex;

  float pRandom;
  float temperature;

  uptr<rnn::RNN> network;
  unsigned itersSinceTargetUpdated = 0;

  LearningAgentImpl(unsigned inputDim) : pRandom(0.1f), temperature(0.1f) {
    createNetwork(inputDim);
    itersSinceTargetUpdated = 0;
  }

  void createNetwork(unsigned inputDim) {
    rnn::RNNSpec spec;

    spec.numInputs = inputDim;
    spec.numOutputs = Action::NUM_ACTIONS();
    spec.hiddenActivation = rnn::LayerActivation::TANH;
    spec.outputActivation = rnn::LayerActivation::LINEAR;
    spec.nodeActivationRate = 1.0f;
    spec.maxBatchSize = EXPERIENCE_BATCH_SIZE;
    spec.maxTraceLength = EXPERIENCE_MAX_TRACE_LENGTH;

    // Forward connections
    spec.connections.emplace_back(0, 1, 0);
    spec.connections.emplace_back(1, 2, 0);
    spec.connections.emplace_back(2, 3, 0);

    // Recurrent connections
    spec.connections.emplace_back(2, 2, 1);
    // spec.connections.emplace_back(1, 1, 1);

    // Layer defs
    spec.layers.emplace_back(1, 64, false);
    spec.layers.emplace_back(2, 128, false);
    spec.layers.emplace_back(3, spec.numOutputs, true);

    network = make_unique<rnn::RNN>(spec);
  }

  Action SelectAction(const State *state) {
    assert(state != nullptr);

    boost::shared_lock<boost::shared_mutex> lock(rwMutex);
    return chooseBestAction(state, false);
  }

  void ResetMemory(void) {
    boost::unique_lock<boost::shared_mutex> lock(rwMutex);
    network->ClearMemory();
  }

  void SetPRandom(float pRandom) {
    assert(pRandom >= 0.0f && pRandom <= 1.0f);
    this->pRandom = pRandom;
  }

  void SetTemperature(float temperature) {
    assert(temperature > 0.0f);
    this->temperature = temperature;
  }

  Action SelectLearningAction(const State *state) {
    assert(state != nullptr);

    boost::shared_lock<boost::shared_mutex> lock(rwMutex);
    if (math::RandInterval(0.0, 1.0) < pRandom) {
      return chooseExplorativeAction(state);
    } else {
      return chooseWeightedAction(state);
      // return chooseBestAction(state, false);
    }
  }

  void Learn(const vector<Experience> &experiences, float learnRate) {
    rnn::RNNSpec rnnSpec = network->GetSpec();
    assert(experiences.size() <= rnnSpec.maxBatchSize);

    if (experiences.empty() || experiences.front().moments.empty()) {
      return;
    }

    assert(experiences.front().moments.size() <= network->GetSpec().maxTraceLength);

    if (itersSinceTargetUpdated > TARGET_FUNCTION_UPDATE_RATE) {
      Finalise();
      itersSinceTargetUpdated = 0;
    }
    itersSinceTargetUpdated++;

    vector<rnn::SliceBatch> trainInput;
    trainInput.reserve(experiences.size());

    // TODO: this could probably be just a "static" slice batch vector that is kept and reset
    // between Learn calls.
    for (unsigned i = 0; i < experiences.front().moments.size(); i++) {
      trainInput.emplace_back(EMatrix(experiences.size(), rnnSpec.numInputs),
                              EMatrix(experiences.size(), rnnSpec.numOutputs),
                              EMatrix(experiences.size(), 1));

      trainInput.back().batchInput.fill(0.0f);
      trainInput.back().batchActions.fill(0.0f);
      trainInput.back().batchRewards.fill(0.0f);
    }

    for (unsigned i = 0; i < experiences.size(); i++) {
      assert(experiences[i].moments.size() == trainInput.size());

      for (unsigned j = 0; j < experiences[i].moments.size(); j++) {
        unsigned actionIndex = Action::ACTION_INDEX(experiences[i].moments[j].actionTaken);

        trainInput[j].batchInput.row(i) = experiences[i].moments[j].observedState.transpose();
        trainInput[j].batchActions(i, actionIndex) = 1.0f;
        trainInput[j].batchRewards(i, 0) = experiences[i].moments[j].reward;
      }
    }

    network->Update(trainInput, learnRate);
  }

  void Finalise(void) {
    // obtain a write lock
    boost::unique_lock<boost::shared_mutex> lock(rwMutex);
    network->RefreshAndGetTarget();
  }

  Action chooseBestAction(const State *state, bool print) {
    EVector qvalues = network->Process(state->Encode());
    assert(qvalues.rows() == static_cast<int>(Action::NUM_ACTIONS()));

    std::vector<unsigned> availableActions = state->AvailableActions();
    assert(availableActions.size() > 0);

    unsigned bestActionIndex = availableActions[0];
    float bestQValue = qvalues(availableActions[0]);

    if (print) {
      cout << *state << endl;
      cout << "q(0) = " << qvalues(availableActions[0]) << endl;
    }
    for (unsigned i = 1; i < availableActions.size(); i++) {
      if (print) {
        cout << "q(" << i << ") = " << qvalues(availableActions[i]) << endl;
      }
      if (qvalues(availableActions[i]) > bestQValue) {
        bestQValue = qvalues(availableActions[i]);
        bestActionIndex = availableActions[i];
      }
    }
    if (print) {
      getchar();
    }
    return Action::ACTION(bestActionIndex);
  }

  Action chooseExplorativeAction(const State *state) {
    auto aa = state->AvailableActions();
    return Action::ACTION(aa[rand() % aa.size()]);
  }

  Action chooseWeightedAction(const State *state) {
    EVector qvalues = network->Process(state->Encode());
    assert(qvalues.rows() == static_cast<int>(Action::NUM_ACTIONS()));

    std::vector<unsigned> availableActions = state->AvailableActions();

    qvalues *= 1.0f / temperature;
    qvalues = math::SoftmaxActivations(qvalues);

    float sample = math::RandInterval(0.0, 1.0);
    for (unsigned i = 0; i < qvalues.rows(); i++) {
      sample -= qvalues(i);
      if (sample <= 0.0f) {
        return Action::ACTION(availableActions[i]);
      }
    }

    return chooseExplorativeAction(state);
  }
};

LearningAgent::LearningAgent(unsigned inputDim) : impl(new LearningAgentImpl(inputDim)) {}
LearningAgent::~LearningAgent() = default;

Action LearningAgent::SelectAction(const State *state) { return impl->SelectAction(state); }
void LearningAgent::ResetMemory(void) { impl->ResetMemory(); }

void LearningAgent::SetPRandom(float pRandom) { impl->SetPRandom(pRandom); }
void LearningAgent::SetTemperature(float temperature) { impl->SetTemperature(temperature); }

Action LearningAgent::SelectLearningAction(const State *state) {
  return impl->SelectLearningAction(state);
}

void LearningAgent::Learn(const vector<Experience> &experiences, float learnRate) {
  impl->Learn(experiences, learnRate);
}

void LearningAgent::Finalise(void) { impl->Finalise(); }
