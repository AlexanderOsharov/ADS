#include <algorithm>
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
using namespace std;

// Очередь на двух стеках с поддержкой быстрого получения минимума.
// Каждое стековое хранилище содержит пары (value, current_min_in_stack).
class MinQueue {
  // стек пар: (value, current_min)
  vector<pair<int, int>> in_stack, out_stack;

 public:
  MinQueue() {}

  void enqueue(int x) {
    int curmin = in_stack.empty() ? x : min(in_stack.back().second, x);
    in_stack.emplace_back(x, curmin);
  }

  void shift_stacks() {
    if (!out_stack.empty()) return;
    while (!in_stack.empty()) {
      int val = in_stack.back().first;
      in_stack.pop_back();
      int curmin = out_stack.empty() ? val : min(out_stack.back().second, val);
      out_stack.emplace_back(val, curmin);
    }
  }

  void dequeue() {
    shift_stacks();
    if (out_stack.empty()) throw runtime_error("dequeue from empty queue");
    out_stack.pop_back();
  }

  int front() {
    shift_stacks();
    if (out_stack.empty()) throw runtime_error("front from empty queue");
    return out_stack.back().first;
  }

  bool empty() const { return in_stack.empty() && out_stack.empty(); }

  int get_min() const {
    if (in_stack.empty() && out_stack.empty())
      throw runtime_error("get_min from empty queue");
    if (in_stack.empty()) return out_stack.back().second;
    if (out_stack.empty()) return in_stack.back().second;
    return min(in_stack.back().second, out_stack.back().second);
  }
};

// Тесты для MinQueue
void run_tests() {
  MinQueue q;
  // простой тест
  q.enqueue(3);
  q.enqueue(1);
  q.enqueue(5);
  assert(q.get_min() == 1);
  q.dequeue();  // removes 3
  assert(q.get_min() == 1);
  q.dequeue();  // removes 1
  assert(q.get_min() == 5);
  q.enqueue(2);
  assert(q.get_min() == 2 || q.get_min() == 2);  // min among [5,2]
  q.dequeue();                                   // removes 5
  assert(q.get_min() == 2);
  q.dequeue();  // removes 2
  assert(q.empty());

  // чередование операций
  q.enqueue(4);
  q.enqueue(4);
  q.enqueue(1);
  q.dequeue();  // remove first 4
  assert(q.get_min() == 1);
  q.enqueue(0);
  assert(q.get_min() == 0);
  q.dequeue();
  q.dequeue();
  assert(q.get_min() == 0);
  q.dequeue();
  assert(q.empty());

  cerr << "All tests passed\\n";
}

int main() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);

  // Запускаем тесты
  run_tests();

  // Демонстрация: читаем команды из stdin (optional)
  // Формат команд (пример):
  // push x
  // pop
  // min
  // front
  // exit
  MinQueue q;
  string cmd;
  while (cin >> cmd) {
    if (cmd == "push") {
      int x;
      cin >> x;
      q.enqueue(x);
      cout << "ok\n";
    } else if (cmd == "pop") {
      if (q.empty())
        cout << "error\n";
      else {
        cout << q.front() << "\n";
        q.dequeue();
      }
    } else if (cmd == "min") {
      if (q.empty())
        cout << "error\n";
      else
        cout << q.get_min() << "\n";
    } else if (cmd == "front") {
      if (q.empty())
        cout << "error\n";
      else
        cout << q.front() << "\n";
    } else if (cmd == "exit") {
      cout << "bye\n";
      break;
    } else {
      cout << "unknown command\n";
    }
  }

  return 0;
}
