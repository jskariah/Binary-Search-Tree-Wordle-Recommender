#pragma once

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

using namespace std;

template <typename T>
class BSTSet {
 private:
  struct BSTNode {
    // These two members should never change after initialization!
    const T value;
    BSTNode* parent;
    BSTNode* left;
    BSTNode* right;

    // This is fancy constructor syntax
    // Don't worry too much about it -- it's just necesssary for the const
    // member.
    BSTNode(T value, BSTNode* parent)
        : value(value), parent(parent), left(nullptr), right(nullptr) {
    }
  };

  BSTNode* root;
  size_t sz;

  // Utility pointer for begin and next.
  BSTNode* curr;

  // TODO_STUDENT: add private helper function definitions here

  // Recursive helper for insert

  void insertHelper(BSTNode* node, T value) {
    if (value < node->value) {
      if (node->left == nullptr) {
        node->left = new BSTNode(value, node);
        sz++;
      } else {
        insertHelper(node->left, value);
      }
    } else if (value > node->value) {
      if (node->right == nullptr) {
        node->right = new BSTNode(value, node);
        sz++;
      } else {
        insertHelper(node->right, value);
      }
    }
  }

  // Helper for clear

  void clearHelper(BSTNode* node) {
    if (node == nullptr) {
      return;
    }
    clearHelper(node->left);
    clearHelper(node->right);
    delete node;
  }

  // Helper for toString

  void toStringHelper(BSTNode* node, ostringstream& ss, int& count) const {
    if (node == nullptr) {
      return;
    }
    toStringHelper(node->left, ss, count);
    if (count != 0) {
      ss << ", ";
    }
    ss << node->value;
    count++;
    toStringHelper(node->right, ss, count);
  }

  // Helper for copy and assignment

  BSTNode* copyHelper(BSTNode* node, BSTNode* parent) {
    if (node == nullptr) {
      return nullptr;
    }
    BSTNode* newNode = new BSTNode(node->value, parent);
    newNode->left = copyHelper(node->left, newNode);
    newNode->right = copyHelper(node->right, newNode);
    return newNode;
  }

  // Helper for removeMin
  BSTNode* findMin(BSTNode* node) const {
    while (node->left != nullptr) {
      node = node->left;
    }
    return node;
  }

  // Helper for erase
  void removeNode(BSTNode* node) {
    if (node->left != nullptr && node->right != nullptr) {
      BSTNode* successor = findMin(node->right);

      if (successor->parent != node) {
        successor->parent->left = successor->right;
        if (successor->right != nullptr) {
          successor->right->parent = successor->parent;
        }
        successor->right = node->right;
        node->right->parent = successor;
      }

      successor->left = node->left;
      node->left->parent = successor;
      successor->parent = node->parent;

      if (node->parent == nullptr) {
        root = successor;
      } else if (node->parent->left == node) {
        node->parent->left = successor;
      } else {
        node->parent->right = successor;
      }

      delete node;
      return;
    }

    BSTNode* child = nullptr;
    if (node->left != nullptr) {
      child = node->left;
    } else if (node->right != nullptr) {
      child = node->right;
    }

    if (child != nullptr) {
      child->parent = node->parent;
    }

    if (node->parent == nullptr) {
      root = child;
    } else if (node->parent->left == node) {
      node->parent->left = child;
    } else {
      node->parent->right = child;
    }

    delete node;
  }

 public:
  /**
   * Creates an empty `BSTSet`. Runs in O(1).
   */
  BSTSet() {
    root = nullptr;
    sz = 0;
    curr = nullptr;
  }

  /**
   * Checks if the `BSTSet` is empty. Runs in O(1).
   */
  bool empty() const {
    return sz == 0;
  }

  /**
   * Returns the number of elements in the `BSTSet`. Runs in O(1).
   */
  size_t size() const {
    return sz;
  }

  /**
   * Adds the `value` to the `BSTSet`. If the value already
   * exists in the set, does not do anything.
   *
   * Uses the `value` to determine the location in the underlying BST.
   * Creates exactly one new node.
   *
   * Runs in O(H), where H is the height of the tree.
   */
  void insert(T value) {
    if (root == nullptr) {
      root = new BSTNode(value, nullptr);
      sz++;
    } else {
      insertHelper(root, value);
    }
  }

  /**
   * Returns `true` if the `value` is present in the set,
   * and `false` otherwise.
   *
   * Runs in O(H), where H is the height of the tree.
   */
  bool contains(const T& value) const {
    BSTNode* node = root;
    while (node != nullptr) {
      if (value < node->value) {
        node = node->left;
      } else if (value > node->value) {
        node = node->right;
      } else {
        return true;
      }
    }
    return false;  // Returns false all the way at the end
  }  // if it isn't found during traversal

