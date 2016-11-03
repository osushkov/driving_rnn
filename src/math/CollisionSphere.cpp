/*
 * CollisionSphere.cpp
 *
 *  Created on: 10/12/2013
 *      Author: osushkov
 */

#include "CollisionSphere.hpp"
#include "CollisionLineSegment.hpp"
#include "Geometry.hpp"
#include <cassert>
#include <cmath>
#include <iostream>

using namespace std;

CollisionSphere::CollisionSphere() : CollisionSphere(Vector2(0.0f, 0.0f), 0.0f) {}

CollisionSphere::CollisionSphere(const Vector2 &centre, float radius)
    : centre(centre), radius(radius) {
  assert(radius >= 0.0f);
}

bool CollisionSphere::CheckIntersectsSphere(const CollisionSphere &otherSphere) const {
  return centre.distanceTo2(otherSphere.centre) <=
         (radius + otherSphere.radius) * (radius + otherSphere.radius);
}

bool CollisionSphere::CheckIntersectsLineSegment(const CollisionLineSegment &line) const {
  return Geometry::PointSegmentDist(centre, line.start, line.end).second < radius;
}

CollisionResult CollisionSphere::IntersectSphere(const CollisionSphere &otherSphere) const {
  float dist2 = centre.distanceTo2(otherSphere.centre);
  if (dist2 <= (radius + otherSphere.radius) * (radius + otherSphere.radius)) {
    Vector2 toTarget = (centre - otherSphere.centre).normalised();
    return CollisionResult::At(otherSphere.centre + (toTarget * otherSphere.radius), toTarget);
  } else {
    return CollisionResult::None();
  }
}

CollisionResult CollisionSphere::IntersectLineSegment(const CollisionLineSegment &line) const {
  std::pair<Vector2, float> nearestSegmentPoint =
      Geometry::PointSegmentDist(centre, line.start, line.end);

  if (nearestSegmentPoint.second < Geometry::EPSILON) {
    Vector2 lineRay = (line.end - line.start).normalised();
    return CollisionResult::At(nearestSegmentPoint.first, lineRay.rotated(static_cast<float>(M_PI) / 2.0f));
  } else if (nearestSegmentPoint.second < radius) {
    Vector2 collisionNormal = (centre - nearestSegmentPoint.first).normalised();
    assert(nearestSegmentPoint.first.distanceTo(centre) <= radius + Geometry::EPSILON);
    return CollisionResult::At(nearestSegmentPoint.first, collisionNormal);
  } else {
    return CollisionResult::None();
  }
}

void CollisionSphere::Set(const Vector2 &centre, float radius) {
  this->centre = centre;
  this->radius = radius;
}
