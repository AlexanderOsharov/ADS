#ifndef BST_NODE_HPP
#define BST_NODE_HPP

#include <memory>

template <typename T>
struct BSTNode {
  T data;
  std::unique_ptr<BSTNode> left;
  std::unique_ptr<BSTNode> right;
  BSTNode* parent;

  explicit BSTNode(const T& val) : data(val), parent(nullptr) {}
};

#endif  // BST_NODE_HPP
