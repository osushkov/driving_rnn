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
    Vector2 toTarget = (otherSphere.centre - centre).normalised();
    return CollisionResult::At((toTarget * radius) + centre, toTarget);
  } else {
    return CollisionResult::None();
  }
}

CollisionResult CollisionSphere::IntersectLineSegment(const CollisionLineSegment &line) const {
  std::pair<Vector2, float> nearestSegmentPoint =
      Geometry::PointSegmentDist(centre, line.start, line.end);

  if (nearestSegmentPoint.second < radius) {
    float p = sqrtf(radius * radius - nearestSegmentPoint.second * nearestSegmentPoint.second);
    Vector2 dir = (line.start - nearestSegmentPoint.first).normalised();

    Vector2 collisionPoint = nearestSegmentPoint.first + (dir * p);
    Vector2 collisionNormal = collisionPoint - centre;

    if (collisionNormal.length2() < Geometry::EPSILON) {
      collisionNormal = Vector2(1.0f, 0.0f);
    } else {
      collisionNormal.normalise();
    }

    return CollisionResult::At(collisionPoint, collisionNormal);
  } else {
    return CollisionResult::None();
  }
}

void CollisionSphere::Set(const Vector2 &centre, float radius) {
  this->centre = centre;
  this->radius = radius;
}
