#include <algorithm>
#include <cmath>
#include <iostream>
#include <tuple>
#include <unordered_map>
#include <vector>

using ll = long long;

struct CosmicCrumb {
  ll x, y, z;
  int badge;
};

inline ll cosmicGapSq(const CosmicCrumb& a, const CosmicCrumb& b) {
  ll dx = a.x - b.x;
  ll dy = a.y - b.y;
  ll dz = a.z - b.z;
  return dx * dx + dy * dy + dz * dz;
}

struct NebulaHash {
  size_t operator()(const std::tuple<ll, ll, ll>& cell) const {
    ll x = std::get<0>(cell);
    ll y = std::get<1>(cell);
    ll z = std::get<2>(cell);
    return (size_t)(x * 73856093LL ^ y * 19349663LL ^ z * 83492791LL);
  }
};

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  int n;
  std::cin >> n;
  std::vector<CosmicCrumb> specks(n);
  for (int i = 0; i < n; ++i) {
    std::cin >> specks[i].x >> specks[i].y >> specks[i].z;
    specks[i].badge = i + 1;
  }

  if (n <= 1000) {
    ll tightestGapSq = (ll)4e18;
    int speckAlpha = -1, speckBeta = -1;
    for (int i = 0; i < n; ++i) {
      for (int j = i + 1; j < n; ++j) {
        ll gapSq = cosmicGapSq(specks[i], specks[j]);
        if (gapSq < tightestGapSq) {
          tightestGapSq = gapSq;
          speckAlpha = specks[i].badge;
          speckBeta = specks[j].badge;
        }
      }
    }
    double minDistance = std::sqrt((double)tightestGapSq);
    if (speckAlpha > speckBeta) std::swap(speckAlpha, speckBeta);
    std::cout << std::fixed << minDistance << "\n";
    std::cout << speckAlpha << " " << speckBeta << "\n";
    return 0;
  }

  ll tightestGapSq = (ll)4e18;
  int speckAlpha = -1, speckBeta = -1;
  const int scoutCount = 200;

  for (int i = 0; i < scoutCount; ++i) {
    for (int j = i + 1; j < scoutCount; ++j) {
      ll gapSq = cosmicGapSq(specks[i], specks[j]);
      if (gapSq < tightestGapSq) {
        tightestGapSq = gapSq;
        speckAlpha = specks[i].badge;
        speckBeta = specks[j].badge;
      }
    }
  }

  double scoutRadius = std::sqrt((double)tightestGapSq);
  ll cellSize = (ll)(scoutRadius / 2);
  if (cellSize == 0) cellSize = 1;

  std::unordered_map<std::tuple<ll, ll, ll>, std::vector<int>, NebulaHash>
      cosmicGrid;

  for (int i = 0; i < n; ++i) {
    ll sectorX = specks[i].x / cellSize;
    ll sectorY = specks[i].y / cellSize;
    ll sectorZ = specks[i].z / cellSize;

    for (ll dx = -2; dx <= 2; ++dx) {
      for (ll dy = -2; dy <= 2; ++dy) {
        for (ll dz = -2; dz <= 2; ++dz) {
          std::tuple<ll, ll, ll> neighborSector =
              std::make_tuple(sectorX + dx, sectorY + dy, sectorZ + dz);
          auto it = cosmicGrid.find(neighborSector);
          if (it != cosmicGrid.end()) {
            for (int j : it->second) {
              ll gapSq = cosmicGapSq(specks[i], specks[j]);
              if (gapSq < tightestGapSq) {
                tightestGapSq = gapSq;
                speckAlpha = specks[i].badge;
                speckBeta = specks[j].badge;
              }
            }
          }
        }
      }
    }

    std::tuple<ll, ll, ll> homeSector =
        std::make_tuple(sectorX, sectorY, sectorZ);
    cosmicGrid[homeSector].push_back(i);
  }

  double minDistance = std::sqrt((double)tightestGapSq);
  if (speckAlpha > speckBeta) std::swap(speckAlpha, speckBeta);
  std::cout << std::fixed << minDistance << "\n";
  std::cout << speckAlpha << " " << speckBeta << "\n";
  return 0;
}
