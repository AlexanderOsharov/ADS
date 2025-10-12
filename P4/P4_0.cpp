#include <algorithm>
#include <iomanip>
#include <iostream>
#include <vector>

using namespace std;

int main() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);

  long long L;
  int N;
  cin >> L >> N;

  vector<int> t_seg, v_seg;
  vector<long long> t_pref, s_pref;
  t_pref.push_back(0);
  s_pref.push_back(0);

  for (int i = 0; i < N; ++i) {
    int t, v;
    cin >> t >> v;
    t_seg.push_back(t);
    v_seg.push_back(v);
    t_pref.push_back(t_pref.back() + t);
    s_pref.push_back(s_pref.back() + 1LL * t * v);
  }

  auto when_reach = [&](long long dist) -> double {
    if (dist <= 0) return 0.0;
    if (dist >= s_pref.back()) return (double)t_pref.back();
    int idx =
        upper_bound(s_pref.begin(), s_pref.end(), dist) - s_pref.begin() - 1;
    long long missing = dist - s_pref[idx];

    if (v_seg[idx] == 0) {
      return (double)t_pref[idx];
    }
    return (double)t_pref[idx] + (double)missing / v_seg[idx];
  };

  double best_lag = 0.0;

  if (s_pref.back() < L) {
    best_lag = (double)t_pref.back();
  } else {
    for (int i = 0; i <= N; ++i) {
      if (s_pref[i] >= L) {
        long long s0 = s_pref[i] - L;
        double t0 = when_reach(s0);
        best_lag = max(best_lag, (double)t_pref[i] - t0);
      }
    }

    for (int i = 0; i <= N; ++i) {
      long long s1 = s_pref[i] + L;
      if (s1 > s_pref.back()) break;
      double t1 = when_reach(s1);
      best_lag = max(best_lag, t1 - (double)t_pref[i]);
    }
  }

  cout << fixed << setprecision(3) << best_lag << "\n";
  return 0;
}
