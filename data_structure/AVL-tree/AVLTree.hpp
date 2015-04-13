#ifndef AVLTREE_HPP
#define AVLTREE_HPP
#include <memory>

template <class T>
class AVLTree{
private:
  enum class BPL{
    L2_L,
    L2_RL_L,
    L2_LR_L,
    L3_L,
    L3_RL_L,
    L3_LR_L
  };
  enum class BPR{
    L2_R,
    L2_RL_R,
    L2_LR_R,
    L3_R,
    L3_RL_R,
    L3_LR_R
  };
  enum class FP{
    FOUND,
    NOTFOUND
  }; 
  struct Node {
    T value;
    Node *parent;
    Node *left;
    Node *right;
    int balance;
  };
  Node *make_node(T value = 0){
    Node *new_node = new Node;
    new_node->value = value;
    new_node->parent = nullptr;
    new_node->left = nullptr;
    new_node->right = nullptr;
    new_node->balance = 0;
    return new_node;
  }
  Node* root;
  size_t tree_size;
  void clear(Node*);
  Node* rotate_left(Node*, BPL);
  Node* rotate_right(Node *, BPR);
  void rebalance(Node*, bool);
  Node* find(T);
public:
  AVLTree();
  ~AVLTree();
  void insert(T);
  void erase(T);
  bool contain(T);
  size_t size();
  void clear();
};


template <class T>
AVLTree<T>::AVLTree()
  : tree_size(0)
{
  root = this->make_node();
}

template <class T>
AVLTree<T>::~AVLTree()
{
  this->clear();
  delete this->root;
}

template <class T>
void
AVLTree<T>::clear()
{
  if (this->tree_size) {
    this->clear(this->root->left);
    this->root->left = nullptr;
    this->tree_size = 0;
  }
}

template <class T>
void
AVLTree<T>::clear(Node *node)
{
  if (node->left) {
    this->clear(node->left);
  }
  if (node->right) {
    this->clear(node->right);
  }
  delete node;
}

template <class T>
bool
AVLTree<T>::contain(T value)
{
  return this->find(value) != nullptr;
}


template <class T>
typename AVLTree<T>::Node*
AVLTree<T>::find(T value)
{
  Node *parent = this->root->left;
  while(parent){
    if (value < parent->value) {
      parent = parent->left; 
    }else if (value > parent->value) {
      parent = parent->right;
    }else{
      return parent;
    }
  }
  return nullptr;
}

template <class T>
void
AVLTree<T>::insert(T value)
{
  Node* parent = this->root->left;
  Node *add_node = make_node(value);

  if (parent == nullptr) {
    this->root->left = add_node;
    add_node->parent = this->root;
    this->tree_size += 1;
    return;
  }

  if (this->contain(value)) {
    return;
  }

  while (true) {
    if (value < parent->value) {
      parent->balance -= 1;
      if (parent->left) {
        parent = parent->left;
      }else{
        parent->left = add_node;
        break;
      }
    }else{
      parent->balance += 1;

      if (parent->right) {
        parent = parent->right;
      }else{
        parent->right = add_node;
        break;
      }
    }
  }
  
  add_node->parent = parent;
  this->rebalance(add_node, true);
  this->tree_size += 1;
}

template <class T>
void
AVLTree<T>::erase(T value)
{
  Node *delete_node = this->find(value);
  if (delete_node == nullptr) {
    return;
  }
  Node *node = delete_node;

  while (node != this->root) {
    if (node == node->parent->left) {
      node->parent->balance += 1;
    }else{
      node->parent->balance -= 1;
    }
    node = node->parent;
  }
  if (delete_node->left == nullptr && delete_node->right == nullptr) {
    this->rebalance(delete_node, false);

    if (delete_node == delete_node->parent->left) {
      delete_node->parent->left = nullptr;
    }else{
      delete_node->parent->right = nullptr;
    }
    delete delete_node;
    this->tree_size -= 1;
    
  }else if (delete_node->right == nullptr) {

    this->rebalance(delete_node, false);

    if (delete_node == delete_node->parent->left) {
      delete_node->parent->left = delete_node->left;
    }else{
      if (delete_node->parent == this->root) {
        delete_node->parent->left = delete_node->left;
      }else{
        delete_node->parent->right = delete_node->left;
      }
    }
    delete_node->left->parent = delete_node->parent;

    delete delete_node;
    this->tree_size -= 1;
    
  }else if (delete_node->left == nullptr) {

    this->rebalance(delete_node, false);

    if (delete_node == delete_node->parent->left) {
      delete_node->parent->left = delete_node->right;
    }else{
      if (delete_node->parent == this->root) {
        delete_node->parent->left = delete_node->right;
      }else{
        delete_node->parent->right = delete_node->right;
      }
    }
    delete_node->right->parent = delete_node->parent;
    delete delete_node;
    this->tree_size -= 1;
    
  }else{

    delete_node->balance -= 1;
    Node *min_node = delete_node->right;
    while(min_node->left != nullptr){
      min_node->balance += 1;
      min_node=min_node->left;
    }

    this->rebalance(min_node, false);
    delete_node->value = min_node->value;

    if (min_node == min_node->parent->left) {
      min_node->parent->left = min_node->right;
    }else{
      if (min_node->parent == this->root) {
        min_node->parent->left = min_node->right;
      }else{
        min_node->parent->right = min_node->right;
      }
    }

    if (min_node->right != nullptr) {
      min_node->right->parent = min_node->parent;
    }
    this->tree_size -= 1;
    
  }
 
}


