
#include "Track.hpp"
#include "../math/CollisionLineSegment.hpp"
#include "../math/Math.hpp"
#include "../math/Vector2.hpp"
#include <cassert>
#include <cmath>
#include <vector>

using namespace simulation;
using namespace std;

struct Track::TrackImpl {
  vector<CollisionLineSegment> walls;
  vector<Vector2> trackLine;

  TrackImpl(const TrackSpec &spec) {
    generateTrackLine(spec);
    // perturbTrack(spec);
    generateWalls(spec);
  }

  void Render(renderer::Renderer *renderer) {
    for (const auto &wall : walls) {
      renderer->DrawLine(make_pair(wall.start, ColorRGB::White()),
                         make_pair(wall.end, ColorRGB::Red()));
    }
  }

  bool generateTrackLine(const TrackSpec &spec) {
    vector<float> perturbAmounts = genPertubation(spec);

    trackLine.reserve(spec.numLinePoints);
    for (int i = 0; i < spec.numLinePoints; i++) {
      float r = spec.radius + perturbAmounts[i];
      float theta = i * 2.0f * static_cast<float>(M_PI) / static_cast<float>(spec.numLinePoints);
      trackLine.emplace_back(cosf(theta) * r, sinf(theta) * r);
    }
    return true;
  }

  vector<float> genPertubation(const TrackSpec &spec) {
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

  void smooth(vector<float> &target, int neighbourhood, int passes) {
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

  void perturbTrack(const TrackSpec &spec) {

    for (int i = 0; i < spec.numMods; i++) {
      Vector2 modPos(math::RandInterval(2.0f * -spec.radius, 2.0f * spec.radius),
                     math::RandInterval(2.0f * -spec.radius, 2.0f * spec.radius));
      float strength = math::RandInterval(0.1f, spec.maxModStrength);
      bool attract = false; // rand() % 2 == 0;

      for (auto &trackPoint : trackLine) {
        float d = std::min(1.0f, modPos.distanceTo2(trackPoint));
        Vector2 displacement = (strength / d) * ((modPos - trackPoint).normalised());
        if (!attract) {
          displacement *= -1.0f;
        }
        trackPoint += displacement;
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

    walls.reserve(2 * trackLine.size());
    for (unsigned i = 0; i < trackLine.size(); i++) {
      int nextIndex = (i + 1) % trackLine.size();
      walls.emplace_back(leftWallVerts[i], leftWallVerts[nextIndex]);
      walls.emplace_back(rightWallVerts[i], rightWallVerts[nextIndex]);
    }

    return true;
  }

  Vector2 offsetDirection(int trackIndex) {
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

void Track::Render(renderer::Renderer *renderer) { impl->Render(renderer); }
