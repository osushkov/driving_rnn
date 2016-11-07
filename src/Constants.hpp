#pragma once

#include <cmath>

static constexpr float CAR_SIZE = 0.5f;
static constexpr float CAR_EYE_SEPARATION = 0.45f;
static constexpr float CAR_TURN_RATE = 180.0f * static_cast<float>(M_PI) / 180.0f;
static constexpr float CAR_ACCEL_RATE = 3.0f;

static constexpr float CAR_VELOCITY_DECAY = 0.65f; // per second
static constexpr float CAR_VELOCITY_COLLISION_DECAY = 0.1f;

static constexpr float EYE_FOV = 120.0f * static_cast<float>(M_PI) / 180.0f;
static constexpr unsigned PIXELS_PER_EYE = 30;
static constexpr unsigned SAMPLER_PER_PIXEL = 5;

static constexpr float SONAR_FOV = 120.0f * static_cast<float>(M_PI) / 180.0f;
static constexpr unsigned SONAR_PIXELS = 10;
static constexpr unsigned SAMPLES_PER_SONAR_PIXEL = 3;

static constexpr float STEP_LENGTH_SECS = 1.0f / 20.0f;
static constexpr unsigned STEPS_PER_ACTION = 5;

static constexpr float TRACK_RADIUS = 15.0f;
static constexpr float TRACK_MIN_WIDTH = 1.5f;
static constexpr float TRACK_MAX_WIDTH = 4.0f;
static constexpr float TRACK_MAX_SKEW = 2.0f;
static constexpr unsigned TRACK_NUM_POINTS = 70;
static constexpr unsigned TRACK_COLOR_PALETTE = 20;
