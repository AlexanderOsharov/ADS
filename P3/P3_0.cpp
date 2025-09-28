#include <cctype>
#include <iostream>
#include <string>
#include <vector>

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  std::string s;
  std::getline(std::cin, s);

  std::string building;
  std::vector<std::pair<std::string, int>> decoder_stack;
  decoder_stack.reserve(100);

  for (char c : s) {
    if (std::isdigit(static_cast<unsigned char>(c))) {
      int repeat = c - '0';

      decoder_stack.emplace_back(std::move(building), repeat);
      building.clear();
    } else if (c == ']') {
      std::string prefix = std::move(decoder_stack.back().first);
      int times = decoder_stack.back().second;

      decoder_stack.pop_back();

      if (times > 0) {
        size_t total =
            prefix.size() + building.size() * static_cast<size_t>(times);

        prefix.reserve(total);

        for (int i = 0; i < times; ++i) {
          prefix += building;
        }
      }

      building = std::move(prefix);
    } else if (c != '[') {
      building += c;
    }
  }

  std::cout << building;
  return 0;
}
