
#include "ExperienceGenerator.hpp"
#include "../simulation/Action.hpp"
#include "Constants.hpp"
#include <vector>

using namespace learning;
using namespace simulation;

struct ExperienceGenerator::ExperienceGeneratorImpl {
  ExperienceGeneratorImpl() {}

  Experience GenerateExperience(LearningAgent *agent) {
    assert(agent != nullptr);

    Experience result;
    return result;
  }
};

ExperienceGenerator::ExperienceGenerator() : impl(new ExperienceGeneratorImpl()) {}

ExperienceGenerator::~ExperienceGenerator() = default;

Experience ExperienceGenerator::GenerateExperience(LearningAgent *agent) {
  return impl->GenerateExperience(agent);
}
