#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <cmath>

using namespace std;

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
  int left = 2 * i + 1;
  int right = 2 * i + 2;

  if (left < n && arr[left] > arr[largest])
    largest = left;

  if (right < n && arr[right] > arr[largest])
    largest = right;

  if (largest != i) {
    swap(arr[i], arr[largest]);
    heapify(arr, n, largest);
  }
}

void heapSort(vector<int>& arr, int left, int right) {
  int n = right - left + 1;
  vector<int> temp(arr.begin() + left, arr.begin() + right + 1);

  for (int i = n / 2 - 1; i >= 0; --i)
    heapify(temp, n, i);

  for (int i = n - 1; i > 0; --i) {
    swap(temp[0], temp[i]);
    heapify(temp, i, 0);
  }

  for (int i = 0; i < n; ++i) {
    arr[left + i] = temp[i];
  }
}

int partition(vector<int>& arr, int low, int high) {
  random_device rd;
  static mt19937 gen(rd());
  uniform_int_distribution<> dis(low, high);
  int random_idx = dis(gen);
  swap(arr[random_idx], arr[high]);

  int pivot = arr[high];
  int i = low - 1;

  for (int j = low; j < high; ++j) {
    if (arr[j] <= pivot) {
      ++i;
      swap(arr[i], arr[j]);
    }
  }
  swap(arr[i + 1], arr[high]);
  return i + 1;
}

void quickSortImpl(vector<int>& arr, int left, int right, int depth_limit) {
  while (left < right) {
    if (right - left + 1 < 16) {
      insertionSort(arr, left, right);
      return;
    }

    if (depth_limit == 0) {
      heapSort(arr, left, right);
      return;
    }

    int pi = partition(arr, left, right);

    if (pi - left < right - pi) {
      quickSortImpl(arr, left, pi - 1, depth_limit - 1);
      left = pi + 1;
    }
    else {
      quickSortImpl(arr, pi + 1, right, depth_limit - 1);
      right = pi - 1;
    }
  }
}

void quickSort(vector<int>& arr, int left, int right) {
  if (left < right) {
    int depth_limit = 2 * static_cast<int>(log2(right - left + 1));
    quickSortImpl(arr, left, right, depth_limit);
  }
}

int main() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);

  int n;
  cin >> n;

  if (n == 0) {
    cout << "\n";
    return 0;
  }

  vector<int> data(n);
  for (int i = 0; i < n; ++i) {
    cin >> data[i];
  }

  quickSort(data, 0, n - 1);

  for (int i = 0; i < n; ++i) {
    cout << data[i] << (i + 1 < n ? " " : "\n");
  }

  return 0;
}