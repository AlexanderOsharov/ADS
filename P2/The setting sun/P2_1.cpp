#include <algorithm>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

using u32 = uint32_t;
using u64 = uint64_t;
using i64 = int64_t;

// ========================
// ¬—œŒÃŒ√¿“≈À‹Õ€≈ ‘”Õ ÷»»
// ========================

void add_bits(vector<u32>& a, const vector<u32>& b) {
  u64 carry = 0;
  size_t n = max(a.size(), b.size());
  a.resize(n, 0);
  for (size_t i = 0; i < n; ++i) {
    u64 s = a[i] + (i < b.size() ? b[i] : 0) + carry;
    a[i] = static_cast<u32>(s);
    carry = s >> 32;
  }
  while (carry) {
    a.push_back(static_cast<u32>(carry));
    carry >>= 32;
  }
}

void sub_bits(vector<u32>& a, const vector<u32>& b) {
  i64 borrow = 0;
  for (size_t i = 0; i < b.size(); ++i) {
    i64 d = static_cast<i64>(a[i]) - static_cast<i64>(b[i]) - borrow;
    if (d < 0) {
      a[i] = static_cast<u32>(d + (1LL << 32));
      borrow = 1;
    } else {
      a[i] = static_cast<u32>(d);
      borrow = 0;
    }
  }
  for (size_t i = b.size(); borrow && i < a.size(); ++i) {
    i64 d = static_cast<i64>(a[i]) - borrow;
    if (d < 0) {
      a[i] = static_cast<u32>(d + (1LL << 32));
      borrow = 1;
    } else {
      a[i] = static_cast<u32>(d);
      borrow = 0;
    }
  }
  while (!a.empty() && a.back() == 0) a.pop_back();
}

void lshift_bits(vector<u32>& a, int k) {
  if (k <= 0 || a.empty()) return;
  int word_shift = k / 32;
  int bit_shift = k % 32;
  vector<u32> res(word_shift, 0);
  u64 carry = 0;
  for (u32 x : a) {
    u64 val = (static_cast<u64>(x) << bit_shift) | carry;
    res.push_back(static_cast<u32>(val));
    carry = val >> 32;
  }
  if (carry) res.push_back(static_cast<u32>(carry));
  a = res;
}

void rshift_bits(vector<u32>& a, int k) {
  if (k <= 0 || a.empty()) return;
  int word_shift = k / 32;
  int bit_shift = k % 32;
  if (word_shift >= static_cast<int>(a.size())) {
    a.clear();
    return;
  }
  vector<u32> res;
  u64 carry = 0;

  for (int i = static_cast<int>(a.size()) - 1; i >= word_shift; --i) {
    u64 val = static_cast<u64>(a[i]) | carry;
    res.push_back(static_cast<u32>(val >> bit_shift));
    carry = (val << (32 - bit_shift)) & 0xFFFFFFFFu;
  }
  reverse(res.begin(), res.end());
  while (!res.empty() && res.back() == 0) res.pop_back();
  a = res;
}

vector<u32> naive_mul(const vector<u32>& a, const vector<u32>& b) {
  if (a.empty() || b.empty()) return {};
  vector<u64> tmp(a.size() + b.size(), 0);
  for (size_t i = 0; i < a.size(); ++i)
    for (size_t j = 0; j < b.size(); ++j)
      tmp[i + j] += static_cast<u64>(a[i]) * b[j];
  u64 carry = 0;
  vector<u32> res;
  for (u64 x : tmp) {
    x += carry;
    res.push_back(static_cast<u32>(x));
    carry = x >> 32;
  }
  while (carry) {
    res.push_back(static_cast<u32>(carry));
    carry >>= 32;
  }
  while (!res.empty() && res.back() == 0) res.pop_back();
  return res;
}

// ========================
// –≈ƒ” ÷»ﬂ œŒ ÃŒƒ”Àﬁ (2^n + 1)
// ========================

