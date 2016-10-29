#pragma once

#include "../common/Maybe.hpp"
#include "Vector2.hpp"

#include <cmath>
#include <cstdlib>
#include <utility>

namespace Geometry {

constexpr float EPSILON = 0.00001f;

float AngleDiff(float angle0, float angle1);

bool CheckLinesIntersect(const Vector2 &start1, const Vector2 &end1, const Vector2 &start2,
                         const Vector2 &end2);

Maybe<Vector2> IntersectLines(const Vector2 &start1, const Vector2 &end1, const Vector2 &start2,
                              const Vector2 &end2);

std::pair<Vector2, float> PointSegmentDist(const Vector2 &point, const Vector2 &start,
                                           const Vector2 &end);
}
