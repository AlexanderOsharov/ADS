#include <algorithm>
#include <chrono>
#include <codecvt>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <locale>
#include <random>
#include <vector>

class ArrayGenerator {
 private:
  std::vector<int> base;
  int max_size = 100000;
  int min_val = 0, max_val = 10000;

  void fill_base() {
    std::mt19937 rng(
        std::chrono::steady_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> dist(min_val, max_val);
    base.reserve(max_size + 1);
    for (int i = 0; i <= max_size; ++i) {
      base.push_back(dist(rng));
    }
  }

 public:
  ArrayGenerator() { fill_base(); }

  std::vector<int> getRandomArray(int size) {
    size = std::min(size, max_size);
    return std::vector<int>(base.begin(), base.begin() + size);
  }

  std::vector<int> getReversedArray(int size) {
    size = std::min(size, max_size);
    std::vector<int> rev(base.begin(), base.begin() + size);
    std::sort(rev.rbegin(), rev.rend());
    return rev;
  }

  std::vector<int> getNearlySortedArray(int size, int swaps) {
    size = std::min(size, max_size);
    std::vector<int> near(base.begin(), base.begin() + size);
    std::sort(near.begin(), near.end());
    std::mt19937 rng(
        std::chrono::steady_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> pos(0, size - 1);
    for (int i = 0; i < swaps; ++i) {
      std::swap(near[pos(rng)], near[pos(rng)]);
    }
    return near;
  }
};

class SortTester {
  const int trials = 15;

  void merge(std::vector<int>& arr, int l, int m, int r) {
    int len1 = m - l + 1, len2 = r - m;
    std::vector<int> left(len1), right(len2);
    for (int i = 0; i < len1; ++i) left[i] = arr[l + i];
    for (int i = 0; i < len2; ++i) right[i] = arr[m + 1 + i];
    int i = 0, j = 0, k = l;
    while (i < len1 && j < len2)
      arr[k++] = (left[i] <= right[j]) ? left[i++] : right[j++];
    while (i < len1) arr[k++] = left[i++];
    while (j < len2) arr[k++] = right[j++];
  }

  void merge_sort_rec(std::vector<int>& arr, int l, int r) {
    if (l >= r) return;
    int mid = l + (r - l) / 2;
    merge_sort_rec(arr, l, mid);
    merge_sort_rec(arr, mid + 1, r);
    merge(arr, l, mid, r);
  }

  void insertion_sort_seg(std::vector<int>& arr, int l, int r) {
    for (int i = l + 1; i <= r; ++i) {
      int val = arr[i], j = i - 1;
      while (j >= l && arr[j] > val) {
        arr[j + 1] = arr[j];
        --j;
      }
      arr[j + 1] = val;
    }
  }

  void hybrid_sort_rec(std::vector<int>& arr, int l, int r, int thresh) {
    if (r - l + 1 <= thresh) {
      insertion_sort_seg(arr, l, r);
      return;
    }
    if (l < r) {
      int mid = l + (r - l) / 2;
      merge_sort_rec(arr, l, mid);
      merge_sort_rec(arr, mid + 1, r);
      merge(arr, l, mid, r);
    }
  }

 public:
  double measureMergeSortTime(std::vector<int> arr, int size) {
    long long total = 0;
    for (int t = 0; t < trials; ++t) {
      auto start = std::chrono::high_resolution_clock::now();
      merge_sort_rec(arr, 0, size - 1);
      auto end = std::chrono::high_resolution_clock::now();
      total +=
          std::chrono::duration_cast<std::chrono::microseconds>(end - start)
              .count();
    }
    return static_cast<double>(total) / trials;
  }

  double measureHybridMergeSortTime(std::vector<int> arr, int size,
                                    int thresh) {
    long long total = 0;
    for (int t = 0; t < trials; ++t) {
      auto start = std::chrono::high_resolution_clock::now();
      hybrid_sort_rec(arr, 0, size - 1, thresh);
      auto end = std::chrono::high_resolution_clock::now();
      total +=
          std::chrono::duration_cast<std::chrono::microseconds>(end - start)
              .count();
    }
    return static_cast<double>(total) / trials;
  }
};

int main() {
  // Устанавливаем локаль для корректного отображения русских символов в консоли
  std::locale::global(std::locale(""));
  std::cout.imbue(std::locale());

  ArrayGenerator gen;
  SortTester tester;

  std::vector<std::string> files = {"merge_rand_.txt", "merge_rev_.txt",
                                    "merge_near_.txt", "hybrid_rand_.txt",
                                    "hybrid_rev_.txt", "hybrid_near_.txt"};
  std::vector<std::ofstream> outs(6);
  for (int i = 0; i < 6; ++i) outs[i].open(files[i]);

  std::vector<int> thresholds = {8, 16, 24, 32, 48};

  const int start_n = 500;
  const int end_n = 100000;
  const int step = 100;
  const int total_iterations = (end_n - start_n) / step + 1;

  auto overall_start = std::chrono::high_resolution_clock::now();

  for (int n = start_n, iteration = 1; n <= end_n; n += step, ++iteration) {
    // --- Начало замеров для текущей итерации ---
    auto iter_start = std::chrono::high_resolution_clock::now();

    auto rand_arr = gen.getRandomArray(n);
    auto rev_arr = gen.getReversedArray(n);
    auto near_arr = gen.getNearlySortedArray(n, n / 25);

    double t1 = tester.measureMergeSortTime(rand_arr, n);
    double t2 = tester.measureMergeSortTime(rev_arr, n);
    double t3 = tester.measureMergeSortTime(near_arr, n);

    outs[0] << n << ' ' << t1 << '\n';
    outs[1] << n << ' ' << t2 << '\n';
    outs[2] << n << ' ' << t3 << '\n';

    outs[3] << n;
    outs[4] << n;
    outs[5] << n;

    for (int th : thresholds) {
      outs[3] << ' ' << tester.measureHybridMergeSortTime(rand_arr, n, th);
      outs[4] << ' ' << tester.measureHybridMergeSortTime(rev_arr, n, th);
      outs[5] << ' ' << tester.measureHybridMergeSortTime(near_arr, n, th);
    }
    outs[3] << '\n';
    outs[4] << '\n';
    outs[5] << '\n';

    // --- Конец замеров для текущей итерации ---
    auto iter_end = std::chrono::high_resolution_clock::now();
    auto iter_duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                             iter_end - iter_start)
                             .count();

    // --- Расчёт и вывод прогресса ---
    double progress =
        (static_cast<double>(iteration) / total_iterations) * 100.0;
    auto elapsed_so_far = std::chrono::duration_cast<std::chrono::milliseconds>(
                              iter_end - overall_start)
                              .count();
    // Предполагаем, что среднее время на итерацию останется постоянным
    double avg_time_per_iter = static_cast<double>(elapsed_so_far) / iteration;
    long long estimated_total_time =
        static_cast<long long>(avg_time_per_iter * total_iterations);
    long long estimated_remaining_time = estimated_total_time - elapsed_so_far;

    std::cout << "\rПрогресс: " << std::fixed << std::setprecision(2)
              << progress << "% (" << iteration << "/" << total_iterations
              << "). "
              << "Осталось примерно: " << estimated_remaining_time / 1000.0
              << " сек. "
              << "[Итерация заняла: " << iter_duration << " мс]";
    std::cout.flush();  // Принудительно выводим в консоль
  }

  // Перевод строки по окончании
  std::cout << "\nСбор данных завершён!\n";

  for (auto& f : outs) f.close();
  return 0;
}
