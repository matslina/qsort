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

/* Deletes node from tree.
 */
void bt_del(bt_node_t *x) {
  /* TODO */
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
