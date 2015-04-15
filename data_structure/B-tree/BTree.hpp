#ifndef BTREE_HPP
#define BTREE_HPP

template <class T>
class BTree{
private:
  int M;
  struct Node {
    int N;
    T *values;
    Node** childs;
  };
  Node *make_node(){
    Node *new_node = new Node;
    new_node->N = 0;
    new_node->values = new T[2*M];
    new_node->childs = new Node*[2*M+1];
    for (int i = 0; i <= 2 * M; i++) {
      new_node->childs[i] = nullptr;
    }
    return new_node;
  }
  Node *root;
  size_t tree_size;
  //insert
  void insert_k(Node*, int, T, Node*);
  void split(Node*, int, T&, Node*&);
  bool insert_sub(Node*, T&, Node*&);
  //erase
  bool erase_k(Node*, int);
  void move_right(Node*, int);
  void move_left(Node*, int);
  bool combine(Node*, int);
  bool restore(Node*, int);
  bool erase_sub(Node*, T);
  //clear
  void clear_node(Node*);
public:
  BTree(int);
  BTree();
  ~BTree();
  void insert(T);
  void erase(T);
  bool contain(T);
  size_t size();
  void clear();
};

template <class T>
BTree<T>::BTree()
{
  this->M = 2;
  this->root = this->make_node();
  this->tree_size = 0;
}
template <class T>
BTree<T>::BTree(int m)
{
  this->M = m;
  this->root = this->make_node();
  this->tree_size = 0;
}

template <class T>
BTree<T>::~BTree()
{
  this->clear_node(this->root);
}


template <class T>
bool
BTree<T>::contain(T value)
{
  Node *current = this->root;
  while (current) {
    int n = 0;
    while (n < current->N && current->values[n] < value) {
      n += 1;
    }
    if (n < current->N && current->values[n] == value) {
      return true;
    }
    current = current->childs[n];
  }
  return false;
}

template <class T>
void
BTree<T>::insert_k(Node *node, int k, T value, Node* new_child)
{
  for (int i = node->N; i > k; i--) {
    node->values[i] = node->values[i-1];
    node->childs[i + 1] = node->childs[i];
  }
  node->values[k] = value;
  node->childs[k+1] = new_child;
  node->N += 1;
}

template <class T>
void
BTree<T>::split(Node *node, int k, T &value, Node* &new_child)
{
  int div = this->M;
  if (k > this->M) {
    div = this->M + 1;
  }
  
  Node *q = this->make_node();
  for (int j = div + 1; j <= 2 * this->M; j++) {
    q->values[j - div - 1] = node->values[j - 1];
    q->childs[j - div] = node->childs[j];
  }
  q->N = 2 * this->M - div;
  node->N = div;
  if (k <= this->M) {
    this->insert_k(node, k, value, new_child);
  }else{
    this->insert_k(q, k - div, value, new_child);
  }
  value = node->values[node->N - 1];
  q->childs[0] = node->childs[node->N];
  node->N -= 1;
  new_child = q;
}

template <class T>
bool
BTree<T>::insert_sub(Node* node,T &value, Node* &new_child)
{
  if (node == nullptr) {
    new_child = nullptr;
    return false;
  }
  int k = 0;
  while (k < node->N && node->values[k] < value) {
    k++;
  }
  if (k < node->N && node->values[k] == value) {
    return true;
  }

  bool OK = this->insert_sub(node->childs[k], value, new_child);
  if (OK) {
    return true;
  }
  if (node-> N < 2 * this->M) {
    this->insert_k(node, k, value, new_child);
    return true;
  }else{
    this->split(node, k, value, new_child);
    return false;
  }
}


template <class T>
void
BTree<T>::insert(T value)
{
  if (this->contain(value)) {
    return;
  }
  Node *new_child = nullptr;
  bool OK = this->insert_sub(this->root, value, new_child);
  if (!OK) {
    Node *p = this->make_node();
    p->N = 1;
    p->values[0] = value;
    p->childs[0] = root;
    p->childs[1] = new_child;
    this->root = p;
  }
  this->tree_size += 1;
}

