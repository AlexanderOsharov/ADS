#include <windows.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

using namespace std;
using namespace std::chrono;

// ---------- КОНФИГУРАЦИЯ ----------
struct SieveConfig {
  bool use_delta = false;        // false = обычный список
  bool show_progress = true;     // показывать прогресс-бар
  int64_t block_size = 1 << 20;  // ~1M чисел на блок
  bool use_wheel = true;         // всегда true в этой реализации
};
SieveConfig config;

// ---------- КОЛЕСО W(30) ----------
constexpr int WHEEL_MOD = 30;
constexpr int WHEEL_REMAINDERS[8] = {1, 7, 11, 13, 17, 19, 23, 29};
constexpr int WHEEL_SIZE = 8;

// Lookup-таблица: остаток mod 30 → индекс в колесе (-1, если делится на 2,3,5)
constexpr int REMAINDER_TO_INDEX[30] = {-1, 0,  -1, -1, -1, -1, -1, 1,  -1, -1,
                                        -1, 2,  -1, 3,  -1, -1, -1, 4,  -1, 5,
                                        -1, -1, -1, 6,  -1, -1, -1, -1, -1, 7};

// ---------- ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ ----------
int64_t integer_sqrt(int64_t n) {
  if (n <= 1) return n;
  int64_t x = static_cast<int64_t>(sqrt((long double)n));
  while (x * x > n) --x;
  while ((x + 1) * (x + 1) <= n) ++x;
  return x;
}

double prime_count_estimate(int64_t n) {
  if (n < 2) return 0.0;
  if (n < 100) return 25;                 // грубая заглушка
  return n / (log((double)n) - 1.08366);  // более точная оценка
}

// ---------- ПРОСТОЕ РЕШЕТО ДЛЯ БАЗОВЫХ ПРОСТЫХ (ДО √N) ----------
vector<int64_t> simple_sieve_base(int64_t limit) {
  if (limit < 2) return {};
  vector<int64_t> primes;
  primes.reserve(static_cast<size_t>(prime_count_estimate(limit) * 1.1));

  if (limit >= 2) primes.push_back(2);
  if (limit >= 3) primes.push_back(3);
  if (limit >= 5) primes.push_back(5);
  if (limit < 7) return primes;

  // Битовая карта для колеса W(30)
  int64_t num_wheels = (limit + WHEEL_MOD) / WHEEL_MOD;
  size_t total_bits = num_wheels * WHEEL_SIZE;
  vector<uint8_t> is_prime((total_bits + 7) / 8, 0xFF);

  auto set_bit = [&](size_t idx, bool value) {
    size_t byte = idx >> 3;
    size_t bit = idx & 7;
    if (value)
      is_prime[byte] |= (1 << bit);
    else
      is_prime[byte] &= ~(1 << bit);
  };

  auto get_bit = [&](size_t idx) -> bool {
    size_t byte = idx >> 3;
    size_t bit = idx & 7;
    return is_prime[byte] & (1 << bit);
  };

  auto to_number = [&](size_t idx) -> int64_t {
    return (idx / WHEEL_SIZE) * WHEEL_MOD + WHEEL_REMAINDERS[idx % WHEEL_SIZE];
  };

  for (size_t idx = 0; idx < total_bits; ++idx) {
    int64_t p = to_number(idx);
    if (p > limit) break;
    if (get_bit(idx)) {
      primes.push_back(p);
      if (p <= limit / p) {
        for (int64_t mult = p * p; mult <= limit; mult += p) {
          int r = mult % WHEEL_MOD;
          int offset = REMAINDER_TO_INDEX[r];
          if (offset == -1) continue;
          int64_t wheel = mult / WHEEL_MOD;
          size_t j = wheel * WHEEL_SIZE + offset;
          if (j < total_bits) set_bit(j, false);
        }
      }
    }
  }
  return primes;
}

// ---------- СТРУКТУРА БЛОКА ----------
struct BlockRange {
  int64_t low, high;
};