template <class T>
void
AVLTree<T>::rebalance(Node *node, bool insert_flag)
{
  Node *previous_node = node;
  node = node->parent;
  bool rotate_flag = false;

  while (node != this->root) {
    Node *child = nullptr;
    if (node->balance == -2) {
      child = node->left;
      if (child->balance == 0 && child->right != nullptr && (child->right->right != nullptr || child->right->left != nullptr)) {
        node = this->rotate_right(node, BPR::L3_R);
        rotate_flag = true;
      }else if (child->balance == -1 || child->balance == 0) {
        node = this->rotate_right(node, BPR::L2_R);
        rotate_flag = true;
      }else if (child->balance == 1) {
        if (child->right != nullptr && child->left == nullptr) {
          this->rotate_left(child, BPL::L2_LR_L);
          node = this->rotate_right(node, BPR::L2_LR_R);
        }else{
          this->rotate_left(child, BPL::L3_LR_L);
          node = this->rotate_right(node, BPR::L3_LR_R);
        }
        rotate_flag = true;
      }
    }else if (node->balance == 2) {
      child = node->right;
      if (child->balance == 0 && child->left != nullptr && (child->left->right != nullptr || child->left->left != nullptr)) {
        node = this->rotate_left(node, BPL::L3_L);
        rotate_flag = true;
      }else if (child->balance == 1 || child->balance == 0) {
        node = this->rotate_left(node, BPL::L2_L);
        rotate_flag = true;
      }else if (child->balance == -1) {
        if (child->right == nullptr && child->left != nullptr) {
          this->rotate_right(child, BPR::L2_RL_R);
          node = this->rotate_left(node, BPL::L2_RL_L);
        }else{
          this->rotate_right(child, BPR::L3_RL_R);
          node = this->rotate_left(node, BPL::L3_RL_L);
        }
        rotate_flag = true;
      }
    }
    if ((insert_flag && node->balance == 0) ||
        (!insert_flag && ((node->left == previous_node && node->balance == 1)
                          || (node->right == previous_node && node->balance == -1)
                          || (rotate_flag && (node->balance == 1 || node->balance == -1))))){
      break;
    }

    previous_node = node;
    node = node->parent;
    rotate_flag = false;
  }

  while(node!= this->root) {
    previous_node = node;
    node = node->parent;
    if (previous_node == node->left) {
      node->balance += (insert_flag) ? 1 : -1;
    }else{
      node->balance += (insert_flag) ? -1 : 1;
    }
  }
}



template <class T>
typename AVLTree<T>::Node*
AVLTree<T>::rotate_left(Node *node, BPL pattern)
{
  Node *right_node = node->right;
  node->right = right_node->left;
  right_node->left = node;
  
  Node *parent = node->parent;

  if (parent->left == node) {
    parent->left = right_node;
  }else{
    parent->right = right_node;
  }
  
  right_node->parent = parent;
  node->parent = right_node;
  if (node->right != nullptr) {
    node->right->parent = node;
  }

  switch(pattern){
  case BPL::L2_L:
    if (right_node->balance == 1) {
      node->balance = 0;
      right_node->balance = 0;
    }else{
      node->balance = 1;
      right_node->balance = -1;
    }
    break;
  case BPL::L2_RL_L:
    node->balance = 0;
    right_node->balance = 0;
    break;
  case BPL::L2_LR_L:
    node->balance = 0;
    right_node->balance = 0;
    break;
  case BPL::L3_L:
    node->balance = 1;
    right_node->balance = -1;
    break;
  case BPL::L3_RL_L:
    if (right_node->balance == 1) {
      node->balance = 0;
      right_node->balance = 0;
    }else if (right_node->balance == 2) {
      node->balance = -1;
      right_node->balance = 0;
    }else{
      ;
    }
    break;
  case BPL::L3_LR_L:
    if (right_node->balance == -1) {
      node->balance = 0;
      right_node->balance = -2;
    }else if (right_node->balance == 1) {
      node->balance = -1;
      right_node->balance = -1;
    }else{
      node->balance = 0;
      right_node->balance = -1;
    }
    break;
  }
  return right_node;
}


template <class T>
typename AVLTree<T>::Node*
AVLTree<T>::rotate_right(Node *node, BPR pattern)
{
  Node *left_node = node->left;
  node->left = left_node->right;
  left_node->right = node;

  Node *parent = node->parent;
  if (parent->left == node) {
    parent->left = left_node;
  }else{
    parent->right = left_node;
  }
  left_node->parent = parent;
  node->parent = left_node;
  if (node->left != nullptr) {
    node->left->parent = node;
  }

  switch(pattern){
  case BPR::L2_R:
    if (left_node->balance == -1) {
      node->balance = 0;
      left_node->balance = 0;
    }else{
      node->balance = -1;
      left_node->balance = 1;
    }
    break;
  case BPR::L2_RL_R:
    node->balance = 0;
    left_node->balance = 1;
    break;
  case BPR::L2_LR_R:
    node->balance = 0;
    left_node->balance = 0;
    break;
  case BPR::L3_R:
    node->balance = -1;
    left_node->balance = 1;
    break;
  case BPR::L3_RL_R:
    if (left_node->balance == -1) {
      node->balance = 1;
      left_node->balance = 1;
    }else if (left_node->balance == 1) {
      node->balance = 0;
      left_node->balance = 2;
    }else{
      node->balance = 0;
      left_node->balance = 1;
    }
    break;
  case BPR::L3_LR_R:
    if (left_node->balance == -2) {
      node->balance = 1;
      left_node->balance = 0;
    }else if (left_node->balance == -1) {
      node->balance = 0;
      left_node->balance = 0;
    }else{
      ;
    }
    break;
  }
  return left_node;
}

template <class T>
size_t
AVLTree<T>::size()
{
  return this->tree_size;
}

#endif
