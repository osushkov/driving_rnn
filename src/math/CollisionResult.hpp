/*
 * CollisionResult.hpp
 *
 *  Created on: 07/12/2013
 *      Author: osushkov
 */

#pragma once

#include "Vector2.hpp"
#include <iosfwd>

class CollisionResult {
public:
  Vector2 collisionPoint;
  Vector2 collisionNormal;
  bool haveCollision;

  /**
   * Static factory method for a CollisionResult signifying no collision.
   */
  static CollisionResult None(void);

  /**
   * Static factory method for a CollisionResult signifying a collision at the given point with
   * the given normal vector.
   */
  static CollisionResult At(const Vector2 &position, const Vector2 &normal);

  CollisionResult();
  CollisionResult(const Vector2 &position, const Vector2 &normal);
};

std::ostream &operator<<(std::ostream &stream, const CollisionResult &collisionResult);
