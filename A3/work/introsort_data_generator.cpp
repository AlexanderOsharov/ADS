#include <algorithm>
#include <chrono>
#include <climits>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <vector>

using namespace std;

class ArrayGenerator {
 private:
  vector<int> base;
  int max_size = 10000000;
  int min_val = 0, max_val = 1000000000;

  void fill_base() {
    mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
    uniform_int_distribution<int> dist(min_val, max_val);
    base.reserve(max_size + 1);
    for (int i = 0; i <= max_size; ++i) base.push_back(dist(rng));
  }

 public:
  ArrayGenerator() { fill_base(); }

  vector<int> getRandomArray(int size) {
    size = min(size, max_size);
    return vector<int>(base.begin(), base.begin() + size);
  }

  vector<int> getReversedArray(int size) {
    size = min(size, max_size);
    auto rev = vector<int>(base.begin(), base.begin() + size);
    sort(rev.rbegin(), rev.rend());
    return rev;
  }

  vector<int> getNearlySortedArray(int size) {
    size = min(size, max_size);
    auto near = vector<int>(base.begin(), base.begin() + size);
    sort(near.begin(), near.end());
    mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
    uniform_int_distribution<int> pos(0, size - 1);
    for (int i = 0; i < size / 25; ++i) swap(near[pos(rng)], near[pos(rng)]);
    return near;
  }

  vector<int> getSortedArray(int size) {
    size = min(size, max_size);
    auto s = vector<int>(base.begin(), base.begin() + size);
    sort(s.begin(), s.end());
    return s;
  }
};

class SortTester {
  const int trials = 15;
  const int cutoff = 16;

  void insertionSort(vector<int>& arr, int left, int right) {
    for (int i = left + 1; i <= right; ++i) {
      int key = arr[i];
      int j = i - 1;
      while (j >= left && arr[j] > key) {
        arr[j + 1] = arr[j];
        --j;
      }
      arr[j + 1] = key;
    }
  }

  void heapify(vector<int>& arr, int n, int i) {
    int largest = i;
    int l = 2 * i + 1;
    int r = 2 * i + 2;

    if (l < n && arr[l] > arr[largest]) largest = l;
    if (r < n && arr[r] > arr[largest]) largest = r;

    if (largest != i) {
      swap(arr[i], arr[largest]);
      heapify(arr, n, largest);
    }
  }

  void heapSort(vector<int>& arr, int left, int right) {
    int n = right - left + 1;
    vector<int> temp(arr.begin() + left, arr.begin() + right + 1);

    for (int i = n / 2 - 1; i >= 0; --i) heapify(temp, n, i);

    for (int i = n - 1; i > 0; --i) {
      swap(temp[0], temp[i]);
      heapify(temp, i, 0);
    }

    for (int i = 0; i < n; ++i) arr[left + i] = temp[i];
  }

  int partition(vector<int>& arr, int low, int high, bool use_random) {
    if (use_random) {
      static mt19937 gen(random_device{}());
      uniform_int_distribution<> dis(low, high);
      swap(arr[dis(gen)], arr[high]);
    }
    int pivot = arr[high];
    int i = low - 1;

    for (int j = low; j < high; ++j) {
      if (arr[j] <= pivot) swap(arr[++i], arr[j]);
    }
    swap(arr[i + 1], arr[high]);
    return i + 1;
  }

  void quickSortImpl(vector<int>& arr, int left, int right, int depth_limit,
                     bool use_random, int cutoff) {
    while (left < right) {
      int size = right - left + 1;
      if (size <= cutoff) {
        insertionSort(arr, left, right);
        return;
      }
      if (depth_limit == 0) {
        heapSort(arr, left, right);
        return;
      }

      int pi = partition(arr, left, right, use_random);

      if (pi - left < right - pi) {
        quickSortImpl(arr, left, pi - 1, depth_limit - 1, use_random, cutoff);
        left = pi + 1;
      } else {
        quickSortImpl(arr, pi + 1, right, depth_limit - 1, use_random, cutoff);
        right = pi - 1;
      }
    }
  }

 public:
  double measure(vector<int> arr, int size, double depth_coef,
                 bool use_random) {
    long long total = 0;
    for (int t = 0; t < trials; ++t) {
      vector<int> copy = arr;
      auto start = chrono::high_resolution_clock::now();

      int depth_limit =
          (depth_coef <= 0.0)
              ? INT_MAX
              : static_cast<int>(depth_coef * log2(static_cast<double>(size)));

      quickSortImpl(copy, 0, size - 1, depth_limit, use_random, cutoff);

      auto end = chrono::high_resolution_clock::now();
      total += chrono::duration_cast<chrono::microseconds>(end - start).count();
    }
    return static_cast<double>(total) / trials;
  }

  double measureStd(vector<int> arr) {
    long long total = 0;
    for (int t = 0; t < trials; ++t) {
      vector<int> copy = arr;
      auto start = chrono::high_resolution_clock::now();
      std::sort(copy.begin(), copy.end());
      auto end = chrono::high_resolution_clock::now();
      total += chrono::duration_cast<chrono::microseconds>(end - start).count();
    }
    return static_cast<double>(total) / trials;
  }

