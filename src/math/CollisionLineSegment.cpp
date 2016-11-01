/*
 * CollisionLineSegment.cpp
 *
 *  Created on: 11/12/2013
 *      Author: osushkov
 */

#include "CollisionLineSegment.hpp"
#include "../common/Maybe.hpp"
#include "CollisionSphere.hpp"
#include "Geometry.hpp"
#include "Vector2.hpp"
#include <cassert>
#include <cmath>

CollisionLineSegment::CollisionLineSegment()
    : CollisionLineSegment(Vector2(0.0f, 0.0f), Vector2(0.0f, 0.0f)) {}

CollisionLineSegment::CollisionLineSegment(const Vector2 &start, const Vector2 &end)
    : start(start), end(end), midPoint((start + end) * 0.5f), length(start.distanceTo(end)) {}

bool CollisionLineSegment::CheckIntersectsSphere(const CollisionSphere &sphere) const {
  return Geometry::PointSegmentDist(sphere.centre, start, end).second < sphere.radius;
}

bool CollisionLineSegment::CheckIntersectsLineSegment(const CollisionLineSegment &otherLine) const {
  return Geometry::CheckLinesIntersect(start, end, otherLine.start, otherLine.end);
}

CollisionResult CollisionLineSegment::IntersectSphere(const CollisionSphere &sphere) const {
  std::pair<Vector2, float> nearestSegmentPoint =
      Geometry::PointSegmentDist(sphere.centre, start, end);

  if (nearestSegmentPoint.second < sphere.radius) {
    Vector2 collisionNormal = (sphere.centre - nearestSegmentPoint.first).normalised();
    return CollisionResult::At(nearestSegmentPoint.first, collisionNormal);
  } else {
    return CollisionResult::None();
  }
}

CollisionResult
CollisionLineSegment::IntersectLineSegment(const CollisionLineSegment &otherLine) const {
  Maybe<Vector2> linesIntersect =
      Geometry::IntersectLines(start, end, otherLine.start, otherLine.end);

  if (!linesIntersect.valid()) {
    return CollisionResult::None();
  } else {
    Vector2 collisionNormal = (start - end).normalised();
    collisionNormal.rotate(static_cast<float>(M_PI) / 2.0f);

    return CollisionResult::At(linesIntersect.val(), collisionNormal);
  }
}

void CollisionLineSegment::Set(const Vector2 &start, const Vector2 &end) {
  this->start = start;
  this->end = end;
  this->midPoint = (start + end) * 0.5f;
  this->length = start.distanceTo(end);
}