void reduce_mod_2n_plus_1(vector<u32>& x, int n) {
  if (x.empty()) return;
  int total_bits = static_cast<int>(x.size()) * 32;
  if (total_bits <= n) return;

  vector<u32> x_low = x;
  int word_n = n / 32;
  int bit_n = n % 32;
  if (word_n < static_cast<int>(x_low.size())) {
    u32 mask = (bit_n == 0) ? 0xFFFFFFFFu : (1U << bit_n) - 1;
    x_low[word_n] &= mask;
    x_low.resize(word_n + 1);
  }

  vector<u32> x_high = x;
  rshift_bits(x_high, n);

  if (x_high.empty()) {
    x = x_low;
    return;
  }

  bool need_add = false;
  if (x_low.size() < x_high.size()) {
    need_add = true;
  } else if (x_low.size() == x_high.size()) {
    for (int i = static_cast<int>(x_low.size()) - 1; i >= 0; --i) {
      if (x_low[i] != x_high[i]) {
        need_add = (x_low[i] < x_high[i]);
        break;
      }
    }
  }

  sub_bits(x_low, x_high);

  if (need_add) {
    int w = n / 32;
    int b = n % 32;
    x_low.resize(max(static_cast<size_t>(w + 1), x_low.size()), 0);
    u64 carry = static_cast<u64>(1) << b;
    for (int i = w; carry && i < static_cast<int>(x_low.size()); ++i) {
      u64 val = static_cast<u64>(x_low[i]) + carry;
      x_low[i] = static_cast<u32>(val);
      carry = val >> 32;
    }
    if (carry) x_low.push_back(static_cast<u32>(carry));

    carry = 1;
    for (u32& word : x_low) {
      u64 val = static_cast<u64>(word) + carry;
      word = static_cast<u32>(val);
      carry = val >> 32;
      if (!carry) break;
    }
    if (carry) x_low.push_back(static_cast<u32>(carry));
  }

  x = x_low;
  while (!x.empty() && x.back() == 0) x.pop_back();
}

// ========================
// FFT ¬  ŒÀ‹÷≈ Z/(2^n + 1)Z
// ========================

void fft_fermat(vector<vector<u32>>& a, int n, bool invert) {
  int m = static_cast<int>(a.size());
  if (m <= 1) return;

  int levels = 0;
  while ((1 << levels) < m) ++levels;

  for (int i = 0; i < m; ++i) {
    int rev = 0;
    for (int j = 0; j < levels; ++j)
      if (i & (1 << j)) rev |= 1 << (levels - 1 - j);
    if (i < rev) swap(a[i], a[rev]);
  }

  for (int len = 1; len < m; len <<= 1) {
    int step = n / (2 * len);
    for (int i = 0; i < m; i += 2 * len) {
      for (int j = 0; j < len; ++j) {
        auto& u = a[i + j];
        auto& v = a[i + j + len];

        auto v_shifted = v;
        lshift_bits(v_shifted, step);
        reduce_mod_2n_plus_1(v_shifted, n);

        auto a_new = u;
        add_bits(a_new, v_shifted);
        reduce_mod_2n_plus_1(a_new, n);

        auto b_new = u;
        sub_bits(b_new, v_shifted);
        reduce_mod_2n_plus_1(b_new, n);

        u = a_new;
        v = b_new;
      }
    }
  }

  if (invert) {
    for (int iter = 0; iter < levels; ++iter) {
      for (auto& x : a) {
        bool odd = (!x.empty() && (x[0] & 1));
        if (odd) {
          int w = n / 32;
          int b = n % 32;
          x.resize(max(static_cast<size_t>(w + 1), x.size()), 0);
          u64 carry = static_cast<u64>(1) << b;
          for (int i = w; carry && i < static_cast<int>(x.size()); ++i) {
            u64 val = static_cast<u64>(x[i]) + carry;
            x[i] = static_cast<u32>(val);
            carry = val >> 32;
          }
          if (carry) x.push_back(static_cast<u32>(carry));
          carry = 1;
          for (u32& word : x) {
            u64 val = static_cast<u64>(word) + carry;
            word = static_cast<u32>(val);
            carry = val >> 32;
            if (!carry) break;
          }
          if (carry) x.push_back(static_cast<u32>(carry));
        }

        u64 carry = 0;
        for (int i = static_cast<int>(x.size()) - 1; i >= 0; --i) {
          u64 val = static_cast<u64>(x[i]) | (carry << 32);
          x[i] = static_cast<u32>(val >> 1);
          carry = val & 1;
        }
        while (!x.empty() && x.back() == 0) x.pop_back();
      }
    }
  }
}

// ========================
// Œ—ÕŒ¬ÕŒ… ¿À√Œ–»“Ã
// ========================

vector<u32> schoenhage_strassen_mul(const vector<u32>& A, const vector<u32>& B);