  double measureHeap(vector<int> arr, int size) {
    long long total = 0;
    for (int t = 0; t < trials; ++t) {
      vector<int> copy = arr;
      auto start = chrono::high_resolution_clock::now();
      heapSort(copy, 0, size - 1);
      auto end = chrono::high_resolution_clock::now();
      total += chrono::duration_cast<chrono::microseconds>(end - start).count();
    }
    return static_cast<double>(total) / trials;
  }
};

int main() {
  setlocale(LC_ALL, "ru_RU.UTF-8");
  ios::sync_with_stdio(false);
  cin.tie(nullptr);

  ArrayGenerator gen;
  SortTester tester;

  vector<string> filenames = {"times_random.txt", "times_nearly.txt",
                              "times_reversed.txt", "times_sorted.txt"};
  vector<ofstream> outs(4);
  for (int i = 0; i < 4; ++i) {
    outs[i].open(filenames[i]);
    outs[i]
        << "n std_sort heap quick_random intro_random quick_det intro_det\n";
  }

  // === Хорошие случаи ===
  int good_max = 10000000;
  int step = 50000;
  int total_good = (good_max - 10000) / step + 1;

  cout << "Обработка случайных и почти отсортированных массивов...\n";
  for (int idx = 0, n = 10000; n <= good_max; n += step, ++idx) {
    auto rand_arr = gen.getRandomArray(n);
    auto near_arr = gen.getNearlySortedArray(n);

    double tr_std = tester.measureStd(rand_arr);
    double tr_heap = tester.measureHeap(rand_arr, n);
    double tr_qr = tester.measure(rand_arr, n, 0.0, true);
    double tr_ir = tester.measure(rand_arr, n, 2.0, true);
    double tr_qd = tester.measure(rand_arr, n, 0.0, false);
    double tr_id = tester.measure(rand_arr, n, 2.0, false);

    double tn_std = tester.measureStd(near_arr);
    double tn_heap = tester.measureHeap(near_arr, n);
    double tn_qr = tester.measure(near_arr, n, 0.0, true);
    double tn_ir = tester.measure(near_arr, n, 2.0, true);
    double tn_qd = tester.measure(near_arr, n, 0.0, false);
    double tn_id = tester.measure(near_arr, n, 2.0, false);

    outs[0] << n << ' ' << tr_std << ' ' << tr_heap << ' ' << tr_qr << ' '
            << tr_ir << ' ' << tr_qd << ' ' << tr_id << '\n';
    outs[1] << n << ' ' << tn_std << ' ' << tn_heap << ' ' << tn_qr << ' '
            << tn_ir << ' ' << tn_qd << ' ' << tn_id << '\n';

    // Прогресс: перезаписываем строку
    cout << "\rХорошие случаи: " << idx + 1 << " / " << total_good << " ("
         << fixed << setprecision(1) << 100.0 * (idx + 1) / total_good
         << "%)          ";
    cout.flush();
  }
  cout << "\n✅ Хорошие случаи завершены!\n";

  // === Плохие случаи ===
  int bad_max = 500000;
  step = 10000;
  int total_bad = (bad_max - 5000) / step + 1;

  cout << "Обработка отсортированных и обратных массивов...\n";
  for (int idx = 0, n = 5000; n <= bad_max; n += step, ++idx) {
    auto rev_arr = gen.getReversedArray(n);
    auto sort_arr = gen.getSortedArray(n);

    double rv_std = tester.measureStd(rev_arr);
    double rv_heap = tester.measureHeap(rev_arr, n);
    double rv_qr = tester.measure(rev_arr, n, 0.0, true);
    double rv_ir = tester.measure(rev_arr, n, 2.0, true);
    double rv_qd = tester.measure(rev_arr, n, 0.0, false);
    double rv_id = tester.measure(rev_arr, n, 2.0, false);

    double s_std = tester.measureStd(sort_arr);
    double s_heap = tester.measureHeap(sort_arr, n);
    double s_qr = tester.measure(sort_arr, n, 0.0, true);
    double s_ir = tester.measure(sort_arr, n, 2.0, true);
    double s_qd = tester.measure(sort_arr, n, 0.0, false);
    double s_id = tester.measure(sort_arr, n, 2.0, false);

    outs[2] << n << ' ' << rv_std << ' ' << rv_heap << ' ' << rv_qr << ' '
            << rv_ir << ' ' << rv_qd << ' ' << rv_id << '\n';
    outs[3] << n << ' ' << s_std << ' ' << s_heap << ' ' << s_qr << ' ' << s_ir
            << ' ' << s_qd << ' ' << s_id << '\n';

    cout << "\rПлохие случаи: " << idx + 1 << " / " << total_bad << " ("
         << fixed << setprecision(1) << 100.0 * (idx + 1) / total_bad
         << "%)          ";
    cout.flush();
  }
  cout << "\n✅ Все данные успешно сгенерированы!\n";
  return 0;
}