template <class T>
bool
BTree<T>::erase_k(Node *node, int k)
{
  for (int i = k + 1; i < node->N; i++) {
    node->values[i - 1] = node->values[i];
    node->childs[i] = node->childs[i + 1];
  }
  node->N -= 1;
  return node->N < this->M;
}

template <class T>
void
BTree<T>::move_right(Node *node, int k)
{
  Node *left = node->childs[k - 1];
  Node *right = node->childs[k];

  for (int i = right->N; i > 0; i--) {
    right->values[i] = right->values[i - 1];
    right->childs[i + 1] = right->childs[i];
  }
  right->childs[1] = right->childs[0];
  right->N += 1;
  right->values[0] = node->values[k - 1];
  node->values[k - 1] = left->values[left->N - 1];
  right->childs[0] = left->childs[left->N];
  left->N -= 1;
}

template <class T>
void
BTree<T>::move_left(Node *node, int k)
{
  Node *left = node->childs[k - 1];
  Node *right = node->childs[k];

  left->N += 1;
  left->values[left->N - 1] = node->values[k - 1];
  left->childs[left->N] = right->childs[0];
  
  node->values[k - 1] = right->values[0];
  
  right->childs[0] = right->childs[1];
  right->N -= 1;
  for (int i = 1; i <= right->N; i++) {
    right->childs[i] = right->childs[i + 1];
    right->values[i - 1] = right->values[i];
  }
}

template <class T>
bool
BTree<T>::combine(Node *node, int k)
{
  Node *left = node->childs[k - 1];
  Node *right = node->childs[k];

  left->N += 1;
  left->values[left->N - 1] = node->values[k - 1];
  left->childs[left->N] = right->childs[0];

  for (int i = 1; i <= right->N; i++) {
    left->N += 1;
    left->values[left->N - 1] = right->values[i - 1];
    left->childs[left->N] = right->childs[i];
  }
  bool under_size = this->erase_k(node, k - 1);
  delete right;
  return under_size;
}

template <class T>
bool
BTree<T>::restore(Node *node, int k)
{
  if (k > 0) {
    if (node->childs[k - 1]->N > M) {
      this->move_right(node, k);
    }else{
      return this->combine(node, k);
    }
  }else{
    if (node->childs[1]->N > M) {
      this->move_left(node, 1);
    }else{
      return this->combine(node, 1);
    }
  }
  return false;
}
#define endl "\n"
template <class T>
bool
BTree<T>::erase_sub(Node *node, T value)
{
  if (node == nullptr) {
    return true;
  }
  int k = 0;
  while (k < node->N && node->values[k] < value) {
    k += 1;
  }
  if (k < node->N && node->values[k] == value) {
    Node *p = node->childs[k + 1];
    if (p) {
      while(p->childs[0]) {
        p = p->childs[0];
      }
      value = p->values[0];
      node->values[k] = value;
      bool under_size = this->erase_sub(node->childs[k + 1], value);
      if (under_size) {
        return this->restore(node, k + 1);
      }
    }else{
      auto x = this->erase_k(node,k);
      return x;
    }
  }else{
    bool under_size = this->erase_sub(node->childs[k], value);
    if (under_size) {
      return this->restore(node, k);
    }
  }
  return false;
}


template <class T>
void
BTree<T>::erase(T value)
{
  if (!this->contain(value)) {
    return;
  }
  this->erase_sub(this->root, value);
  if (this->root->N == 0) {
    Node *p = root;
    this->root = this->root->childs[0];
    free(p);
  }
  this->tree_size -= 1;
}

 
template <class T>
size_t
BTree<T>::size()
{
  return this->tree_size;
}

template <class T>
void
BTree<T>::clear_node(Node *node)
{
  if (node != nullptr) {
    for (int i = 0; i <= node->N; i++) {
      this->clear_node(node->childs[i]);
    }
    delete node;
  }
}
template <class T>
void
BTree<T>::clear()
{
  for (int i = 0; i <= 2 * M; i++) {
    this->clear_node(this->root->childs[i]);
    this->root->childs[i] = nullptr;
  }
  this->root->N = 0;
  
}
#endif
