#include <omp.h>

#include <iostream>
#include <unordered_map>
#include <vector>

using ull = unsigned long long;

const ull SACRED_MODULUS_ALPHA = 1000000007ULL;
const ull SACRED_MODULUS_BETA = 1000000009ULL;
const ull MYSTIC_BASE_ALPHA = 131;
const ull MYSTIC_BASE_BETA = 13131;

struct HashTwin {
  ull first_soul, second_soul;
  bool operator==(const HashTwin& other) const {
    return first_soul == other.first_soul && second_soul == other.second_soul;
  }
};

struct HashTwinRitual {
  size_t operator()(const HashTwin& twin) const {
    return twin.first_soul ^ (twin.second_soul << 1);
  }
};

inline ull sacred_multiply(ull a, ull b, ull modulus) {
  return (a * b) % modulus;
}

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  int n;
  std::cin >> n;

  std::vector<ull> original_dancer(n);
  for (int i = 0; i < n; ++i) {
    long long x;
    std::cin >> x;
    x %= (long long)SACRED_MODULUS_ALPHA;
    if (x < 0) x += SACRED_MODULUS_ALPHA;
    original_dancer[i] = static_cast<ull>(x);
  }

  if (n == 1) {
    std::cout << 1;
    return 0;
  }

  std::vector<ull> infinite_dance_floor(2 * n);
  for (int i = 0; i < n; ++i) {
    infinite_dance_floor[i] = infinite_dance_floor[i + n] = original_dancer[i];
  }

  std::vector<ull> power_of_alpha(n + 1, 1), power_of_beta(n + 1, 1);
  for (int i = 1; i <= n; ++i) {
    power_of_alpha[i] = sacred_multiply(
        power_of_alpha[i - 1], MYSTIC_BASE_ALPHA, SACRED_MODULUS_ALPHA);
    power_of_beta[i] = sacred_multiply(power_of_beta[i - 1], MYSTIC_BASE_BETA,
                                       SACRED_MODULUS_BETA);
  }

  std::vector<ull> prefix_alpha(2 * n + 1, 0), prefix_beta(2 * n + 1, 0);
  for (int i = 0; i < 2 * n; ++i) {
    prefix_alpha[i + 1] = (sacred_multiply(prefix_alpha[i], MYSTIC_BASE_ALPHA,
                                           SACRED_MODULUS_ALPHA) +
                           infinite_dance_floor[i]) %
                          SACRED_MODULUS_ALPHA;
    prefix_beta[i + 1] = (sacred_multiply(prefix_beta[i], MYSTIC_BASE_BETA,
                                          SACRED_MODULUS_BETA) +
                          infinite_dance_floor[i]) %
                         SACRED_MODULUS_BETA;
  }

  auto extract_subcycle_hash = [&](int l) -> HashTwin {
    ull hash_alpha = (prefix_alpha[l + n] + SACRED_MODULUS_ALPHA -
                      sacred_multiply(prefix_alpha[l], power_of_alpha[n],
                                      SACRED_MODULUS_ALPHA)) %
                     SACRED_MODULUS_ALPHA;
    ull hash_beta = (prefix_beta[l + n] + SACRED_MODULUS_BETA -
                     sacred_multiply(prefix_beta[l], power_of_beta[n],
                                     SACRED_MODULUS_BETA)) %
                    SACRED_MODULUS_BETA;
    return {hash_alpha, hash_beta};
  };

  std::vector<int> hamming_distance_for_shift(n);
  std::vector<HashTwin> hash_of_shifted_cycle(n);

#pragma omp parallel for schedule(static)
  for (int shift = 0; shift < n; ++shift) {
    int differences = 0;
    for (int i = 0; i < n; ++i) {
      if (original_dancer[i] != original_dancer[(i + shift) % n]) ++differences;
    }
    hamming_distance_for_shift[shift] = differences;
    hash_of_shifted_cycle[shift] = extract_subcycle_hash(shift);
  }

  std::unordered_map<HashTwin, int, HashTwinRitual> unique_dances;
  for (int shift = 0; shift < n; ++shift) {
    unique_dances[hash_of_shifted_cycle[shift]] =
        hamming_distance_for_shift[shift];
  }

  int max_hamming = -1;
  for (const auto& dance : unique_dances) {
    if (dance.second > max_hamming) max_hamming = dance.second;
  }

  int count_of_champions = 0;
  for (const auto& dance : unique_dances) {
    if (dance.second == max_hamming) ++count_of_champions;
  }

  std::cout << count_of_champions;
  return 0;
}
