#ifndef EDIT_H
#define EDIT_H
#include <cstdint>
#include "implicitTreap.h"

class Edit {
 private:
  int64_t ptr = 0;
  int64_t start = -1;  // start of selection; -1 — no selection
  ImplicitTreap* root = nullptr;  // current text as treap
  ImplicitTreap* clipboard = nullptr;  // clipboard buffer

 public:
  explicit Edit(const Vector<char*>& copy) {
      ptr = 0;
      start = -1;
      root = nullptr;
      clipboard = nullptr;

      for (uint64_t i = 0; i < copy.get_size(); i++)
      {
          ImplicitTreap* node = new ImplicitTreap(copy[i]);
          root = ImplicitTreap::join(root, node);
      }
  }

  ~Edit(){
      if (root != nullptr)
      {
          ImplicitTreap::free_treap(root);
      }

      if (clipboard != nullptr)
      {
          ImplicitTreap::free_treap(clipboard);
      }
  }

  // Cursor move (Up/Down)
  void Up() {
      if (ImplicitTreap::get_size(root) == 0) { return; }
      if (ptr > 0) { --ptr; }
      else { ptr = 0; }  // ignore move
  }

  void Down()
  {
      uint64_t n = ImplicitTreap::get_size(root);
      if (n == 0) { return; }
      if (ptr + 1 < n) { ++ptr; }
      else { ptr = n - 1; }  // ignore move
  }

  // Start selection
  void startShift() {
      if (ImplicitTreap::get_size(root) == 0) { return; }
      if (start == -1) { start = ptr; }
  }

  // Ctrl+X
  void Cut() {
      if (start == -1) return;  // no selection

      int64_t l = std::min(start, ptr);  // 1st line of selection
      int64_t r = std::max(start, ptr);  // last line of selection

      // q1[0] — subtree b4 selection, q1[1] — selection + subtree after selection
      // q2[0] — selection, q2[1] — subtree after selection
      auto q1 = ImplicitTreap::split(root, l);
      auto q2 = ImplicitTreap::split(q1[1], r - l);  // r - l - the amount of elems in selection

      if (clipboard != nullptr)
      {
          ImplicitTreap::free_treap(clipboard);
      }
      clipboard = q2[0];  // cut out the middle of q1, q2 (q1[0], q1[1]=>q2[0], q2[1]) 

      root = ImplicitTreap::join(q1[0], q2[1]);

      start = -1;  // no selection
      ptr = l;  // move cursor
  }

  // Ctrl+V
  void Paste() {
      if (clipboard == nullptr) {
          return;
      }

      auto q = ImplicitTreap::split(root, ptr);

      // Deep copy to avoid double free behavior 
      ImplicitTreap* clipboard_copy = ImplicitTreap::deep_copy(clipboard);
      
      root = ImplicitTreap::join(q[0], ImplicitTreap::join(clipboard_copy, q[1]));
  
      ptr += ImplicitTreap::get_size(clipboard_copy);  // move cursor
  }

  static void editPrint(Edit& edit, std::ofstream& out)
  {
      ImplicitTreap::print(edit.root, out);
  }
};

#endif