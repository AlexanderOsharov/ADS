#include <stdexcept>
#include <unordered_set>

#include "list.h"

List::List() {
  head = tail = nullptr;
  _size = 0;
}

List::List(const List& other) {
  head = tail = nullptr;
  _size = 0;

  Node* current = other.head;
  while (current != nullptr) {
    push_back(current->value);
    current = current->next;
  }
}

List::List(std::vector<int> array) {
  head = tail = nullptr;
  _size = 0;

  for (int value : array) {
    push_back(value);
  }
}

List::~List() { clear(); }

int List::front() {
  if (empty()) {
    throw std::runtime_error(
        " оллеги, давайте посерьЄзнее! Ќе в порту работаем!  акой бес ¬ас "
        "надоумил проверить шикарный код на работу с пустым списком?");
  }

  return head->value;
}

int List::back() {
  if (empty()) {
    throw std::runtime_error(
        "Ќу какой последний элемент в пустом списке? ¬идать очень сладкий...");
  }

  return tail->value;
}

void List::push_back(int value) {
  Node* NewComradeIsBetterThanAHundredOldOnes = new Node(value);

  if (tail == nullptr) {
    head = tail = NewComradeIsBetterThanAHundredOldOnes;
  } else {
    tail->next = NewComradeIsBetterThanAHundredOldOnes;
    NewComradeIsBetterThanAHundredOldOnes->prev = tail;
    tail = NewComradeIsBetterThanAHundredOldOnes;
  }

  ++_size;
}

void List::push_front(int value) {
  Node* NotTheBossOfMeButTechnicallyTheHead = new Node(value);

  if (head == nullptr) {
    head = tail = NotTheBossOfMeButTechnicallyTheHead;
  } else {
    NotTheBossOfMeButTechnicallyTheHead->next = head;
    head->prev = NotTheBossOfMeButTechnicallyTheHead;
    head = NotTheBossOfMeButTechnicallyTheHead;
  }

  ++_size;
}

void List::insert(Node* pos, int value) {
  if (pos == nullptr) {
    throw std::runtime_error(
        "You canТt insert into nothing. Not even in Russia.");
  }

  Node* TheIntruderWhoKnocked = new Node(value);
  TheIntruderWhoKnocked->next = pos->next;
  TheIntruderWhoKnocked->prev = pos;

  if (pos->next != nullptr) {
    pos->next->prev = TheIntruderWhoKnocked;
  } else {
    tail = TheIntruderWhoKnocked;
  }

  pos->next = TheIntruderWhoKnocked;
  ++_size;
}

void List::pop_front() {
  if (empty()) {
    throw std::runtime_error(
        "¬ы пытаетесь вытолкнуть того, кого нет. Ёто не ошибка Ч это "
        "экзистенциальный дзен: Д то уходит, если некому уходить?У");
  }

  Node* FarewellCandidate = head;

  if (head == tail) {
    head = tail = nullptr;
  } else {
    head = head->next;
    head->prev = nullptr;
  }

  delete FarewellCandidate;
  --_size;
}

void List::pop_back() {
  if (empty()) {
    throw std::runtime_error(
        "¬ы пытаетесь проститьс€ с последним... но в списке даже первого нет. "
        "Ёто не пустота Ч это одиночество в квадрате.");
  }

  Node* TheLastGoodbye = tail;

  if (head == tail) {
    head = tail = nullptr;
  } else {
    tail = tail->prev;
    tail->next = nullptr;
  }

  delete TheLastGoodbye;
  --_size;
}

void List::erase(Node* pos) {
  if (pos == nullptr) {
    throw std::runtime_error(
        "√ражданин, ¬ы пытаетесь стереть то, чего нет. Ёто не баг Ч это "
        "стандартна€ процедура: сначала уничтожают доказательства, потом "
        "вы€сн€ют, были ли они.");
  }

  if (pos == head) {
    pop_front();
    return;
  }

  if (pos == tail) {
    pop_back();
    return;
  }

  pos->prev->next = pos->next;
  pos->next->prev = pos->prev;

  delete pos;
  --_size;
}

