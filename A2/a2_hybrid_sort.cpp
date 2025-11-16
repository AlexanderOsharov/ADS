#include <iostream>
#include <vector>

using namespace std;

void merge_arr(vector<int>& v, int l, int m, int r) {
  int sz1 = m - l + 1;
  int sz2 = r - m;
  vector<int> tmp1(sz1), tmp2(sz2);

  for (int i = 0; i < sz1; ++i) tmp1[i] = v[l + i];
  for (int i = 0; i < sz2; ++i) tmp2[i] = v[m + 1 + i];

  int i = 0, j = 0, k = l;
  while (i < sz1 && j < sz2) {
    v[k++] = (tmp1[i] <= tmp2[j]) ? tmp1[i++] : tmp2[j++];
  }
  while (i < sz1) v[k++] = tmp1[i++];
  while (j < sz2) v[k++] = tmp2[j++];
}

void merge_sort_rec(vector<int>& v, int l, int r) {
  if (l >= r) return;
  int mid = l + (r - l) / 2;
  merge_sort_rec(v, l, mid);
  merge_sort_rec(v, mid + 1, r);
  merge_arr(v, l, mid, r);
}

void insert_sort(vector<int>& v, int l, int r) {
  for (int i = l + 1; i <= r; ++i) {
    int val = v[i], pos = i - 1;
    while (pos >= l && v[pos] > val) {
      v[pos + 1] = v[pos];
      --pos;
    }
    v[pos + 1] = val;
  }
}

void hybrid_sort(vector<int>& v, int l, int r, int cutoff) {
  if (r - l + 1 <= cutoff) {
    insert_sort(v, l, r);
  } else if (l < r) {
    int mid = l + (r - l) / 2;
    merge_sort_rec(v, l, mid);
    merge_sort_rec(v, mid + 1, r);
    merge_arr(v, l, mid, r);
  }
}

int main() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);

  int n;
  if (!(cin >> n)) return 0;

  vector<int> data(n);
  for (int i = 0; i < n; ++i) {
    cin >> data[i];
  }

  hybrid_sort(data, 0, n - 1, 18);

  for (int i = 0; i < n; ++i) {
    cout << data[i] << (i + 1 < n ? " " : "\n");
  }
  return 0;
}
