#include <functional>
#include <iostream>
#include <random>

struct Performer {
  int act_number;
  int random_luck;
  int crowd_size;
  Performer *left_hand, *right_hand;

  Performer(int act)
      : act_number(act),
        random_luck(rand()),
        crowd_size(1),
        left_hand(nullptr),
        right_hand(nullptr) {}
};

int audience(Performer* p) { return p ? p->crowd_size : 0; }

void recount_audience(Performer* p) {
  if (p) p->crowd_size = 1 + audience(p->left_hand) + audience(p->right_hand);
}

void split_stage(Performer* stage, int pos, Performer*& left_circus,
                 Performer*& right_circus) {
  if (!stage) {
    left_circus = right_circus = nullptr;
    return;
  }

  int left_audience = audience(stage->left_hand);

  if (pos <= left_audience) {
    split_stage(stage->left_hand, pos, left_circus, stage->left_hand);
    right_circus = stage;
  } else {
    split_stage(stage->right_hand, pos - left_audience - 1, stage->right_hand,
                right_circus);
    left_circus = stage;
  }

  recount_audience(stage);
}

Performer* merge_circuses(Performer* left, Performer* right) {
  if (!left) return right;
  if (!right) return left;
  if (left->random_luck > right->random_luck) {
    left->right_hand = merge_circuses(left->right_hand, right);
    recount_audience(left);

    return left;
  } else {
    right->left_hand = merge_circuses(left, right->left_hand);
    recount_audience(right);

    return right;
  }
}

void hire_at(Performer*& main_stage, int pos, int act) {
  Performer *left_show, *right_show;
  split_stage(main_stage, pos, left_show, right_show);

  Performer* newbie = new Performer(act);
  main_stage = merge_circuses(merge_circuses(left_show, newbie), right_show);
}

int peek_at(Performer* stage, int pos) {
  int left_audience = audience(stage->left_hand);

  if (pos < left_audience) return peek_at(stage->left_hand, pos);
  if (pos == left_audience) return stage->act_number;

  return peek_at(stage->right_hand, pos - left_audience - 1);
}

void fire_from_front(Performer*& main_stage) {
  Performer *before, *victim, *after;

  split_stage(main_stage, 0, before, victim);
  split_stage(victim, 1, victim, after);

  delete victim;
  main_stage = merge_circuses(before, after);
}

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  int T;
  std::cin >> T;

  Performer* main_stage = nullptr;

  while (T--) {
    char command;
    std::cin >> command;

    if (command == '+') {
      int act;
      std::cin >> act;

      hire_at(main_stage, audience(main_stage), act);
    } else if (command == '*') {
      int act;
      std::cin >> act;

      int center = (audience(main_stage) + 1) / 2;
      hire_at(main_stage, center, act);
    } else if (command == '-') {
      int first_act = peek_at(main_stage, 0);

      std::cout << first_act << '\n';
      fire_from_front(main_stage);
    }
  }

  return 0;
}
