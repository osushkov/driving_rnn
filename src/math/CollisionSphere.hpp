/*
 * CollisionSphere.h
 *
 *  Created on: 08/12/2013
 *      Author: osushkov
 */

#pragma once

#include "CollisionResult.hpp"
#include "Vector2.hpp"

using namespace std;

class CollisionLineSegment;

class CollisionSphere {
public:
  Vector2 centre;
  float radius;

  CollisionSphere();
  CollisionSphere(const Vector2 &centre, float radius);

  bool CheckIntersectsSphere(const CollisionSphere &sphere) const;
  bool CheckIntersectsLineSegment(const CollisionLineSegment &line) const;

  CollisionResult IntersectSphere(const CollisionSphere &sphere) const;
  CollisionResult IntersectLineSegment(const CollisionLineSegment &line) const;

  void Set(const Vector2 &centre, float radius);
};
