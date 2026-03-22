#ifndef EDIT_H
#define EDIT_H
#include "implicitTreap.h"

class Edit {
 private:
  int64_t ptr = 0;
  int64_t start = -1;                  // start of selection; -1 is no selection
  ImplicitTreap* root = nullptr;       // current text as treap
  ImplicitTreap* clipboard = nullptr;  // clipboard buffer

 public:
  explicit Edit(const Vector<char*>& inBuffer,
                const Vector<NewLine>& new_lines) {
    ptr = 0;
    start = -1;
    root = nullptr;
    clipboard = nullptr;

    for (uint64_t i = 0; i < inBuffer.get_size(); i++) {
      ImplicitTreap* node = new ImplicitTreap(inBuffer[i], new_lines[i]);
      root = ImplicitTreap::join(root, node);
    }
  }

  ~Edit() {
    if (root != nullptr) {
      ImplicitTreap::free_treap(root);
    }

    if (clipboard != nullptr) {
      ImplicitTreap::free_treap(clipboard);
    }
  }

  // Cursor move (Up/Down)
  void Up() {
    if (ImplicitTreap::get_size(root) == 0) {
      return;
    }
    if (ptr > 0) {
      --ptr;
    }
  }

  void Down() {
    uint64_t n = ImplicitTreap::get_size(root);

    if (ptr + 1 <= int64_t(n)) {
      ++ptr;
    }
  }

  // Start selection
  void startShift() {
    if (ImplicitTreap::get_size(root) == 0) {
      return;
    }
    if (start == -1) {
      start = ptr;
    }
  }

  // Ctrl+X
  void Cut() {
    uint64_t n = ImplicitTreap::get_size(root);
    if (n == 0) {
      start = -1;
      return;
    }

    if (start == -1) {  // no selection -> cut the current line at ptr
      if (ptr <= int64_t(n)) {
        auto q1 = ImplicitTreap::split(root, uint64_t(ptr));
        auto q2 = ImplicitTreap::split(q1[1], 1);
        if (clipboard != nullptr) {
          ImplicitTreap::free_treap(clipboard);
        }
        clipboard = q2[0];

        root = ImplicitTreap::join(q1[0], q2[1]);

        // ptr stays at the gap where the line was
      }

      start = -1;
      return;
    }

    int64_t l = std::min(start, ptr);  // 1st line of selection
    int64_t r = std::max(start, ptr);  // last line of selection

    // q1[0] — subtree b4 selection, q1[1] — selection + subtree after selection
    // q2[0] — selection, q2[1] — subtree after selection
    auto q1 = ImplicitTreap::split(root, l);

    // r - l - the amount of lines in selection
    auto q2 = ImplicitTreap::split(q1[1], r - l);

    if (clipboard != nullptr) {
      ImplicitTreap::free_treap(clipboard);
    }

    // cut out the middle of q1, q2 (q1[0], q1[1]=>q2[0], q2[1])
    clipboard = q2[0];

    root = ImplicitTreap::join(q1[0], q2[1]);

    start = -1;  // no selection
    ptr = l;     // move cursor to the gap where the last line was
  }

  // Ctrl+V
  void Paste() {
    // if selection exists, cut it first
    // (don't call Cut() as it will overwrite clipboard)
    if (start != -1) {
      int64_t l = std::min(start, ptr);
      int64_t r = std::max(start, ptr);

      auto q1 = ImplicitTreap::split(root, uint64_t(l));
      auto q2 = ImplicitTreap::split(q1[1], uint64_t(r - l + 1));

      root = ImplicitTreap::join(q1[0], q2[1]);
      ptr = l;
    }

    if (clipboard == nullptr) {
      start = -1;
      return;
    }

    auto q = ImplicitTreap::split(root, ptr);

    // Deep copy to avoid double free behavior
    ImplicitTreap* clipboard_copy = ImplicitTreap::deep_copy(clipboard);

    root = ImplicitTreap::join(q[0], ImplicitTreap::join(clipboard_copy, q[1]));

    ptr += int64_t(ImplicitTreap::get_size(clipboard));  // move cursor

    start = -1;
  }

  static void editPrint(Edit& edit, std::ofstream& out) {
    ImplicitTreap::print(edit.root, out);
  }
};

#endif