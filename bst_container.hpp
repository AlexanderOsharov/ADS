#ifndef BST_CONTAINER_HPP
#define BST_CONTAINER_HPP

#include <memory>

#include "bst_iterator.hpp"
#include "bst_node.hpp"

template <typename T>
class BST {
 public:
  using NodePtr = BSTNode<T>*;
  using iterator = BSTIterator<T>;
  using reverse_iterator = BSTReverseIterator<T>;

  BST() = default;

  void insert(const T& value) {
    root_ = insert_impl(std::move(root_), value, nullptr);
  }

  iterator begin() { return iterator(leftmost(root_.get())); }
  iterator end() { return iterator(nullptr); }

  reverse_iterator rbegin() { return reverse_iterator(rightmost(root_.get())); }
  reverse_iterator rend() { return reverse_iterator(nullptr); }

 private:
  std::unique_ptr<BSTNode<T>> root_;

  std::unique_ptr<BSTNode<T>> insert_impl(std::unique_ptr<BSTNode<T>> node,
                                          const T& value, BSTNode<T>* parent) {
    if (!node) {
      auto new_node = std::make_unique<BSTNode<T>>(value);
      new_node->parent = parent;
      return new_node;
    }

    if (value < node->data) {
      node->left = insert_impl(std::move(node->left), value, node.get());
    } else if (value > node->data) {
      node->right = insert_impl(std::move(node->right), value, node.get());
    }
    // Если value == node->data — игнорируем (уникальные элементы)

    return node;
  }

  static NodePtr leftmost(NodePtr node) {
    while (node && node->left) node = node->left.get();
    return node;
  }

  static NodePtr rightmost(NodePtr node) {
    while (node && node->right) node = node->right.get();
    return node;
  }
};

// ADL-совместимые begin/end
template <typename T>
BSTIterator<T> begin(BST<T>& tree) {
  return tree.begin();
}

template <typename T>
BSTIterator<T> end(BST<T>& tree) {
  return tree.end();
}

template <typename T>
BSTReverseIterator<T> rbegin(BST<T>& tree) {
  return tree.rbegin();
}

template <typename T>
BSTReverseIterator<T> rend(BST<T>& tree) {
  return tree.rend();
}

#endif  // BST_CONTAINER_HPP
