
#include "ColorRGB.hpp"
#include <cassert>
#include <ostream>

ColorRGB ColorRGB::White(void) { return ColorRGB(1.0f, 1.0f, 1.0f); }
ColorRGB ColorRGB::Red(void) { return ColorRGB(1.0f, 0.0f, 0.0f); }
ColorRGB ColorRGB::Green(void) { return ColorRGB(0.0f, 1.0f, 0.0f); }
ColorRGB ColorRGB::Blue(void) { return ColorRGB(0.0f, 0.0f, 1.0f); }

ColorRGB::ColorRGB() : r(0.0f), g(0.0f), b(0.0f) {}

ColorRGB::ColorRGB(float r, float g, float b) : r(r), g(g), b(b) {
  assert(r >= 0.0f && r <= 1.0f);
  assert(g >= 0.0f && g <= 1.0f);
  assert(b >= 0.0f && b <= 1.0f);
}

ColorRGB::ColorRGB(const ColorRGB &other) : r(other.r), g(other.g), b(other.b) {}

void ColorRGB::Set(float r, float g, float b) {
  assert(r >= 0.0f && r <= 1.0f);
  assert(g >= 0.0f && g <= 1.0f);
  assert(b >= 0.0f && b <= 1.0f);

  this->r = r;
  this->g = g;
  this->b = b;
}

const ColorRGB ColorRGB::operator+(const ColorRGB &other) const {
  return ColorRGB(r + other.r, g + other.g, b + other.b);
}

const ColorRGB ColorRGB::operator-(const ColorRGB &other) const {
  return ColorRGB(r - other.r, g - other.g, b - other.b);
}

const ColorRGB ColorRGB::operator*(float s) const { return ColorRGB(r * s, g * s, b * s); }
const ColorRGB ColorRGB::operator/(float s) const { return ColorRGB(r / s, g / s, b / s); }

const ColorRGB &ColorRGB::operator+=(const ColorRGB &other) {
  r += other.r;
  g += other.g;
  b += other.b;
  return *this;
}

const ColorRGB &ColorRGB::operator-=(const ColorRGB &other) {
  r -= other.r;
  g -= other.g;
  b -= other.b;
  return *this;
}

const ColorRGB &ColorRGB::operator*=(float s) {
  r *= s;
  g *= s;
  b *= s;
  return *this;
}

const ColorRGB &ColorRGB::operator/=(float s) {
  r /= s;
  g /= s;
  b /= s;
  return *this;
}

std::ostream &operator<<(std::ostream &stream, const ColorRGB &c) {
  stream << "(" << c.r << "," << c.g << "," << c.b << ")";
  return stream;
}

const ColorRGB operator*(float s, const ColorRGB &c) { return ColorRGB(c.r * s, c.g * s, c.b * s); }