  /**
   * Empties the `BSTSet`, freeing all memory it controls.
   *
   * Runs in O(N), where N is the number of elements in the set.
   */
  void clear() {
    clearHelper(root);
    root = nullptr;
    sz = 0;
  }

  /**
   * Destructor, cleans up the `BSTSet`.
   *
   * Runs in O(N), where N is the number of elements in the set.
   */
  ~BSTSet() {
    clear();
  }

  /**
   * Converts the `BSTSet` to a string representation, with the values
   * in-order by value.
   *
   * Example:
   *
   * c++
   * BSTSet<string> names;
   * names.insert("Gwen");
   * names.insert("Jen");
   * names.insert("Ben");
   * names.insert("Sven");
   *
   * Calling `names.to_string()` would return the following
   * string: `{Ben, Gwen, Jen, Sven}`
   *
   * Runs in O(N), where N is the number of elements.
   */
  string to_string() const {
    ostringstream ss;
    ss << "{";
    int count = 0;
    toStringHelper(root, ss, count);
    ss << "}";
    return ss.str();
  }

  /**
   * Copy constructor.
   *
   * Copies the elements from the provided `BSTSet`.
   * The internal tree structure must be copied exactly.
   *
   * Runs in O(N), where N is the number of elements in `other`.
   */
  BSTSet(const BSTSet& other) {
    root = copyHelper(other.root, nullptr);
    sz = other.sz;
    curr = nullptr;
  }

  /**
   * Assignment operator; `operator=`.
   *
   * Clears this tree, and copies the elements from the provided `BSTSet`.
   * The internal tree structure must be copied exactly.
   *
   * Runs in O(N + O), where N is the number of elements in `this`, and O is
   * the number of elements in `other`.
   */
  BSTSet& operator=(const BSTSet& other) {
    if (this != &other) {
      clear();
      root = copyHelper(other.root, nullptr);
      sz = other.sz;
    }
    return *this;
  }

  // =========================================================================

  /**
   * Locates the smallest element in the BSTSet, and removes it
   * from the `BSTSet`. If the `BSTSet` is empty, throws a
   * `runtime_error`.
   *
   * Creates no new nodes, and does not update any
   * existing nodes.
   *
   * Runs in O(H), where H is the height of the tree.
   */
  T remove_min() {
    if (root == nullptr) {
      throw runtime_error("Error: Tree is empty. No value to delete.");
    }
    BSTNode* minNode = findMin(root);
    T val = minNode->value;

    if (minNode == root) {
      root = minNode->right;
      if (root != nullptr) {
        root->parent = nullptr;
      }
    } else {
      minNode->parent->left = minNode->right;
      if (minNode->right != nullptr) {
        minNode->right->parent = minNode->parent;
      }
    }

    delete minNode;
    sz--;
    return val;
  }

  /**
   * Resets internal state for an iterative inorder traversal.
   *
   * See `next` for usage details. Modifies nothing except for `curr`.
   *
   * O(H), where H is the maximum height of the tree.
   */
  void begin() {
    if (root == nullptr) {
      curr = nullptr;
    } else {
      curr = findMin(root);
    }
  }

  /**
   * Uses the internal state to return the next in-order element
   * by reference, and advances the internal state. Returns `true`
   * if the reference was set, and `false` otherwise.
   *
   * Example usage:
   *
   * c++
   * BSTSet<string> bst;
   * bst.begin();
   * string value;
   * while (bst.next(val)) {
   *   cout << val << endl;
   * }
   * ```
   *
   * Modifies nothing except for `curr`.
   *
   * Runs in worst-case O(H) where H is the height of the tree.
   */
  bool next(T& value) {
    if (curr == nullptr) {
      return false;
    }

    value = curr->value;

    if (curr->right != nullptr) {
      curr = findMin(curr->right);
    } else {
      BSTNode* temp = curr;
      while (temp->parent != nullptr && temp == temp->parent->right) {
        temp = temp->parent;
      }
      curr = temp->parent;
    }
    return true;
  }

  // =========================================================================

  /**
   * Removes the given element from the `BSTSet`, and returns the
   * value.
   *
   * Throws `out_of_range` if the value is not present in the set.
   * Creates no new nodes, and does not update the value of any
   * existing nodes.
   *
   * Runs in worst-case O(H) where H is the height of the tree.
   */
  T erase(const T& value) {
    BSTNode* node = root;
    while (node != nullptr) {
      if (value < node->value) {
        node = node->left;
      } else if (value > node->value) {
        node = node->right;
      } else {
        T val = node->value;
        removeNode(node);
        sz--;
        return val;
      }
    }
    throw out_of_range("Value is not in BST");
  }

  // =========================================================================

  /**
   * Returns a pointer to the root node of the `BSTSet`. For autograder testing
   * purposes only.
   */
  void* getRoot() const {
    return this->root;
  }
};