// ---------- ОБРАБОТКА БЛОКА С КОЛЕСОМ (БЕЗ % И lower_bound) ----------
vector<int64_t> sieve_block_wheel(int64_t low, int64_t high,
                                  const vector<int64_t>& base_primes) {
  if (low > high || high < 7) return {};

  // Выравниваем low вниз до кратного 30
  int64_t block_start = (low / WHEEL_MOD) * WHEEL_MOD;
  if (block_start < low) block_start += WHEEL_MOD;

  // Но лучше: начать с первого полного колеса <= low
  block_start = (low / WHEEL_MOD) * WHEEL_MOD;
  int64_t block_end = ((high + WHEEL_MOD - 1) / WHEEL_MOD) * WHEEL_MOD;

  int64_t num_wheels = (block_end - block_start) / WHEEL_MOD;
  if (num_wheels <= 0) num_wheels = 1;

  size_t total_bits = num_wheels * WHEEL_SIZE;
  vector<uint8_t> sieve((total_bits + 7) / 8, 0xFF);

  auto set_composite = [&](size_t idx) {
    size_t byte = idx >> 3;
    size_t bit = idx & 7;
    if (byte < sieve.size()) {
      sieve[byte] &= ~(1 << bit);
    }
  };

  auto is_prime_bit = [&](size_t idx) -> bool {
    size_t byte = idx >> 3;
    size_t bit = idx & 7;
    if (byte >= sieve.size()) return false;
    return sieve[byte] & (1 << bit);
  };

  // Пометка кратных
  for (int64_t p : base_primes) {
    if (p < 7) continue;
    if (p > high / p) break;

    // Найти первое кратное p >= low
    int64_t start = ((low + p - 1) / p) * p;
    if (start < p * p) start = p * p;
    if (start > high) continue;

    // Пройдём по всем кратным p в [low, high]
    for (int64_t mult = start; mult <= high; mult += p) {
      int r = mult % WHEEL_MOD;
      int offset = REMAINDER_TO_INDEX[r];
      if (offset == -1) continue;  // делится на 2,3 или 5

      int64_t wheel = (mult - block_start) / WHEEL_MOD;
      if (wheel < 0 || wheel >= num_wheels) continue;

      size_t idx = wheel * WHEEL_SIZE + offset;
      set_composite(idx);
    }
  }

  // Сбор простых
  vector<int64_t> primes;
  for (int64_t wheel = 0; wheel < num_wheels; ++wheel) {
    int64_t base = block_start + wheel * WHEEL_MOD;
    if (base + 29 < low) continue;
    if (base > high) break;

    for (int i = 0; i < WHEEL_SIZE; ++i) {
      int64_t num = base + WHEEL_REMAINDERS[i];
      if (num < low || num > high) continue;
      size_t idx = wheel * WHEEL_SIZE + i;
      if (is_prime_bit(idx)) {
        primes.push_back(num);
      }
    }
  }
  return primes;
}

// ---------- СЕГМЕНТИРОВАННОЕ РЕШЕТО С УЛУЧШЕННЫМ ПРОГРЕСС-БАРОМ ----------
vector<int64_t> segmented_sieve_wheel_mt(int64_t N, const SieveConfig& config) {
  if (N < 2) return {};
  int64_t sqrtN = integer_sqrt(N);
  auto base_primes = simple_sieve_base(sqrtN);

  vector<int64_t> all_primes;
  double estimate = prime_count_estimate(N);
  all_primes.reserve(static_cast<size_t>(estimate * 1.05));

  // Добавляем малые простые
  if (N >= 2) all_primes.push_back(2);
  if (N >= 3) all_primes.push_back(3);
  if (N >= 5) all_primes.push_back(5);

  vector<BlockRange> blocks;
  for (int64_t low = 7; low <= N; low += config.block_size) {
    blocks.push_back({low, min(low + config.block_size - 1, N)});
  }

  if (blocks.empty()) {
    return all_primes;
  }

  size_t total_blocks = blocks.size();
  atomic<size_t> completed_blocks{0};

  // Предварительная оценка общего числа простых (для lock-free output)
  size_t total_primes_est = static_cast<size_t>(estimate);
  vector<int64_t> output(total_primes_est + 10);
  atomic<size_t> output_index{all_primes.size()};
  copy(all_primes.begin(), all_primes.end(), output.begin());

  mutex blocks_mutex;
  size_t block_index = 0;

  auto worker = [&](size_t /*tid*/) {
    while (true) {
      BlockRange range;
      {
        lock_guard<mutex> lock(blocks_mutex);
        if (block_index >= total_blocks) break;
        range = blocks[block_index++];
      }
      auto primes = sieve_block_wheel(range.low, range.high, base_primes);
      if (!primes.empty()) {
        size_t start_idx = output_index.fetch_add(primes.size());
        if (start_idx + primes.size() <= output.size()) {
          copy(primes.begin(), primes.end(), output.begin() + start_idx);
        }
        // Если не хватило места — игнорируем (редко)
      }
      completed_blocks++;
    }
  };

  size_t num_threads = thread::hardware_concurrency();
  vector<thread> workers;
  for (size_t i = 0; i < num_threads; ++i) {
    workers.emplace_back(worker, i);
  }

  // Надёжный прогресс-бар
  thread progress_thread;
  if (config.show_progress && total_blocks > 1) {
    progress_thread = thread([&]() {
      size_t last_done = 0;
      while (true) {
        this_thread::sleep_for(milliseconds(500));
        size_t done = completed_blocks.load();
        if (done != last_done || done == total_blocks) {
          int percent = static_cast<int>((100.0 * done) / total_blocks);
          cerr << "\rПрогресс: [" << string(percent / 2, '=')
               << string(50 - percent / 2, ' ') << "] " << percent << "% ("
               << done << "/" << total_blocks << " блоков)" << flush;
          last_done = done;
        }
        if (done >= total_blocks) {
          cerr << "\n";
          break;
        }
      }
    });
  }

  for (auto& w : workers) w.join();
  if (progress_thread.joinable()) {
    progress_thread.join();
  }

  // Обрезаем до реального размера
  size_t final_size = output_index.load();
  output.resize(final_size);
  sort(output.begin() + (N >= 5 ? 3 : (N >= 3 ? 2 : (N >= 2 ? 1 : 0))),
       output.end());
  return output;
}

