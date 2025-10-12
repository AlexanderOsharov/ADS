#include <algorithm>
#include <climits>
#include <cmath>
#include <iostream>
#include <vector>

struct DesertScout {
  long long x, y;
  int id;
};

inline long long duneGapSq(const DesertScout& a, const DesertScout& b) {
  long long dx = a.x - b.x;
  long long dy = a.y - b.y;
  return dx * dx + dy * dy;
}

long long bruteForceOasis(std::vector<DesertScout>::const_iterator start,
                          std::vector<DesertScout>::const_iterator finish) {
  long long minGapSq = 9223372036854775807;
  for (auto i = start; i != finish; ++i) {
    for (auto j = i + 1; j != finish; ++j) {
      long long gapSq = duneGapSq(*i, *j);
      if (gapSq < minGapSq) {
        minGapSq = gapSq;
      }
    }
  }
  return minGapSq;
}

std::vector<DesertScout> dangerStripBuffer;

bool sortByYThenX(const DesertScout& a, const DesertScout& b) {
  if (a.y != b.y) return a.y < b.y;
  return a.x < b.x;
}

long long divideTheDunes(std::vector<DesertScout>& scouts, int lo, int hi) {
  int count = hi - lo;
  if (count <= 3) {
    return bruteForceOasis(scouts.begin() + lo, scouts.begin() + hi);
  }

  int mid = (lo + hi) / 2;
  long long midX = scouts[mid].x;

  long long leftMin = divideTheDunes(scouts, lo, mid);
  long long rightMin = divideTheDunes(scouts, mid, hi);

  long long bestGapSq = std::min(leftMin, rightMin);
  if (bestGapSq == 0) return 0;

  dangerStripBuffer.clear();
  dangerStripBuffer.reserve(hi - lo);
  for (int i = lo; i < hi; ++i) {
    long long dx = scouts[i].x - midX;
    if (dx * dx < bestGapSq) {
      dangerStripBuffer.push_back(scouts[i]);
    }
  }

  std::sort(dangerStripBuffer.begin(), dangerStripBuffer.end(), sortByYThenX);

  const int MAX_CHECK = 6;
  for (size_t i = 0; i < dangerStripBuffer.size(); ++i) {
    for (int j = 1; j <= MAX_CHECK && i + j < dangerStripBuffer.size(); ++j) {
      long long dy = dangerStripBuffer[i + j].y - dangerStripBuffer[i].y;
      if (dy * dy >= bestGapSq) break;
      long long newGapSq =
          duneGapSq(dangerStripBuffer[i], dangerStripBuffer[i + j]);
      if (newGapSq < bestGapSq) {
        bestGapSq = newGapSq;
        if (bestGapSq == 0) return 0;
      }
    }
  }

  return bestGapSq;
}

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  std::vector<DesertScout> scouts;
  long long x, y;
  int idx = 1;
  while (std::cin >> x >> y) {
    scouts.push_back({x, y, idx++});
  }

  if (scouts.size() < 2) return 0;

  std::sort(scouts.begin(), scouts.end(),
            [](const DesertScout& a, const DesertScout& b) {
              if (a.x != b.x) return a.x < b.x;
              return a.y < b.y;
            });

  long long minDistSq = divideTheDunes(scouts, 0, (int)scouts.size());

  long long d = static_cast<long long>(std::sqrt((double)minDistSq));
  while (d * d > minDistSq) --d;
  while ((d + 1) * (d + 1) <= minDistSq) ++d;

  std::cout << d << "\n";
  return 0;
}
