#include <iomanip>
#include <iostream>
#include <random>

using namespace std;

int main() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);
  cout << fixed << setprecision(12);

  double cx[3], cy[3], r[3];
  for (int i = 0; i < 3; ++i) cin >> cx[i] >> cy[i] >> r[i];

  const double min_x = -1.0, max_x = 2.0;
  const double min_y = -1.0, max_y = 2.0;
  const double rect_area = 9.0;

  const int N = 300000;
  int hits = 0;

  uint64_t seed = 12345;
  auto next = [&]() -> uint64_t {
    uint64_t z = (seed += 0x9e3779b97f4a7c15ULL);
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
    z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
    return z ^ (z >> 31);
  };

  for (int i = 0; i < N; ++i) {
    double u = (next() >> 11) * 0x1.0p-53;
    double v = (next() >> 11) * 0x1.0p-53;
    double x = min_x + u * (max_x - min_x);
    double y = min_y + v * (max_y - min_y);

    bool in = true;
    for (int j = 0; j < 3; ++j) {
      double dx = x - cx[j];
      double dy = y - cy[j];
      if (dx * dx + dy * dy > r[j] * r[j]) {
        in = false;
        break;
      }
    }
    if (in) ++hits;
  }

  double area = (double)hits / N * rect_area;
  cout << area << '\n';
  return 0;
}
