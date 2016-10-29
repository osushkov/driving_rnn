
#include "Geometry.hpp"
#include <algorithm>
#include <cmath>
#include <utility>

float Geometry::AngleDiff(float angle0, float angle1) {
  return std::min(fabsf(angle0 - angle1),
                  fabsf(fabsf(angle0 - angle1) - 2.0f * static_cast<float>(M_PI)));
}

bool Geometry::CheckLinesIntersect(const Vector2 &start1, const Vector2 &end1,
                                   const Vector2 &start2, const Vector2 &end2) {

  float denominator =
      (end2.y - start2.y) * (end1.x - start1.x) - (end2.x - start2.x) * (end1.y - start1.y);

  // lines are parallel.
  if (fabsf(denominator) < EPSILON) {
    return false;
  }

  float numerator1 =
      (end2.x - start2.x) * (start1.y - start2.y) - (end2.y - start2.y) * (start1.x - start2.x);

  float numerator2 =
      (end1.x - start1.x) * (start1.y - start2.y) - (end1.y - start1.y) * (start1.x - start2.x);

  float u1 = numerator1 / denominator;
  float u2 = numerator2 / denominator;

  // intesect point falls outside the segment.
  if (u1 < -EPSILON || u1 > 1.0f + EPSILON || u2 < -EPSILON || u2 > 1.0f + EPSILON) {
    return false;
  }

  return true;
}

Maybe<Vector2> Geometry::IntersectLines(const Vector2 &start1, const Vector2 &end1,
                                        const Vector2 &start2, const Vector2 &end2) {

  float denominator =
      (end2.y - start2.y) * (end1.x - start1.x) - (end2.x - start2.x) * (end1.y - start1.y);

  // lines are parallel.
  if (fabsf(denominator) < EPSILON) {
    return Maybe<Vector2>::none;
  }

  float numerator1 =
      (end2.x - start2.x) * (start1.y - start2.y) - (end2.y - start2.y) * (start1.x - start2.x);

  float numerator2 =
      (end1.x - start1.x) * (start1.y - start2.y) - (end1.y - start1.y) * (start1.x - start2.x);

  float u1 = numerator1 / denominator;
  float u2 = numerator2 / denominator;

  // intesect point falls outside the segment.
  if (u1 < -EPSILON || u1 > 1.0f + EPSILON || u2 < -EPSILON || u2 > 1.0f + EPSILON) {
    return Maybe<Vector2>::none;
  }

  return Maybe<Vector2>(start1 + u1 * (end1 - start1));
}

std::pair<Vector2, float> Geometry::PointSegmentDist(const Vector2 &point, const Vector2 &start,
                                                     const Vector2 &end) {
  Vector2 v = end - start;
  Vector2 w = point - start;

  float c1 = w.dotProduct(v);
  if (c1 <= 0.0f) {
    return std::make_pair(start, w.length());
  }

  float c2 = v.dotProduct(v);
  if (c2 <= c1) {
    return std::make_pair(end, (point - end).length());
  }

  float b = c1 / c2;
  Vector2 Pb = start + (v * b);

  return std::make_pair(Pb, (point - Pb).length());
}
