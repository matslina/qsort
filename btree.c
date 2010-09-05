#include <stdlib.h>
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
