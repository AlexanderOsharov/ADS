#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

using u64 = uint64_t;

const int MOD1 = 998244353;  // 119 * 2^23 + 1
const int MOD2 = 985661441;  // 235 * 2^22 + 1
const int ROOT1 = 31;
const int ROOT2 = 31;

inline int mul_mod(int a, int b, int mod) { return (int)((u64)a * b % mod); }

int power(int a, int n, int mod) {
  int res = 1;
  while (n) {
    if (n & 1) res = mul_mod(res, a, mod);
    a = mul_mod(a, a, mod);
    n >>= 1;
  }
  return res;
}

void ntt(vector<int>& a, int mod, int root, bool invert) {
  int n = (int)a.size();
  for (int i = 1, j = 0; i < n; ++i) {
    int bit = n >> 1;
    for (; j & bit; bit >>= 1) j ^= bit;
    j ^= bit;
    if (i < j) swap(a[i], a[j]);
  }

  for (int len = 2; len <= n; len <<= 1) {
    int wlen = power(root, (mod - 1) / len, mod);
    if (invert) wlen = power(wlen, mod - 2, mod);
    for (int i = 0; i < n; i += len) {
      int w = 1;
      for (int j = 0; j < len / 2; ++j) {
        int u = a[i + j];
        int v = mul_mod(a[i + j + len / 2], w, mod);
        a[i + j] = u + v < mod ? u + v : u + v - mod;
        a[i + j + len / 2] = u - v >= 0 ? u - v : u - v + mod;
        w = mul_mod(w, wlen, mod);
      }
    }
  }

  if (invert) {
    int n_inv = power(n, mod - 2, mod);
    for (int& x : a) x = mul_mod(x, n_inv, mod);
  }
}

vector<int> str_to_base(const string& s) {
  vector<int> res;
  for (int i = (int)s.size(); i > 0; i -= 4) {
    int start = max(0, i - 4);
    res.push_back(stoi(s.substr(start, i - start)));
  }
  return res;
}

string base_to_str(const vector<int>& a) {
  if (a.empty()) return "0";
  string res = to_string(a.back());
  for (int i = (int)a.size() - 2; i >= 0; --i) {
    string part = to_string(a[i]);
    res += string(4 - (int)part.size(), '0') + part;
  }
  return res;
}

vector<int> multiply_ntt(const vector<int>& a, const vector<int>& b) {
  if (a.empty() || b.empty()) return {0};
  if (a.size() == 1 && a[0] == 0) return {0};
  if (b.size() == 1 && b[0] == 0) return {0};

  int n1 = (int)a.size(), n2 = (int)b.size();
  int n = 1;
  while (n < n1 + n2 - 1) n <<= 1;

  auto solve = [&](int mod, int root) -> vector<int> {
    vector<int> fa = a, fb = b;
    fa.resize(n, 0);
    fb.resize(n, 0);
    ntt(fa, mod, root, false);
    ntt(fb, mod, root, false);
    for (int i = 0; i < n; ++i) fa[i] = mul_mod(fa[i], fb[i], mod);
    ntt(fa, mod, root, true);
    return fa;
  };

  auto A1 = solve(MOD1, ROOT1);
  auto A2 = solve(MOD2, ROOT2);

  vector<u64> c(n);
  const u64 m1 = MOD1;
  const u64 m2 = MOD2;
  const u64 inv_m1_mod_m2 = power(MOD1, MOD2 - 2, MOD2);

  for (int i = 0; i < n; ++i) {
    u64 x1 = A1[i];
    u64 x2 = (A2[i] + m2 - x1 % m2) % m2;
    x2 = (x2 * inv_m1_mod_m2) % m2;
    c[i] = x1 + x2 * m1;
  }

  vector<int> res;
  u64 carry = 0;
  for (int i = 0; i < n || carry; ++i) {
    if (i < n) carry += c[i];
    res.push_back((int)(carry % 10000));
    carry /= 10000;
  }

  while (res.size() > 1 && res.back() == 0) res.pop_back();
  return res;
}

int main() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);

  string s1, s2;
  cin >> s1 >> s2;

  if (s1 == "0" || s2 == "0") {
    cout << "0\n";
    return 0;
  }

  auto a = str_to_base(s1);
  auto b = str_to_base(s2);
  auto res = multiply_ntt(a, b);
  cout << base_to_str(res) << "\n";

  return 0;
}
