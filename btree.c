#include <string.h> /* memset */
#include <stdlib.h> /* calloc, free */
#include "btree.h"

/* Creates a new tree with (key,value) as root.
 */
bt_node_t *bt_new(bt_key_t key, bt_value_t value) {
  bt_node_t *n;
  n = calloc(1, sizeof(bt_node_t));
  n->key = key;
  n->value = value;
  return n;
}

/* Sets key to value.
 * Overwrites if key already in tree.
 */
bt_node_t *bt_set(bt_node_t *T, bt_key_t key, bt_value_t value) {
  int d;
  bt_node_t *pT;

  do {
    d = bt_key_cmp(key, T->key);

    if (d == 0) {
      T->value = value;
      return T;
    }

    pT = T;
    if (d < 0) T = T->left;
    else T = T->right;
  } while(T);

  T = bt_new(key, value);
  T->parent = pT;
  if (d < 0)
    pT->left = T;
  else
    pT->right = T;
  return T;
}

/* Searches for the node by key.
 * Returns null if key is not in tree.
 */
bt_node_t *bt_get(bt_node_t *T, bt_key_t key) {
  int d;

  while (T) {
    d = bt_key_cmp(key, T->key);
    if (d == 0)
      break;
    else if (d < 0)
      T = T->left;
    else if (d > 0)
      T = T->right;
  }
  return T;
}

/* Finds the largest element in tree.
 */
bt_node_t *bt_max(bt_node_t *T) {
  while (T->right)
    T = T->right;
  return T;
}

/* Finds the smallest element in tree.
 */
bt_node_t *bt_min(bt_node_t *T) {
  while (T->left)
    T = T->left;
  return T;
}

/* Finds the successor of x,
 * i.e. the node with the smallest key larger than that of x.
 */
bt_node_t *bt_next(bt_node_t *x) {
  if (x->right)
    return bt_min(x->right);
  while (x->parent && x->parent->right == x)
    x = x->parent;
  return x->parent;
}

/* Find the predecessor of x,
 * i.e. the node with the largest key smaller than that of x.
 */
bt_node_t *bt_prev(bt_node_t *x) {
  if (x->left)
    return bt_max(x->left);
  while (x->parent && x->parent->left == x)
    x = x->parent;
  return x->parent;
}

/* "Splices out" x, i.e. replaces it with it's child.
 * x must have < 2 children.
 */
bt_node_t *_bt_splice(bt_node_t *x) {
  bt_node_t *child;

  child = x->right ? x->right : x->left;
  if (x->parent) {
    if (x->parent->left == x)
      x->parent->left = child;
    else
      x->parent->right = child;
  }
  if (child)
    child->parent = x->parent;

  return child;
}


/* Deletes node from tree.
 * Returns the node that takes the deleted nodes place, if any.
 * Note: if deleting the root node, the returned node (if any) is the new root.
 */
bt_node_t *bt_del(bt_node_t *x) {
  bt_node_t *y;

  if (!x->left || !x->right) {
    y = _bt_splice(x);
    free(x);
    return y;
  }

  y = bt_next(x);
  _bt_splice(y);

  if (x->parent) {
    if (x->parent->left == x)
      x->parent->left = y;
    else
      x->parent->right = y;
  }

  if (x->left)
    x->left->parent = y;
  if (x->right)
    x->right->parent = y;

  y->parent = x->parent;
  y->left = x->left;
  y->right = x->right;
  free(x);
  return y;
}

int _bt_stat_rec(bt_node_t *T, bt_stat_t *stat, int depth) {

  if (T->left) {
    if (T->left->key >= T->key)
      stat->broken_msg = "left child key too large";
    if (T->left->parent != T)
      stat->broken_msg = "left child has wrong parent";
  }

  if (T->right) {
    if (T->right->key <= T->key)
      stat->broken_msg = "right child key too small";
    if (T->right->parent != T)
      stat->broken_msg = "right child has wrong parent";
  }

  if (stat->broken_msg) {
    stat->broken_node = T;
    stat->broken_depth = depth;
    return 1;
  }

  stat->num_nodes += 1;

  if (!T->left && !T->right) {
    stat->num_leaves += 1;
    stat->max_depth = depth > stat->max_depth ? depth : stat->max_depth;
    stat->min_depth = depth < stat->min_depth ? depth : stat->min_depth;
  }

  if (T->left)
    if (_bt_stat_rec(T->left, stat, depth + 1))
      return 1;

  if (T->right)
    if (_bt_stat_rec(T->right, stat, depth + 1))
      return 1;

  return 0;
}

/* Builds stats for a tree.
 */
void bt_stat(bt_node_t *T, bt_stat_t *stat) {
  memset((void *)stat, 0, sizeof(bt_stat_t));
 _bt_stat_rec(T, stat, 0);
}

struct _bt_walk_bf_level_s {
  bt_node_t **p;
  int size;
  int count;
};

/* Traverses tree breadth first.
 * Invokes f(node, depth) on every node.
 * Stops if f returns non-zero.
 */
void bt_walk_bf(bt_node_t *T, int (*f)(bt_node_t *, int)) {
  struct _bt_walk_bf_level_s a, b;
  struct _bt_walk_bf_level_s *curr, *next, *swap;
  bt_node_t *x;
  int i, depth;

  curr = &a;
  next = &b;

  curr->size = 1; /* any size > 0 will do*/
  curr->p = malloc(curr->size * sizeof(bt_node_t *));
  curr->count = 1;
  curr->p[0] = T;

  next->size = -1;
  next->p = NULL;

  depth = 0;
  while (curr->count) {

    if (next->size < 2 * curr->count) {
      next->size = 2 * curr->count;
      if(next->p) free(next->p);
      next->p = malloc(next->size * sizeof(bt_node_t *));
    }

    next->count = 0;
    for (i=0; i<curr->count; i++) {
      x = curr->p[i];
      if (x->left)
	next->p[next->count++] = x->left;
      if (x->right)
	next->p[next->count++] = x->right;

      if (f(x, depth))
	break;
    }

    swap = curr; curr = next; next = swap;
    depth++;
  }

  if (curr->p) free(curr->p);
  if (next->p) free(next->p);
}