// ---------- ПРОСТОЕ РЕШЕТО ДЛЯ МАЛЫХ N ----------
vector<int64_t> simple_sieve_odd(int64_t limit) {
  if (limit < 2) return {};
  vector<int64_t> primes;
  primes.push_back(2);
  if (limit < 3) return primes;

  int64_t size = (limit - 1) / 2;
  vector<char> is_prime(size, true);

  for (int64_t i = 0; i < size; ++i) {
    if (is_prime[i]) {
      int64_t p = 2 * i + 3;
      primes.push_back(p);
      if (p <= limit / p) {
        for (int64_t j = (p * p - 3) / 2; j < size; j += p) {
          is_prime[j] = false;
        }
      }
    }
  }
  return primes;
}

// ---------- ГЕНЕРАЦИЯ ПРОСТЫХ ----------
vector<int64_t> generate_primes(int64_t N, const SieveConfig& config) {
  if (N <= 1'000'000) {
    return simple_sieve_odd(N);
  }
  return segmented_sieve_wheel_mt(N, config);
}

// ---------- ЗАПИСЬ В ФАЙЛ ----------
void write_primes_to_file(const vector<int64_t>& primes, const string& filename,
                          bool use_delta) {
  ofstream out(filename);
  if (!out) {
    cerr << "Ошибка: не удалось создать файл " << filename << "\n";
    return;
  }

  if (!use_delta) {
    for (int64_t p : primes) {
      out << p << '\n';
    }
  } else {
    if (primes.empty()) return;
    out << primes[0] << '\n';
    for (size_t i = 1; i < primes.size(); ++i) {
      out << (primes[i] - primes[i - 1]) << '\n';
    }
  }
  out.close();
}

// ---------- ОЦЕНКА ПАМЯТИ ----------
size_t estimate_memory_usage(const vector<int64_t>& primes, int64_t N) {
  size_t mem = primes.capacity() * sizeof(int64_t);
  if (N > 1'000'000) {
    int64_t sqrtN = integer_sqrt(N);
    mem += static_cast<size_t>(prime_count_estimate(sqrtN)) * sizeof(int64_t);
    mem += 2 * config.block_size / 4;  // приблизительно
  }
  return mem;
}

// ---------- ЛОКАЛИЗАЦИЯ ----------
void SetConsoleToCP1251() {
  SetConsoleOutputCP(1251);
  SetConsoleCP(1251);
}

void SwitchToRussianLayout() {
  HKL russianLayout = LoadKeyboardLayout(TEXT("00000419"), KLF_SETFORPROCESS);
  if (russianLayout == NULL) {
    std::cerr << "Не удалось загрузить русскую раскладку.\n";
  }
}

// ---------- MAIN ----------
int main() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);
  SetConsoleToCP1251();
  SwitchToRussianLayout();

  int64_t N;
  cout << "Введите N (макс. число): ";
  if (!(cin >> N) || N <= 0) {
    cerr << "Ошибка: введите положительное целое число.\n";
    return 1;
  }

  config.use_delta = true;  // ← измените на true, если нужно delta
  config.show_progress = true;
  config.block_size = 1 << 20;  // 1M чисел на блок

  auto start_time = steady_clock::now();
  vector<int64_t> primes = generate_primes(N, config);
  auto end_time = steady_clock::now();

  double elapsed_sec = duration<double>(end_time - start_time).count();
  size_t mem_bytes =
      primes.capacity() * sizeof(int64_t) + 10'000'000;  // грубая оценка

  write_primes_to_file(primes, "primes.txt", config.use_delta);

  cout << "\n✅ Готово!\n";
  cout << "Найдено простых чисел: " << primes.size() << "\n";
  cout << "Время выполнения: " << fixed << setprecision(3) << elapsed_sec
       << " сек\n";
  cout << "Потребление памяти (оценка): " << mem_bytes / (1024.0 * 1024.0)
       << " МБ\n";
  cout << "Результат сохранён в: primes.txt\n";
  if (config.use_delta) {
    cout << "(Формат: delta-кодирование)\n";
  }
  cout << "Первые 10 простых: ";
  for (size_t i = 0; i < min(size_t(10), primes.size()); ++i) {
    cout << primes[i] << " ";
  }
  cout << "\n";

  return 0;
}
