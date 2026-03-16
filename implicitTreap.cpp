#include "implicitTreap.h"

// Join uses priorities
ImplicitTreap* ImplicitTreap::join(ImplicitTreap* l, ImplicitTreap* r) {
  // The head-tree is going to be the tree (l or r) with the highest priority
  if (l == nullptr) {
    return r;
  }
  if (r == nullptr) {
    return l;
  }

  if (l->prior > r->prior) {
    // l->left remains intact, l->right is joined with r
    l->right = join(l->right, r);
    updateSize(l);
    return l;
  }

  // else r->right remains intact, r->left is joined with l
  r->left = join(l, r->left);
  updateSize(r);
  return r;
}

// Priorities aren't used and changed during split procedure,
// their order was correct before split, it stays correct afterwards
Pair<ImplicitTreap*> ImplicitTreap::split(ImplicitTreap* t, uint64_t key) {
  // Current node is t
  if (t == nullptr) {  // t is leaf
    return {nullptr, nullptr};
  }

  // In implicit treap the size of the left subtree is a key:
  //		t = {left, right},
  //		where left contains "key" amount of elements,
  //		and right contains the rest
  uint64_t ls = get_size(t->left);

  if (ls < key)  // "key" amount of elements include: all left subtree, the t
                 // node, some nodes from t->right
  {
    // Split is entirely in the right subtree: further split right
    // The split point lies after the left subtree
    // New key — amount of remaining elements we should take for the right tree:
    //		key - ls - 1 (already consumed "ls" elements and one element for
    // the current node t)
    auto q = split(t->right, key - ls - 1);

    // Splitting right gives two trees:
    //		"some nodes from t->right" are in q[0]
    //		=> attach them back under t node: t->right = q[0]
    //
    // Now the subtree at root t contains "key" elements

    t->right = q[0];
    updateSize(t);  // recalculate t->size after child change

    // Current node t: tree at root t ("key" elems), the rest of the original
    // tree
    return {t, q[1]};
  }

  // else ls >= key
  // Split is entirely in the left subtree: further split left
  //
  // The split point lies in the left subtree
  auto q = split(t->left, key);

  // q[1] is the part of original left subtree which is going to be
  // attached to t root becoming its left subtree
  //
  // q[0] is the resulting left subtree with "key" elems
  t->left = q[1];
  updateSize(t);
  return {q[0], t};
}