vector<u32> multiply_in_ring(const vector<u32>& A, const vector<u32>& B,
                             int n) {
  int bits_A = static_cast<int>(A.size()) * 32;
  int bits_B = static_cast<int>(B.size()) * 32;
  if (bits_A == 0 || bits_B == 0) return {};

  int coeff_bits = (n + 1) / 2;
  int m = 1;
  while (m * coeff_bits < max(bits_A, bits_B)) m <<= 1;
  m <<= 1;

  auto split = [&](const vector<u32>& X) -> vector<vector<u32>> {
    vector<vector<u32>> res(m);
    vector<u32> temp = X;
    for (int i = 0; i < m; ++i) {
      if (temp.empty()) break;
      res[i] = temp;
      if (static_cast<int>(temp.size()) * 32 > coeff_bits) {
        int ws = coeff_bits / 32;
        int bs = coeff_bits % 32;
        if (ws < static_cast<int>(res[i].size())) {
          u32 mask = (bs == 0) ? 0xFFFFFFFFu : (1U << bs) - 1;
          res[i][ws] &= mask;
          res[i].resize(ws + 1);
        }
        rshift_bits(temp, coeff_bits);
      } else {
        temp.clear();
      }
      while (!res[i].empty() && res[i].back() == 0) res[i].pop_back();
    }
    return res;
  };

  auto polyA = split(A);
  auto polyB = split(B);

  fft_fermat(polyA, n, false);
  fft_fermat(polyB, n, false);

  vector<vector<u32>> polyC(m);
  for (int i = 0; i < m; ++i) {
    polyC[i] = schoenhage_strassen_mul(polyA[i], polyB[i]);
    reduce_mod_2n_plus_1(polyC[i], n);
  }

  fft_fermat(polyC, n, true);

  vector<u32> result;
  int shift = 0;
  for (int i = 0; i < m; ++i) {
    if (!polyC[i].empty()) {
      auto term = polyC[i];
      lshift_bits(term, shift);
      add_bits(result, term);
    }
    shift += coeff_bits;
  }
  reduce_mod_2n_plus_1(result, n);
  return result;
}

vector<u32> schoenhage_strassen_mul(const vector<u32>& A,
                                    const vector<u32>& B) {
  if (A.empty() || B.empty()) return {};
  int bits_A = static_cast<int>(A.size()) * 32;
  int bits_B = static_cast<int>(B.size()) * 32;
  const int CUTOFF_BITS = 256;  // ~77 ‰ÂÒˇÚË˜Ì˚ı ˆËÙ
  if (bits_A + bits_B <= CUTOFF_BITS) {
    return naive_mul(A, B);
  }

  int n = 1;
  while (n < bits_A + bits_B) n <<= 1;

  return multiply_in_ring(A, B, n);
}

// ========================
//  ŒÕ¬≈–“¿÷»ﬂ
// ========================

vector<u32> dec_to_bits(const string& s) {
  if (s == "0") return {};
  vector<u32> res = {0};
  for (char c : s) {
    u64 carry = c - '0';
    for (u32& word : res) {
      u64 val = static_cast<u64>(word) * 10 + carry;
      word = static_cast<u32>(val);
      carry = val >> 32;
    }
    while (carry) {
      res.push_back(static_cast<u32>(carry));
      carry >>= 32;
    }
  }
  while (!res.empty() && res.back() == 0) res.pop_back();
  return res;
}

string bits_to_dec(const vector<u32>& a) {
  if (a.empty()) return "0";
  vector<u32> x = a;
  string res;
  const u64 BASE = 1000000000ULL;  // 10^9
  while (!x.empty()) {
    u64 rem = 0;
    for (int i = static_cast<int>(x.size()) - 1; i >= 0; --i) {
      u64 cur = (rem << 32) | x[i];
      x[i] = static_cast<u32>(cur / BASE);
      rem = cur % BASE;
    }
    while (!x.empty() && x.back() == 0) x.pop_back();
    string part = to_string(static_cast<long long>(rem));
    if (!x.empty()) {
      part = string(9 - part.size(), '0') + part;
    }
    res = part + res;
  }
  return res;
}

// ========================
// MAIN
// ========================

int main() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);

  string s1, s2;
  cin >> s1 >> s2;

  if (s1 == "0" || s2 == "0") {
    cout << "0\n";
    return 0;
  }

  auto a = dec_to_bits(s1);
  auto b = dec_to_bits(s2);
  auto res = schoenhage_strassen_mul(a, b);
  cout << bits_to_dec(res) << "\n";

  return 0;
}
