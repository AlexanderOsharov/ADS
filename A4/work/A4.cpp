#include <windows.h>

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

void SetConsoleToCP1251() {
  SetConsoleOutputCP(1251);
  SetConsoleCP(1251);
}

long long mergeAndCount(vector<int>& arr, int left, int mid, int right) {
  vector<int> temp(right - left + 1);
  int i = left, j = mid + 1, k = 0;
  long long inv_count = 0;

  while (i <= mid && j <= right) {
    if (arr[i] <= arr[j]) {
      temp[k++] = arr[i++];
    } else {
      temp[k++] = arr[j++];
      inv_count += (mid - i + 1);
    }
  }

  while (i <= mid) temp[k++] = arr[i++];
  while (j <= right) temp[k++] = arr[j++];

  for (int idx = 0; idx < k; ++idx) arr[left + idx] = temp[idx];

  return inv_count;
}

long long countInversions(vector<int>& arr, int left, int right) {
  if (left >= right) return 0;
  int mid = left + (right - left) / 2;
  long long inv_count = 0;
  inv_count += countInversions(arr, left, mid);
  inv_count += countInversions(arr, mid + 1, right);
  inv_count += mergeAndCount(arr, left, mid, right);
  return inv_count;
}

long long countSignificantInversions(vector<int>& arr, int left, int right) {
  if (left >= right) return 0;
  int mid = left + (right - left) / 2;
  long long inv_count = 0;
  inv_count += countSignificantInversions(arr, left, mid);
  inv_count += countSignificantInversions(arr, mid + 1, right);

  vector<int> L(arr.begin() + left, arr.begin() + mid + 1);
  vector<int> R(arr.begin() + mid + 1, arr.begin() + right + 1);
  sort(L.begin(), L.end());
  sort(R.begin(), R.end());

  int j = 0;
  for (int x : L) {
    while (j < R.size() && static_cast<long long>(x) > 2LL * R[j]) j++;
    inv_count += j;
  }

  vector<int> temp;
  merge(L.begin(), L.end(), R.begin(), R.end(), back_inserter(temp));
  for (size_t idx = 0; idx < temp.size(); ++idx) arr[left + idx] = temp[idx];

  return inv_count;
}

long long countInversions(vector<int> arr) {
  return countInversions(arr, 0, static_cast<int>(arr.size()) - 1);
}

long long countSignificantInversions(vector<int> arr) {
  return countSignificantInversions(arr, 0, static_cast<int>(arr.size()) - 1);
}

string formatArray(const vector<int>& arr) {
  stringstream ss;
  ss << "[";
  for (size_t i = 0; i < arr.size(); ++i) {
    if (i > 0) ss << " ";
    ss << arr[i];
  }
  ss << "]";
  return ss.str();
}

void testAlgorithm(bool useRussian) {
  cout << (useRussian ? "=== Тестирование алгоритма ==="
                      : "=== Algorithm Test ===")
       << "\n\n";

  vector<vector<int>> tests = {{1, 3, 4, 2, 5},
                               {5, 3, 2, 4, 1},
                               {1, 2, 3, 4, 5},
                               {5, 4, 3, 2, 1},
                               {10, 1, 8, 3, 6}};

  if (useRussian) {
    // Двухстрочный заголовок для лучшей читаемости
    cout << setw(14) << "Входной массив"
         << " | " << setw(10) << "Обычные"
         << " | " << setw(10) << "Полученные"
         << " | " << setw(16) << "Значимые"
         << " | " << setw(10) << "Полученные" << "\n";
    cout << setw(14) << ""
         << " | " << setw(10) << "инверсии"
         << " | " << setw(10) << "обычные"
         << " | " << setw(16) << "инверсии"
         << " | " << setw(10) << "значимые" << "\n";
    cout << setw(14) << ""
         << " | " << setw(10) << " "
         << " | " << setw(10) << "инверсии"
         << " | " << setw(16) << " "
         << " | " << setw(10) << "инверсии" << "\n";
    cout << string(78, '-') << "\n";
  } else {
    cout << setw(14) << "Input"
         << " | " << setw(10) << "Inversions"
         << " | " << setw(10) << "Got (Inv)"
         << " | " << setw(16) << "Sig Inversions"
         << " | " << setw(10) << "Got (Sig)" << "\n";
    cout << string(78, '-') << "\n";
  }

  for (const auto& test : tests) {
    long long expected_inv = 0, expected_sig_inv = 0;
    for (size_t i = 0; i < test.size(); ++i) {
      for (size_t j = i + 1; j < test.size(); ++j) {
        if (test[i] > test[j]) expected_inv++;
        if (static_cast<long long>(test[i]) > 2LL * test[j]) expected_sig_inv++;
      }
    }

    long long got_inv = countInversions(test);
    long long got_sig_inv = countSignificantInversions(test);

    cout << setw(14) << formatArray(test) << " | " << setw(10) << expected_inv
         << " | " << setw(10) << got_inv << " | " << setw(16)
         << expected_sig_inv << " | " << setw(10) << got_sig_inv << "\n";
  }
}

int main() {
  ios_base::sync_with_stdio(false);
  cin.tie(nullptr);

  SetConsoleToCP1251();

  cout << "Выберите язык вывода:\n";
  cout << "1 — Русский\n";
  cout << "2 — English\n";
  cout << "Ваш выбор (1 или 2): ";

  int choice;
  cin >> choice;
  bool useRussian = (choice == 1);

  cout << "\n";
  testAlgorithm(useRussian);

  return 0;
}
