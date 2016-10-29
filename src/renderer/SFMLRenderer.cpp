
#include "SFMLRenderer.hpp"
#include <SFML/Graphics.hpp>
#include <cassert>

using namespace renderer;

struct SFMLRenderer::SFMLRendererImpl {
  sf::RenderWindow window;
  sf::View view;

  SFMLRendererImpl(unsigned width, unsigned height, const string &windowName)
      : window(sf::VideoMode(width, height), windowName),
        view(sf::Vector2f(0, 0), sf::Vector2f(100, (100 * height) / width)) {
    assert(width > 0 && height > 0);

    view.rotate(180.0f);
    window.setView(view);
    window.clear();
  }

  void SwapBuffers(void) {
    window.display();
    window.clear();
  }

  void DrawCircle(const Vector2 &pos, float radius, const ColorRGB &c) {
    assert(radius > 0.0f);

    sf::CircleShape circle(radius);
    circle.setPosition(pos.x - radius, pos.y - radius);
    circle.setFillColor(sf::Color(255 * c.r, 255 * c.g, 255 * c.b));

    window.draw(circle);
  }

  void DrawRectangle(const Vector2 &halfExtents, const Vector2 &pos, const ColorRGB &c) {
    sf::RectangleShape rect(sf::Vector2f(halfExtents.x * 2.0f, halfExtents.y * 2.0f));
    rect.setPosition(pos.x - halfExtents.x, pos.y - halfExtents.y);
    rect.setFillColor(sf::Color(255 * c.r, 255 * c.g, 255 * c.b));

    window.draw(rect);
  }

  void DrawLine(const std::pair<Vector2, ColorRGB> &start,
                const std::pair<Vector2, ColorRGB> &end) {

    sf::Vertex line[] = {
        sf::Vertex(sf::Vector2f(start.first.x, start.first.y),
                   sf::Color(255 * start.second.r, 255 * start.second.g, 255 * start.second.b)),
        sf::Vertex(sf::Vector2f(end.first.x, end.first.y),
                   sf::Color(255 * end.second.r, 255 * end.second.g, 255 * end.second.b))};

    window.draw(line, 2, sf::Lines);
  }
};

SFMLRenderer::SFMLRenderer(unsigned width, unsigned height, const string &windowName)
    : impl(new SFMLRendererImpl(width, height, windowName)) {}

SFMLRenderer::~SFMLRenderer() = default;

void SFMLRenderer::SwapBuffers(void) { impl->SwapBuffers(); }

void SFMLRenderer::DrawCircle(const Vector2 &pos, float radius, const ColorRGB &c) {
  impl->DrawCircle(pos, radius, c);
}

void SFMLRenderer::DrawRectangle(const Vector2 &halfExtents, const Vector2 &pos,
                                 const ColorRGB &c) {
  impl->DrawRectangle(halfExtents, pos, c);
}

void SFMLRenderer::DrawLine(const std::pair<Vector2, ColorRGB> &start,
                            const std::pair<Vector2, ColorRGB> &end) {
  impl->DrawLine(start, end);
}
