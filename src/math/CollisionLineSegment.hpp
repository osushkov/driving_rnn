/*
 * CollisionLine.h
 *
 *  Created on: 11/12/2013
 *      Author: osushkov
 */

#pragma once

#include "CollisionResult.hpp"
#include "CollisionSphere.hpp"
#include "Vector2.hpp"

using namespace std;

class CollisionLineSegment {
public:
  Vector2 start;
  Vector2 end;
  CollisionSphere boundingSphere;

  CollisionLineSegment();
  CollisionLineSegment(const Vector2 &start, const Vector2 &end);

  bool CheckIntersectsSphere(const CollisionSphere &sphere) const;
  bool CheckIntersectsLineSegment(const CollisionLineSegment &line) const;

  CollisionResult IntersectSphere(const CollisionSphere &sphere) const;
  CollisionResult IntersectLineSegment(const CollisionLineSegment &line) const;

  void Set(const Vector2 &start, const Vector2 &end);
};
