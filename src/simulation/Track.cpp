
#include "Track.hpp"
#include "../math/CollisionLineSegment.hpp"
#include "../math/Geometry.hpp"
#include "../math/Math.hpp"
#include "../math/Vector2.hpp"
#include <cassert>
#include <cmath>
#include <vector>

using namespace simulation;
using namespace std;

struct WallSegment {
  CollisionLineSegment line;
  Vector2 normal;

  ColorRGB startColor;
  ColorRGB endColor;

  WallSegment(CollisionLineSegment line, Vector2 normal, ColorRGB startColor, ColorRGB endColor)
      : line(line), normal(normal), startColor(startColor), endColor(endColor) {}
};

struct Track::TrackImpl {
  vector<WallSegment> walls;
  vector<Vector2> trackLine;
  float trackTotalLength;
  float trackMaxSize;

  Vector2 startPos;
  Vector2 startOrientation;

  TrackImpl(const TrackSpec &spec) {
    generateTrackLine(spec);
    generateWalls(spec);

    startPos = trackLine[0];
    if (rand() % 2 == 0) {
      startOrientation = (trackLine[1] - startPos).normalised();
    } else {
      startOrientation = (trackLine.back() - startPos).normalised();
    }
  }

  void Render(renderer::Renderer *renderer) const {
    for (const auto &wall : walls) {
      renderer->DrawLine(make_pair(wall.line.start, wall.endColor),
                         make_pair(wall.line.end, wall.startColor));
    }
  }

  pair<Vector2, Vector2> StartPosAndOrientation(void) const {
    return make_pair(startPos, startOrientation);
  }

  float DistanceAlongTrack(const Vector2 &point) const {
    float minDistance = 0.0f;
    float bestResult = 0.0f;

    float distanceSoFar = 0.0f;

    for (unsigned i = 0; i < trackLine.size(); i++) {
      unsigned next = (i + 1) % trackLine.size();

      std::pair<Vector2, float> psd =
          Geometry::PointSegmentDist(point, trackLine[i], trackLine[next]);

      if (i == 0 || psd.second < minDistance) {
        minDistance = psd.second;
        bestResult = distanceSoFar + psd.first.distanceTo(trackLine[i]);
      }

      distanceSoFar += trackLine[i].distanceTo(trackLine[next]);
    }

    return bestResult;
  }

  Maybe<ColorRGB> ColorAlongRay(const Vector2 &start, const Vector2 &dir) const {
    CollisionLineSegment line(start, start + dir * trackMaxSize);

    // TODO: if it makes sense we can have a quad-tree or some kind of spatial partitioning
    // structure to make this intersection test faster. But maybe its not necessary given the
    // mall number of walls.

    CollisionResult wallCollision;
    unsigned wallIndex = 0;

    for (unsigned i = 0; i < walls.size(); i++) {
      CollisionResult cr = walls[i].line.IntersectLineSegment(line);
      if (cr.haveCollision) {
        wallIndex = i;
        wallCollision = cr;

        line.Set(start, cr.collisionPoint);
      }
    }

    if (wallCollision.haveCollision) {
      float distToStart = wallCollision.collisionPoint.distanceTo(walls[wallIndex].line.start);
      float f = distToStart / walls[wallIndex].line.length;
      f = std::max(0.0f, std::min(1.0f, f)); // clip to range 0 - 1

      return Maybe<ColorRGB>(walls[wallIndex].endColor * f +
                             walls[wallIndex].startColor * (1.0f - f));
    } else {
      return Maybe<ColorRGB>::none;
    }
  }

  vector<CollisionResult> IntersectSphere(const Vector2 &pos, float radius) const {
    CollisionSphere sphere(pos, radius);

    vector<CollisionResult> result;
    for (const auto &wall : walls) {
      if (wall.line.midPoint.distanceTo2(pos) >
          (radius + wall.line.length / 2.0f) * (radius + wall.line.length / 2.0f)) {
        continue;
      }

      CollisionResult cr = sphere.IntersectLineSegment(wall.line);
      if (cr.haveCollision) {
        cr.collisionNormal = wall.normal;
        result.push_back(cr);
      }
    }
    return result;
  }

  bool generateTrackLine(const TrackSpec &spec) {
    vector<float> perturbAmounts = genPertubation(spec);

    trackLine.reserve(spec.numLinePoints);
    for (int i = 0; i < spec.numLinePoints; i++) {
      float r = spec.radius + perturbAmounts[i];
      float theta = i * 2.0f * static_cast<float>(M_PI) / static_cast<float>(spec.numLinePoints);
      trackLine.emplace_back(cosf(theta) * r, sinf(theta) * r);
    }

    trackTotalLength = 0.0f;
    for (unsigned i = 0; i < trackLine.size(); i++) {
      unsigned next = (i + 1) % trackLine.size();
      trackTotalLength += trackLine[i].distanceTo(trackLine[next]);
    }

    return true;
  }

