/*
 * CollisionResult.cpp
 *
 *  Created on: 10/12/2013
 *      Author: osushkov
 */

#include "CollisionResult.hpp"
#include "Vector2.hpp"

#include <ostream>

CollisionResult CollisionResult::None(void) { return CollisionResult(); }

CollisionResult CollisionResult::At(const Vector2 &position, const Vector2 &normal) {
  return CollisionResult(position, normal);
}

CollisionResult::CollisionResult() : haveCollision(false) {}

CollisionResult::CollisionResult(const Vector2 &position, const Vector2 &normal)
    : collisionPoint(position), collisionNormal(normal), haveCollision(true) {}

std::ostream &operator<<(std::ostream &stream, const CollisionResult &collisionResult) {
  stream << "CollisionResult(" << collisionResult.haveCollision << ", "
         << collisionResult.collisionPoint << ", " << collisionResult.collisionNormal << ")";
  return stream;
}
