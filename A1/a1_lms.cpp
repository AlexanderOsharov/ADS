#include <cmath>
#include <iomanip>
#include <iostream>
#include <random>
#include <vector>

using namespace std;

bool is_inside_circle(double cx, double cy, double r, double x, double y) {
  double dx = x - cx;
  double dy = y - cy;
  return dx * dx + dy * dy <= r * r;
}

int main() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);
  cout << fixed << setprecision(12);
  vector<double> circle_a(3);
  vector<double> circle_b(3);
  vector<double> circle_c(3);

  circle_a[0] = 1.0;
  circle_a[1] = 1.0;
  circle_a[2] = 1.0;

  circle_b[0] = 1.5;
  circle_b[1] = 2.0;
  circle_b[2] = sqrt(5) / 2;

  circle_c[0] = 2.0;
  circle_c[1] = 1.5;
  circle_c[2] = sqrt(5) / 2;

  double x_min = max(max(circle_a[0] - circle_a[2], circle_b[0] - circle_b[2]),
                     circle_c[0] - circle_c[2]);
  double x_max = min(min(circle_a[0] + circle_a[2], circle_b[0] + circle_b[2]),
                     circle_c[0] + circle_c[2]);
  double y_min = max(max(circle_a[1] - circle_a[2], circle_b[1] - circle_b[2]),
                     circle_c[1] - circle_c[2]);
  double y_max = min(min(circle_a[1] + circle_a[2], circle_b[1] + circle_b[2]),
                     circle_c[1] + circle_c[2]);

  double rect_area = (x_max - x_min) * (y_max - y_min);

  uint64_t seed = 12345;
  auto next_random = [&]() -> uint64_t {
    uint64_t z = (seed += 0x9e3779b97f4a7c15ULL);
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
    z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
    return z ^ (z >> 31);
  };

  for (int total_points = 100; total_points <= 100000; total_points += 500) {
    int hits = 0;

    for (int j = 0; j < total_points; ++j) {
      double u = (next_random() >> 11) * 0x1.0p-53;
      double v = (next_random() >> 11) * 0x1.0p-53;
      double x = x_min + u * (x_max - x_min);
      double y = y_min + v * (y_max - y_min);

      bool inside_all = true;
      inside_all &=
          is_inside_circle(circle_a[0], circle_a[1], circle_a[2], x, y);
      inside_all &=
          is_inside_circle(circle_b[0], circle_b[1], circle_b[2], x, y);
      inside_all &=
          is_inside_circle(circle_c[0], circle_c[1], circle_c[2], x, y);

      if (inside_all) {
        ++hits;
      }
    }

    double estimated_area =
        (static_cast<double>(hits) / total_points) * rect_area;

    cout << total_points << ' ' << estimated_area << '\n';
  }

  return 0;
}
