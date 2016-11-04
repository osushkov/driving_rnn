
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
    startOrientation = (trackLine[1] - startPos).normalised();
  }

  void Render(renderer::Renderer *renderer) const {
    for (const auto &wall : walls) {
      renderer->DrawLine(make_pair(wall.line.start, wall.startColor),
                         make_pair(wall.line.end, wall.endColor));
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

  Maybe<TrackRayIntersection> IntersectRay(const Vector2 &start, const Vector2 &dir) const {
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
      ColorRGB color(walls[wallIndex].endColor * f + walls[wallIndex].startColor * (1.0f - f));

      return Maybe<TrackRayIntersection>(TrackRayIntersection(wallCollision.collisionPoint, color));
    } else {
      return Maybe<TrackRayIntersection>::none;
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
    vector<float> result(target.size());
    for (int p = 0; p < passes; p++) {
      for (unsigned i = 0; i < target.size(); i++) {
        float sum = 0.0f;
        for (int n = -neighbourhood; n <= neighbourhood; n++) {
          unsigned index = (i + n + target.size()) % target.size();
          sum += target[index];
        }
        result[i] = sum / (neighbourhood * 2.0f + 1.0f);
      }
      target = result;
    }
  }

  bool generateWalls(const TrackSpec &spec) {
    vector<float> trackWidths = genTrackWidths(spec);

    vector<Vector2> leftWallVerts;
    vector<Vector2> rightWallVerts;
    leftWallVerts.reserve(trackLine.size());
    rightWallVerts.reserve(trackLine.size());

    for (unsigned i = 0; i < trackLine.size(); i++) {
      Vector2 offset = offsetDirection(i) * trackWidths[i] / 2.0f;
      leftWallVerts.emplace_back(trackLine[i] + offset);
      rightWallVerts.emplace_back(trackLine[i] - offset);
    }

    vector<WallSegment> leftWall, rightWall;
    leftWall.reserve(trackLine.size());
    rightWall.reserve(trackLine.size());

    for (unsigned i = 0; i < trackLine.size(); i++) {
      int nextIndex = (i + 1) % trackLine.size();

      Vector2 toNextLeft = (leftWallVerts[nextIndex] - leftWallVerts[i]).normalised();
      Vector2 toNextRight = (rightWallVerts[nextIndex] - rightWallVerts[i]).normalised();

      leftWall.emplace_back(CollisionLineSegment(leftWallVerts[i], leftWallVerts[nextIndex]),
                            toNextLeft.rotated(-static_cast<float>(M_PI) / 2.0f), ColorRGB::Blue(),
                            ColorRGB::Green());
      rightWall.emplace_back(CollisionLineSegment(rightWallVerts[i], rightWallVerts[nextIndex]),
                             toNextRight.rotated(static_cast<float>(M_PI) / 2.0f),
                             ColorRGB::White(), ColorRGB::Red());
    }

    leftWall = deIntersectWalls(leftWall);
    rightWall = deIntersectWalls(rightWall);
    walls.insert(walls.end(), leftWall.begin(), leftWall.end());
    walls.insert(walls.end(), rightWall.begin(), rightWall.end());

    computeTrackMaxSize();
    return true;
  }

  vector<WallSegment> deIntersectWalls(const vector<WallSegment> &prelimWalls) {
    vector<WallSegment> result;
    result.reserve(prelimWalls.size());

    const unsigned maxLookahead = 5;

    for (unsigned i = 0; i < prelimWalls.size(); i++) {
      bool haveIntersection = false;

      for (unsigned offset = maxLookahead; offset >= 2; offset--) {
        unsigned ni = (i + offset) % prelimWalls.size();
        CollisionResult cr = prelimWalls[i].line.IntersectLineSegment(prelimWalls[ni].line);

        if (cr.haveCollision) {
          WallSegment modifiedLineA = prelimWalls[i];
          WallSegment modifiedLineB = prelimWalls[ni];

          modifiedLineA.line.Set(modifiedLineA.line.start, cr.collisionPoint);
          modifiedLineB.line.Set(cr.collisionPoint, modifiedLineB.line.end);

          result.push_back(modifiedLineA);
          result.push_back(modifiedLineB);

          i = ni;
          haveIntersection = true;
          break;
        }
      }

      if (!haveIntersection) {
        result.push_back(prelimWalls[i]);
      }
    }

    cout << "num walls: " << result.size() << endl;
    return result;
  }

  void computeTrackMaxSize(void) {
    float minX = walls[0].line.start.x;
    float maxX = minX;

    float minY = walls[0].line.start.y;
    float maxY = minY;

    for (const auto &w : walls) {
      minX = std::min(minX, w.line.start.x);
      minY = std::min(minY, w.line.start.y);
      maxX = std::max(maxX, w.line.start.x);
      maxY = std::max(maxY, w.line.start.y);
    }

    trackMaxSize = sqrtf((maxX - minX) * (maxX - minX) + (maxY - minY) * (maxY - minY));
  }

  vector<float> genTrackWidths(const TrackSpec &spec) {
    unsigned extra = 5;
    vector<float> result(trackLine.size() + extra);
    for (unsigned i = 0; i < trackLine.size(); i++) {
      result[i] = math::RandInterval(spec.trackMinWidth, spec.trackMaxWidth);
    }
    for (unsigned i = 0; i < extra; i++) {
      result[i + trackLine.size()] = result[i];
    }
    smooth(result, 1, 1);
    for (unsigned i = 0; i < extra; i++) {
      result.pop_back();
    }
    return result;
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

Maybe<TrackRayIntersection> Track::IntersectRay(const Vector2 &start, const Vector2 &dir) const {
  return impl->IntersectRay(start, dir);
}

vector<CollisionResult> Track::IntersectSphere(const Vector2 &pos, float radius) const {
  return impl->IntersectSphere(pos, radius);
}
