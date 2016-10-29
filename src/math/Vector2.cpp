/*
 * Vector2.cpp
 *
 *  Created on: 05/12/2013
 *      Author: osushkov
 */

#include "Vector2.hpp"

#include <cassert>
#include <cmath>
#include <ostream>

Vector2::Vector2() : Vector2(0.0f, 0.0f) {}

Vector2::Vector2(float x, float y) : x(x), y(y) {}

Vector2::Vector2(const Vector2 &other) : x(other.x), y(other.y){};

void Vector2::set(float newX, float newY) {
  x = newX;
  y = newY;
}

void Vector2::normalise(void) {
  float invLength = 1.0f / length();
  assert(!isnan(invLength));

  x *= invLength;
  y *= invLength;
}

const Vector2 Vector2::normalised(void) const {
  Vector2 result(*this);
  result.normalise();
  return result;
}

float Vector2::dotProduct(const Vector2 &v) const { return (x * v.x) + (y * v.y); }

float Vector2::absThetaTo(const Vector2 &v) const {
  float dp = dotProduct(v);
  if (dp > 1.0f) {
    return 0.0f;
  }
  if (dp < -1.0f) {
    return static_cast<float>(M_PI);
  }

  return fabsf(acosf(dp));
}

float Vector2::length() const { return sqrtf((x * x) + (y * y)); }

float Vector2::length2() const { return (x * x) + (y * y); }

void Vector2::rotate(float theta) {
  float sinTheta = sinf(theta);
  float cosTheta = cosf(theta);

  float newX = (cosTheta * x) - (sinTheta * y);
  float newY = (sinTheta * x) + (cosTheta * y);

  x = newX;
  y = newY;
}

const Vector2 Vector2::rotated(float theta) const {
  Vector2 result(*this);
  result.rotate(theta);
  return result;
}

float Vector2::distanceTo(const Vector2 &v) const {
  return sqrtf((x - v.x) * (x - v.x) + (y - v.y) * (y - v.y));
}

float Vector2::distanceTo2(const Vector2 &v) const {
  return (x - v.x) * (x - v.x) + (y - v.y) * (y - v.y);
}

void Vector2::reflect(const Vector2 &normal) {
  *this += normal * (-2.0f * normal.dotProduct(*this));
}

const Vector2 Vector2::reflected(const Vector2 &normal) const {
  Vector2 result(*this);
  result.reflect(normal);
  return result;
}

float Vector2::orientation() const { return atan2f(y, x); }

const Vector2 Vector2::operator+(const Vector2 &v) const { return Vector2(x + v.x, y + v.y); }

const Vector2 Vector2::operator-(const Vector2 &v) const { return Vector2(x - v.x, y - v.y); }

const Vector2 Vector2::operator*(float s) const { return Vector2(x * s, y * s); }

const Vector2 Vector2::operator/(float s) const { return Vector2(x / s, y / s); }

const Vector2 &Vector2::operator+=(const Vector2 &v) {
  x += v.x;
  y += v.y;
  return *this;
}

const Vector2 &Vector2::operator-=(const Vector2 &v) {
  x -= v.x;
  y -= v.y;
  return *this;
}

const Vector2 &Vector2::operator*=(float s) {
  x *= s;
  y *= s;
  return *this;
}

const Vector2 &Vector2::operator/=(float s) {
  x /= s;
  y /= s;
  return *this;
}

std::ostream &operator<<(std::ostream &stream, const Vector2 &v) {
  stream << "(" << v.x << "," << v.y << ")";
  return stream;
}

const Vector2 operator*(float s, const Vector2 &v) { return Vector2(v.x * s, v.y * s); }