  vector<float> genPertubation(const TrackSpec &spec) const {
    vector<float> perturbAmounts(spec.numLinePoints, 0.0f);

    for (int scale = 0; scale < 6; scale++) {
      int skip = 1 << scale;
      float rs = pow(2.0f, scale);

      int indexOffset = rand() % spec.numLinePoints;
      for (unsigned i = 0; i < perturbAmounts.size(); i += skip) {
        perturbAmounts[(i + indexOffset) % perturbAmounts.size()] +=
            math::RandInterval(-spec.maxModStrength * 0.5f * rs, spec.maxModStrength * rs);
      }
    }

    smooth(perturbAmounts, 2, 2);
    return perturbAmounts;
  }

  void smooth(vector<float> &target, int neighbourhood, int passes) const {
    for (int p = 0; p < passes; p++) {
      for (unsigned i = 0; i < target.size(); i++) {
        float sum = 0.0f;
        for (int n = -neighbourhood; n <= neighbourhood; n++) {
          unsigned index = (i + n + target.size()) % target.size();
          sum += target[index];
        }
        target[i] = sum / (neighbourhood * 2.0f + 1.0f);
      }
    }
  }

  bool generateWalls(const TrackSpec &spec) {
    const float offsetAmount = spec.trackWidth / 2.0f;

    vector<Vector2> leftWallVerts;
    vector<Vector2> rightWallVerts;
    leftWallVerts.reserve(trackLine.size());
    rightWallVerts.reserve(trackLine.size());

    for (unsigned i = 0; i < trackLine.size(); i++) {
      Vector2 offset = offsetDirection(i) * offsetAmount;
      leftWallVerts.emplace_back(trackLine[i] + offset);
      rightWallVerts.emplace_back(trackLine[i] - offset);
    }

    float minX = leftWallVerts[0].x;
    float maxX = minX;

    float minY = leftWallVerts[0].y;
    float maxY = minY;

    walls.reserve(2 * trackLine.size());
    for (unsigned i = 0; i < trackLine.size(); i++) {
      minX = std::min(minX, leftWallVerts[i].x);
      minX = std::min(minX, rightWallVerts[i].x);
      minY = std::min(minY, leftWallVerts[i].y);
      minY = std::min(minY, rightWallVerts[i].y);
      maxX = std::max(maxX, leftWallVerts[i].x);
      maxX = std::max(maxX, rightWallVerts[i].x);
      maxY = std::max(maxY, leftWallVerts[i].y);
      maxY = std::max(maxY, rightWallVerts[i].y);

      int nextIndex = (i + 1) % trackLine.size();

      Vector2 toNextLeft = (leftWallVerts[nextIndex] - leftWallVerts[i]).normalised();
      Vector2 toNextRight = (rightWallVerts[nextIndex] - rightWallVerts[i]).normalised();

      walls.emplace_back(CollisionLineSegment(leftWallVerts[i], leftWallVerts[nextIndex]),
                         toNextLeft.rotated(-static_cast<float>(M_PI) / 2.0f), ColorRGB::Blue(),
                         ColorRGB::Green());
      walls.emplace_back(CollisionLineSegment(rightWallVerts[i], rightWallVerts[nextIndex]),
                         toNextRight.rotated(static_cast<float>(M_PI) / 2.0f), ColorRGB::White(),
                         ColorRGB::Red());
    }

    trackMaxSize = sqrtf((maxX - minX) * (maxX - minX) + (maxY - minY) * (maxY - minY));
    return true;
  }

  Vector2 offsetDirection(int trackIndex) const {
    int nextIndex = (trackIndex + 1) % trackLine.size();
    int prevIndex = (trackIndex + trackLine.size() - 1) % trackLine.size();

    Vector2 toNext = trackLine[nextIndex] - trackLine[trackIndex];
    Vector2 fromPrev = trackLine[trackIndex] - trackLine[prevIndex];

    Vector2 avrgDir = (toNext + fromPrev).normalised();
    avrgDir.rotate(static_cast<float>(M_PI) / 2.0f);
    return avrgDir;
  }
};

Track::Track(const TrackSpec &spec) : impl(new TrackImpl(spec)) {}

Track::~Track() = default;

void Track::Render(renderer::Renderer *renderer) const { impl->Render(renderer); }

pair<Vector2, Vector2> Track::StartPosAndOrientation(void) const {
  return impl->StartPosAndOrientation();
}

float Track::DistanceAlongTrack(const Vector2 &point) const {
  return impl->DistanceAlongTrack(point);
}

Maybe<ColorRGB> Track::ColorAlongRay(const Vector2 &start, const Vector2 &dir) const {
  return impl->ColorAlongRay(start, dir);
}

vector<CollisionResult> Track::IntersectSphere(const Vector2 &pos, float radius) const {
  return impl->IntersectSphere(pos, radius);
}
