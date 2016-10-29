/*
 *
 *  Created on: 04/12/2013
 *      Author: osushkov
 */

#pragma once

#include <iosfwd>

class Vector2 final {
public:
  float x;
  float y;

  Vector2();
  Vector2(float x, float y);
  Vector2(const Vector2 &other);

  void set(float newX, float newY);

  void normalise(void);
  const Vector2 normalised(void) const;

  float dotProduct(const Vector2 &v) const;
  float absThetaTo(const Vector2 &v) const;

  float length() const;
  float length2() const;

  void rotate(float theta);
  const Vector2 rotated(float theta) const;

  float distanceTo(const Vector2 &v) const;
  float distanceTo2(const Vector2 &v) const;

  void reflect(const Vector2 &normal);
  const Vector2 reflected(const Vector2 &normal) const;

  float orientation() const;

  const Vector2 operator+(const Vector2 &v) const;
  const Vector2 operator-(const Vector2 &v) const;
  const Vector2 operator*(float s) const;
  const Vector2 operator/(float s) const;

  const Vector2 &operator+=(const Vector2 &v);
  const Vector2 &operator-=(const Vector2 &v);
  const Vector2 &operator*=(float s);
  const Vector2 &operator/=(float s);
};

std::ostream &operator<<(std::ostream &stream, const Vector2 &v);
const Vector2 operator*(float s, const Vector2 &v);
