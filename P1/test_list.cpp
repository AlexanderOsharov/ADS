#include <cassert>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "list.h"

void test_default_constructor() {
  List list;
  assert(list.empty());
  assert(list.size() == 0);
  assert(list.head == nullptr);
  assert(list.tail == nullptr);
  std::cout << "✓ Default constructor OK\n";
}

void test_vector_constructor() {
  std::vector<int> v = {1, 2, 3};
  List list(v);
  assert(!list.empty());
  assert(list.size() == 3);
  assert(list.front() == 1);
  assert(list.back() == 3);
  std::cout << "✓ Vector constructor OK\n";
}

void test_copy_constructor() {
  std::vector<int> v = {4, 5, 6};
  List list1(v);
  List list2(list1);
  assert(list2.size() == 3);
  assert(list2.front() == 4);
  assert(list2.back() == 6);
  list1.pop_back();
  assert(list1.size() == 2);
  assert(list2.size() == 3);
  std::cout << "✓ Copy constructor OK\n";
}

void test_push_pop() {
  List list;
  list.push_back(10);
  assert(list.size() == 1);
  assert(list.front() == 10);
  assert(list.back() == 10);

  list.push_front(5);
  assert(list.size() == 2);
  assert(list.front() == 5);
  assert(list.back() == 10);

  list.pop_back();
  assert(list.size() == 1);
  assert(list.front() == 5);
  assert(list.back() == 5);

  list.pop_front();
  assert(list.empty());
  std::cout << "✓ Push/pop OK\n";
}

void test_exceptions_on_empty() {
  List list;
  bool caught = false;

  try {
    list.front();
  } catch (const std::runtime_error&) {
    caught = true;
  }
  assert(caught);
  caught = false;

  try {
    list.back();
  } catch (const std::runtime_error&) {
    caught = true;
  }
  assert(caught);
  caught = false;

  try {
    list.pop_front();
  } catch (const std::runtime_error&) {
    caught = true;
  }
  assert(caught);
  caught = false;

  try {
    list.pop_back();
  } catch (const std::runtime_error&) {
    caught = true;
  }
  assert(caught);

  std::cout << "✓ Empty list exceptions OK\n";
}

void test_insert_erase() {
  List list;
  list.push_back(1);
  list.push_back(3);
  Node* node1 = list.head;        // value = 1
  Node* node3 = list.head->next;  // value = 3

  list.insert(node1, 2);  // 1 <-> 2 <-> 3
  assert(list.size() == 3);
  assert(list.head->next->value == 2);
  assert(list.head->next->next->value == 3);

  // Test erase middle
  list.erase(node1->next);  // remove 2
  assert(list.size() == 2);
  assert(list.head->next->value == 3);

  // Test erase head
  list.erase(list.head);  // remove 1
  assert(list.size() == 1);
  assert(list.front() == 3);

  // Test erase tail
  list.erase(list.tail);
  assert(list.empty());

  std::cout << "✓ Insert/erase OK\n";
}

void test_insert_erase_exceptions() {
  List list;
  bool caught = false;

  try {
    list.insert(nullptr, 42);
  } catch (const std::runtime_error&) {
    caught = true;
  }
  assert(caught);
  caught = false;

  list.push_back(1);
  try {
    list.erase(nullptr);
  } catch (const std::runtime_error&) {
    caught = true;
  }
  assert(caught);

  std::cout << "✓ Insert/erase exceptions OK\n";
}

void test_reverse() {
  List list;
  for (int i = 1; i <= 5; ++i) list.push_back(i);  // 1 2 3 4 5
  list.reverse();                                  // 5 4 3 2 1
  std::vector<int> expected = {5, 4, 3, 2, 1};
  Node* cur = list.head;
  for (int val : expected) {
    assert(cur->value == val);
    cur = cur->next;
  }
  assert(list.front() == 5);
  assert(list.back() == 1);
  std::cout << "✓ Reverse OK\n";
}

void test_remove_duplicates() {
  List list;
  std::vector<int> vals = {1, 2, 1, 1, 3, 2, 4};
  for (int v : vals) list.push_back(v);
  list.remove_duplicates();  // should be 1, 2, 3, 4
  std::vector<int> expected = {1, 2, 3, 4};
  assert(list.size() == expected.size());
  Node* cur = list.head;
  for (int val : expected) {
    assert(cur->value == val);
    cur = cur->next;
  }
  std::cout << "✓ Remove duplicates OK\n";
}

