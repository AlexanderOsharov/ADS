#include <iostream>
#include <vector>

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  int n;
  std::cin >> n;

  std::vector<int> incoming(n);
  for (int i = 0; i < n; ++i) {
    std::cin >> incoming[i];
  }

  std::vector<std::pair<int, int>> train_log;
  std::vector<int> siding;
  int next_incoming = 0;
  int next_expected = 1;

  while (next_expected <= n) {
    if (!siding.empty() && siding.back() == next_expected) {
      int popped = 0;

      while (!siding.empty() && siding.back() == next_expected) {
        siding.pop_back();
        ++popped;
        ++next_expected;
      }

      train_log.push_back({2, popped});
    } else if (next_incoming < n) {
      int pushed = 0;

      while (next_incoming < n) {
        siding.push_back(incoming[next_incoming]);
        ++next_incoming;
        ++pushed;

        if (!siding.empty() && siding.back() == next_expected) {
          break;
        }
      }

      train_log.push_back({1, pushed});
    } else {
      break;
    }
  }

  if (next_expected > n) {
    for (const auto& op : train_log) {
      std::cout << op.first << ' ' << op.second << '\n';
    }
  } else {
    std::cout << 0;
  }

  return 0;
}
