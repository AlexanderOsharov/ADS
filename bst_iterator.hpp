#ifndef BST_ITERATOR_HPP
#define BST_ITERATOR_HPP

#include <cstddef>
#include <iterator>

#include "bst_node.hpp"

template <typename T>
class BST;

template <typename T>
class BSTIterator {
 public:
  using value_type = T;
  using pointer = T*;
  using reference = T&;
  using difference_type = std::ptrdiff_t;
  using iterator_category = std::bidirectional_iterator_tag;

  BSTIterator() : node_(nullptr) {}
  explicit BSTIterator(BSTNode<T>* node) : node_(node) {}

  reference operator*() const { return node_->data; }
  pointer operator->() const { return &node_->data; }

  BSTIterator& operator++();  // определение ниже
  BSTIterator operator++(int);
  BSTIterator& operator--();  // определение ниже
  BSTIterator operator--(int);

  bool operator==(const BSTIterator& other) const {
    return node_ == other.node_;
  }
  bool operator!=(const BSTIterator& other) const { return !(*this == other); }

 private:
  BSTNode<T>* node_;
};

template <typename T>
class BSTReverseIterator {
 public:
  using value_type = T;
  using pointer = T*;
  using reference = T&;
  using difference_type = std::ptrdiff_t;
  using iterator_category = std::bidirectional_iterator_tag;

  BSTReverseIterator() : node_(nullptr) {}
  explicit BSTReverseIterator(BSTNode<T>* node) : node_(node) {}

  reference operator*() const { return node_->data; }
  pointer operator->() const { return &node_->data; }

  BSTReverseIterator& operator++();
  BSTReverseIterator operator++(int);
  BSTReverseIterator& operator--();
  BSTReverseIterator operator--(int);

  bool operator==(const BSTReverseIterator& other) const {
    return node_ == other.node_;
  }
  bool operator!=(const BSTReverseIterator& other) const {
    return !(*this == other);
  }

 private:
  BSTNode<T>* node_;
};

// Определения методов ++ и --
template <typename T>
BSTIterator<T>& BSTIterator<T>::operator++() {
  if (node_->right) {
    node_ = node_->right.get();
    while (node_->left) node_ = node_->left.get();
  } else {
    BSTNode<T>* parent = node_->parent;
    while (parent && node_ == parent->right.get()) {
      node_ = parent;
      parent = parent->parent;
    }
    node_ = parent;
  }
  return *this;
}

template <typename T>
BSTIterator<T> BSTIterator<T>::operator++(int) {
  BSTIterator tmp = *this;
  ++(*this);
  return tmp;
}

template <typename T>
BSTIterator<T>& BSTIterator<T>::operator--() {
  if (!node_) {
    // end() -- => rbegin()
    // Но в нашем случае вызов -- от end() не поддерживается напрямую.
    // Поддержка требует знания корня. Поэтому не реализуем это здесь.
    // Вместо этого гарантируем, что -- вызывается только от валидного узла.
    return *this;
  }
  if (node_->left) {
    node_ = node_->left.get();
    while (node_->right) node_ = node_->right.get();
  } else {
    BSTNode<T>* parent = node_->parent;
    while (parent && node_ == parent->left.get()) {
      node_ = parent;
      parent = parent->parent;
    }
    node_ = parent;
  }
  return *this;
}

template <typename T>
BSTIterator<T> BSTIterator<T>::operator--(int) {
  BSTIterator tmp = *this;
  --(*this);
  return tmp;
}

template <typename T>
BSTReverseIterator<T>& BSTReverseIterator<T>::operator++() {
  if (node_->left) {
    node_ = node_->left.get();
    while (node_->right) node_ = node_->right.get();
  } else {
    BSTNode<T>* parent = node_->parent;
    while (parent && node_ == parent->left.get()) {
      node_ = parent;
      parent = parent->parent;
    }
    node_ = parent;
  }
  return *this;
}

template <typename T>
BSTReverseIterator<T> BSTReverseIterator<T>::operator++(int) {
  BSTReverseIterator tmp = *this;
  ++(*this);
  return tmp;
}

template <typename T>
BSTReverseIterator<T>& BSTReverseIterator<T>::operator--() {
  if (!node_) return *this;
  if (node_->right) {
    node_ = node_->right.get();
    while (node_->left) node_ = node_->left.get();
  } else {
    BSTNode<T>* parent = node_->parent;
    while (parent && node_ == parent->right.get()) {
      node_ = parent;
      parent = parent->parent;
    }
    node_ = parent;
  }
  return *this;
}

template <typename T>
BSTReverseIterator<T> BSTReverseIterator<T>::operator--(int) {
  BSTReverseIterator tmp = *this;
  --(*this);
  return tmp;
}

#endif  // BST_ITERATOR_HPP