void test_replace() {
  List list;
  for (int v : {1, 2, 1, 3}) list.push_back(v);
  list.replace(1, 9);
  std::vector<int> expected = {9, 2, 9, 3};
  Node* cur = list.head;
  for (int val : expected) {
    assert(cur->value == val);
    cur = cur->next;
  }
  list.replace(99, 100);  // no effect
  assert(list.size() == 4);
  std::cout << "✓ Replace OK\n";
}

void test_clear() {
  List list;
  for (int i = 0; i < 10; ++i) list.push_back(i);
  list.clear();
  assert(list.empty());
  assert(list.head == nullptr);
  assert(list.tail == nullptr);
  assert(list.size() == 0);
  std::cout << "✓ Clear OK\n";
}

void test_check_cycle() {
  // Тест 1: пустой список
  {
    List list;
    assert(!list.check_cycle());
  }

  // Тест 2: один элемент — нет цикла
  {
    List list;
    list.push_back(1);
    assert(!list.check_cycle());
  }

  // Тест 3: несколько элементов — нет цикла
  {
    List list;
    for (int i = 1; i <= 5; ++i) list.push_back(i);
    assert(!list.check_cycle());
  }

  // Тест 4: цикл из одного узла
  {
    List list;
    list.push_back(42);
    list.head->next = list.head;  // цикл
    assert(list.check_cycle());
    // Не удаляем — просто выходим из области видимости (утечка, но для теста
    // допустимо) Или: вручную почистим, но без деструктора
  }

  // Тест 5: цикл длины 2
  {
    List list;
    list.push_back(10);
    list.push_back(20);
    // Сделаем: 10 <-> 20, но 20->next = 10 (цикл)
    list.tail->next = list.head;
    assert(list.check_cycle());
  }

  // Тест 6: цикл, начинающийся не с головы (1->2->3->4, и 4->next = 2)
  {
    List list;
    for (int i = 1; i <= 4; ++i) list.push_back(i);
    // Найдём узел со значением 2
    Node* node2 = list.head->next;  // 1->2->3->4
    list.tail->next = node2;        // 4->next = 2 → цикл: 2->3->4->2...
    assert(list.check_cycle());
  }

  // Тест 7: кольцевой список (tail->next = head) — классический цикл
  {
    List list;
    for (int i = 1; i <= 3; ++i) list.push_back(i);
    list.tail->next = list.head;
    assert(list.check_cycle());
  }

  // Тест 8: испорченный указатель в середине (но не цикл) — не должно
  // детектиться как цикл
  {
    List list;
    for (int i = 1; i <= 3; ++i) list.push_back(i);
    // Нарушим next у среднего узла, но не создадим цикл
    list.head->next->next = nullptr;  // обрежем хвост
    list.tail = list.head->next;
    assert(!list.check_cycle());
  }

  std::cout << "✓ Check cycle OK\n";
}

void test_merge() {
  List list1;
  list1.push_back(1);
  list1.push_back(2);

  List list2;
  list2.push_back(3);
  list2.push_back(4);

  list1.merge(list2);
  assert(list1.size() == 4);
  std::vector<int> expected = {1, 2, 3, 4};
  Node* cur = list1.head;
  for (int val : expected) {
    assert(cur->value == val);
    cur = cur->next;
  }
  // list2 must remain unchanged
  assert(list2.size() == 2);
  assert(list2.front() == 3);
  assert(list2.back() == 4);
  std::cout << "✓ Merge OK\n";
}

void test_copy() {
  List list1;
  for (int v : {10, 20, 30}) list1.push_back(v);

  List list2;
  list2.push_back(999);
  list2.copy(list1);

  assert(list2.size() == 3);
  assert(list2.front() == 10);
  assert(list2.back() == 30);

  // Original unchanged
  assert(list1.size() == 3);
  std::cout << "✓ Copy OK\n";
}

void test_destructor_and_memory() {
  {
    List list;
    for (int i = 0; i < 100; ++i) {
      list.push_back(i);
    }
  }
  std::cout << "✓ Destructor/memory OK\n";
}

int main() {
  test_default_constructor();
  test_vector_constructor();
  test_copy_constructor();
  test_push_pop();
  test_exceptions_on_empty();
  test_insert_erase();
  test_insert_erase_exceptions();
  test_reverse();
  test_remove_duplicates();
  test_replace();
  test_clear();
  test_check_cycle();
  test_merge();
  test_copy();
  test_destructor_and_memory();

  std::cout << "\n🎉 All tests passed!\n";
  return 0;
}