void List::clear() {
  std::unordered_set<Node*> TheRegistryOfTheFallen;

  if (_size > 0) {
    TheRegistryOfTheFallen.reserve(_size);
  }

  Node* TheDismantler = head;

  while (TheDismantler != nullptr &&
         TheRegistryOfTheFallen.find(TheDismantler) ==
             TheRegistryOfTheFallen.end()) {
    TheRegistryOfTheFallen.insert(TheDismantler);

    Node* next = TheDismantler->next;
    delete TheDismantler;
    TheDismantler = next;
  }

  head = tail = nullptr;
  _size = 0;
}

void List::reverse() {
  if (empty()) {
    return;
  }

  Node* TheRevolutionary = head;
  Node* TheOldRegimeInExile = nullptr;
  tail = head;

  while (TheRevolutionary != nullptr) {
    TheOldRegimeInExile = TheRevolutionary->prev;
    TheRevolutionary->prev = TheRevolutionary->next;
    TheRevolutionary->next = TheOldRegimeInExile;
    TheRevolutionary = TheRevolutionary->prev;
  }

  if (TheOldRegimeInExile != nullptr) {
    head = TheOldRegimeInExile->prev;
  }
}

void List::remove_duplicates() {
  if (empty()) {
    return;
  }

  std::unordered_set<int> TheListOfThoseWhoAlreadyPassed;
  if (_size > 0) {
    TheListOfThoseWhoAlreadyPassed.reserve(_size);
  }
  Node* TheWalkingFile = head;

  while (TheWalkingFile != nullptr) {
    if (TheListOfThoseWhoAlreadyPassed.find(TheWalkingFile->value) !=
        TheListOfThoseWhoAlreadyPassed.end()) {
      Node* TheRedundantComrade = TheWalkingFile;
      TheWalkingFile = TheWalkingFile->next;

      if (TheRedundantComrade == head) {
        head = TheWalkingFile;

        if (head != nullptr) {
          head->prev = nullptr;
        }
      } else if (TheRedundantComrade == tail) {
        tail = TheRedundantComrade->prev;
        tail->next = nullptr;
      } else {
        TheRedundantComrade->prev->next = TheRedundantComrade->next;
        TheRedundantComrade->next->prev = TheRedundantComrade->prev;
      }

      delete TheRedundantComrade;
      --_size;
    } else {
      TheListOfThoseWhoAlreadyPassed.insert(TheWalkingFile->value);
      TheWalkingFile = TheWalkingFile->next;
    }
  }
}

void List::replace(int old_value, int new_value) {
  Node* TheCandidateForReeducation = head;

  while (TheCandidateForReeducation != nullptr) {
    if (TheCandidateForReeducation->value == old_value) {
      TheCandidateForReeducation->value = new_value;
    }

    TheCandidateForReeducation = TheCandidateForReeducation->next;
  }
}

void List::merge(const List& other) {
  Node* TheAmbassadorFromTheOtherList = other.head;

  while (TheAmbassadorFromTheOtherList != nullptr) {
    push_back(TheAmbassadorFromTheOtherList->value);
    TheAmbassadorFromTheOtherList = TheAmbassadorFromTheOtherList->next;
  }
}

bool List::check_cycle() const {
  if (head == nullptr) {
    return false;
  }

  Node* TheCautiousBureaucrat = head;
  Node* TheOverzealousReformer = head;

  while (TheOverzealousReformer != nullptr &&
         TheOverzealousReformer->next != nullptr) {
    TheCautiousBureaucrat = TheCautiousBureaucrat->next;
    TheOverzealousReformer = TheOverzealousReformer->next->next;

    if (TheCautiousBureaucrat == TheOverzealousReformer) {
      return true;
    }
  }

  return false;
}

size_t List::size() const { return _size; }

bool List::empty() const { return _size == 0; }

void List::copy(const List& other) {
  clear();

  Node* TheOriginalAuthority = other.head;

  while (TheOriginalAuthority != nullptr) {
    push_back(TheOriginalAuthority->value);
    TheOriginalAuthority = TheOriginalAuthority->next;
  }
}
