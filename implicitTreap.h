#ifndef TREAP_H
#define TREAP_H
#include <cstring>
#include <fstream>
#include <random>

#include "vector.h"

enum class NewLine {
  CRLF,  // "\r\n"
  LF,    // '\n'
  CR     // carriage return '\r'
};

// Treap is tree + heap
//
// Stores pairs (x,y):
//		key x (in implicit treap it is index of subtree. Index is
// calculated via size of subtree) - for BST 		priority y - for binary
// heap
//
// Therefore: for every node all children's priorities are less,
//            all children on the left have key less than the node's
//
// Build: O(nlogn), search/insert/delete: O(log n) + bulk operations: split and
// join
//
// Operations of Edit and ImplicitTreap:
//		move (up and down): change current key (index)
//      cut lines: split (isolate a tree M, clipboard = M) + join
//		paste lines: split (insert at key) + join

// Class if describing each Node in ImplicitTreap
class ImplicitTreap {
 private:
  char* line_;
  NewLine new_line_;

  // subtree size; used to calculate index which is key
  uint64_t size = 1;
  // random priority to keep tree balanced (operation O(logn))
  uint64_t prior = 0;

  ImplicitTreap* left = nullptr;
  ImplicitTreap* right = nullptr;

 public:
  explicit ImplicitTreap(char* line, NewLine new_line)
      : line_(line),
        new_line_(new_line),
        size(1),
        prior(std::rand()),
        left{nullptr},
        right{nullptr} {}

  // No copy constructors so there is no double free of memory
  // (when 2 treaps that share objects with the same pointers
  // (they share the same memory as they copy addresses, not data)
  // exist and one of them is deleted => both are deleted)
  ImplicitTreap(const ImplicitTreap&) = delete;
  ImplicitTreap& operator=(const ImplicitTreap&) = delete;

  static ImplicitTreap* deep_copy(ImplicitTreap* t) {
    if (t == nullptr) {
      return nullptr;
    }

    // Copy the string
    size_t L = std::strlen(t->line_);
    char* str = new char[L + 1]{0};
    std::memcpy(str, t->line_, L + 1);
    ImplicitTreap* node = new ImplicitTreap(str, t->new_line_);

    node->prior = t->prior;

    node->left = deep_copy(t->left);
    node->right = deep_copy(t->right);

    updateSize(node);

    return node;
  }

  static NewLine get_new_line(ImplicitTreap* t) { return t->new_line_; }

  static uint64_t get_size(ImplicitTreap* t) {
    return t != nullptr ? t->size : 0;
  }
  static void updateSize(ImplicitTreap* t) {
    if (t != nullptr) {
      t->size = 1 + get_size(t->left) + get_size(t->right);
    }
  }

  static ImplicitTreap* join(ImplicitTreap* l, ImplicitTreap* r);
  static Pair<ImplicitTreap*> split(ImplicitTreap* t, uint64_t key);

  static void free_treap(ImplicitTreap* t) {
    if (t == nullptr) {
      return;
    }

    // free children first
    free_treap(t->left);
    free_treap(t->right);

    delete[] t->line_;
    delete t;  // t was allocated with new ImplicitTreap; free pointer
  }

  static void print(ImplicitTreap* t, std::ofstream& out) {
    if (t == nullptr) {
      return;
    }

    print(t->left, out);

    out << t->line_;
    if (get_new_line(t) == NewLine::CRLF) {
      out << "\r\n";
    } else if (get_new_line(t) == NewLine::LF) {
      out << '\n';
    } else {
      out << '\r';
    }

    print(t->right, out);
  }
};

#endif