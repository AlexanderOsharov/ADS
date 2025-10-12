#include <algorithm>  // только для std::min/max
#include <iostream>
#include <vector>

struct Interval {
  int left;
  int right;

  size_t length() const {
    return (left <= right) ? static_cast<size_t>(right - left + 1) : 0;
  }

  Interval overlap(const Interval& other) const {
    int new_left = (left > other.left) ? left : other.left;
    int new_right = (right < other.right) ? right : other.right;

    if (new_left > new_right) {
      return {1, 0};
    }

    return {new_left, new_right};
  }
};

bool by_left(const Interval& a, const Interval& b) {
  if (a.left != b.left) return a.left < b.left;
  return a.right < b.right;
}

void merge_segments(std::vector<Interval>& arr, int l, int m, int r) {
  int n1 = m - l, n2 = r - m;
  std::vector<Interval> L(n1), R(n2);
  for (int i = 0; i < n1; ++i) L[i] = arr[l + i];
  for (int j = 0; j < n2; ++j) R[j] = arr[m + j];

  int i = 0, j = 0, k = l;
  while (i < n1 && j < n2) {
    if (by_left(L[i], R[j])) {
      arr[k++] = L[i++];
    } else {
      arr[k++] = R[j++];
    }
  }
  while (i < n1) arr[k++] = L[i++];
  while (j < n2) arr[k++] = R[j++];
}

void sort_segments(std::vector<Interval>& arr, int l, int r) {
  if (r - l <= 1) return;
  int m = l + (r - l) / 2;
  sort_segments(arr, l, m);
  sort_segments(arr, m, r);
  merge_segments(arr, l, m, r);
}

struct BestOverlap {
  size_t max_len = 0;
  Interval champion{1, 0};  // пока без титула

  void challenge(const Interval& candidate) {
    size_t len = candidate.length();
    if (len == 0) return;

    if (len > max_len) {
      max_len = len;
      champion = candidate;
    } else if (len == max_len) {
      // выбираем самого "левого" чемпиона
      if (candidate.left < champion.left ||
          (candidate.left == champion.left &&
           candidate.right < champion.right)) {
        champion = candidate;
      }
    }
  }
};

BestOverlap crown_the_best(const BestOverlap& a, const BestOverlap& b) {
  BestOverlap winner = a;
  if (b.max_len > winner.max_len) {
    winner = b;
  } else if (b.max_len == winner.max_len && b.max_len > 0) {
    const auto& cand = b.champion;
    const auto& curr = winner.champion;
    if (cand.left < curr.left ||
        (cand.left == curr.left && cand.right < curr.right)) {
      winner.champion = cand;
    }
  }
  return winner;
}

BestOverlap find_overlap(const std::vector<Interval>& segments, int l, int r) {
  int n = r - l;
  if (n < 2) return BestOverlap{};
  if (n == 2) {
    BestOverlap res;
    res.challenge(segments[l].overlap(segments[l + 1]));
    return res;
  }

  int mid = l + (r - l) / 2;
  BestOverlap left_best = find_overlap(segments, l, mid);
  BestOverlap right_best = find_overlap(segments, mid, r);

  // Поиск пересечений между левой и правой половинами
  BestOverlap cross_best;

  static std::vector<int> suffix_max;
  int left_len = mid - l;
  if (static_cast<int>(suffix_max.size()) < left_len) {
    suffix_max.resize(left_len);
  }

  suffix_max[left_len - 1] = segments[mid - 1].right;
  for (int i = left_len - 2; i >= 0; --i) {
    suffix_max[i] = std::max(segments[l + i].right, suffix_max[i + 1]);
  }

  int i = l;
  for (int j = mid; j < r; ++j) {
    while (i < mid && segments[i].right < segments[j].left) ++i;
    if (i >= mid) break;

    int best_right = suffix_max[i - l];
    int L = segments[j].left;
    int R = std::min(best_right, segments[j].right);
    if (L <= R) {
      cross_best.challenge({L, R});
    }
  }

  BestOverlap final = crown_the_best(left_best, right_best);
  final = crown_the_best(final, cross_best);
  return final;
}

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  int n;
  std::cin >> n;
  if (n <= 1) {
    std::cout << "0";
    return 0;
  }

  std::vector<Interval> segments(n);
  for (int i = 0; i < n; ++i) {
    std::cin >> segments[i].left >> segments[i].right;
    if (segments[i].left > segments[i].right) {
      std::swap(segments[i].left, segments[i].right);
    }
  }

  sort_segments(segments, 0, n);
  BestOverlap trophy = find_overlap(segments, 0, n);

  std::cout << trophy.max_len << "\n";
  if (trophy.max_len > 0) {
    std::cout << trophy.champion.left << " " << trophy.champion.right;
  }

  return 0;
}
