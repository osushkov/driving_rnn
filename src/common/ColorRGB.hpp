#pragma once

#include <iosfwd>

struct ColorRGB {
  float r;
  float g;
  float b;

  static ColorRGB Black(void);
  static ColorRGB White(void);
  static ColorRGB Red(void);
  static ColorRGB Green(void);
  static ColorRGB Blue(void);

  ColorRGB();
  ColorRGB(float r, float g, float b);
  ColorRGB(const ColorRGB &other);

  void Set(float r, float g, float b);

  const ColorRGB operator+(const ColorRGB &c) const;
  const ColorRGB operator-(const ColorRGB &c) const;
  const ColorRGB operator*(float s) const;
  const ColorRGB operator/(float s) const;

  const ColorRGB &operator+=(const ColorRGB &c);
  const ColorRGB &operator-=(const ColorRGB &c);
  const ColorRGB &operator*=(float s);
  const ColorRGB &operator/=(float s);
};

std::ostream &operator<<(std::ostream &stream, const ColorRGB &c);
const ColorRGB operator*(float s, const ColorRGB &c);
