#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "rbtree.h"

rb_tree_t *rb_new() {
  rb_tree_t *T;

  T = malloc(sizeof(rb_tree_t));
  T->nil = malloc(sizeof(rb_node_t));
  memset(T->nil, 0, sizeof(rb_node_t));
  T->nil->left = T->nil;
  T->nil->right = T->nil;
  T->nil->parent = T->nil;
  T->root = T->nil;

  return T;
}

rb_node_t *rb_search(rb_tree_t *T, rb_key_t key) {
  rb_node_t *t;
  int d;

  t = T->root;

  while (t != T->nil) {
    d = rb_key_cmp(key, t->key);
    if (d == 0)
      break;
    else if (d < 0)
      t = t->left;
    else if (d > 0)
      t = t->right;
  }
  return t;
}

void rb_rotate(rb_tree_t *T, rb_node_t *x) {
  rb_node_t *gp = x->parent->parent;

  if (gp != T->nil) {
    if (gp->left == x->parent)
      gp->left = x;
    else
      gp->right = x;
  } else
    T->root = x;

  if (x == x->parent->left) {
    x->parent->left = x->right;
    if (x->right != T->nil)
      x->right->parent = x->parent;
    x->parent->parent = x;
    x->right = x->parent;
    x->parent = gp;
  } else {
    x->parent->right = x->left;
    if (x->left != T->nil)
      x->left->parent = x->parent;
    x->parent->parent = x;
    x->left = x->parent;
    x->parent = gp;
  }
}

rb_node_t *rb_insert(rb_tree_t *T, rb_key_t key, rb_val_t value) {
  int d;
  rb_node_t *t, *p, *uncle, *gp;

  /* search for key from root */
  p = T->nil;
  t = T->root;
  d = 0;
  while (t != T->nil) {
    d = rb_key_cmp(key, t->key);
    if (d == 0) {
      t->value = value;
      return t;
    }
    p = t;
    t = d < 0 ? t->left : t->right;
  }

  /* new leaf */
  t = malloc(sizeof(rb_node_t));
  t->key = key;
  t->value = value;
  t->left = T->nil;
  t->right = T->nil;
  t->parent = p;
  t->red = 1;
  if (d < 0) p->left = t;
  else p->right = t;

  /* special case the empty tree */
  if (T->root == T->nil) {
    t->red = 0;
    T->root = t;
    T->nil->left = T->nil->right = NULL;
    return t;
  }

  /* do the red black dance */
  while (t->parent->red) {
    gp = t->parent->parent;
    uncle = t->parent == gp->left ? gp->right : gp->left;

    /* case 1: uncle is red */
    if (uncle->red) {
      uncle->red = 0;
      gp->red = 1;
      t->parent->red = 0;
      t = gp;
      continue;
    }


    /* case 2: both t and uncle are on same side of their parent */
    if (!((t == t->parent->left) ^ (uncle == gp->left))) {
      p = t->parent;
      rb_rotate(T, t);
      t = p;
    }

    /* case 3: t and uncle are on different sides of their parent */
    t->parent->red = 0;
    t->parent->parent->red = 1;
    rb_rotate(T, t->parent);
  }

  T->root->red = 0;
  return t;
}

rb_node_t *rb_min(rb_tree_t *T, rb_node_t *t) {
  while (t->left != T->nil)
    t = t->left;
  return t;
}

rb_node_t *rb_max(rb_tree_t *T, rb_node_t *t) {
  while (t->right != T->nil)
    t = t->right;
  return t;
}

rb_node_t *rb_next(rb_tree_t *T, rb_node_t *t) {
  if (t->right != T->nil)
    return rb_min(T, t->right);
  while (t->parent != T->nil && t->parent->right == t)
    t = t->parent;
  return t->parent;
}

rb_node_t *rb_prev(rb_tree_t *T, rb_node_t *t) {
  if (t->left != T->nil)
    return rb_max(T, t->left);
  while (t->parent != T->nil && t->parent->left == t)
    t = t->parent;
  return t->parent;
}

rb_node_t *_rb_splice(rb_tree_t *T, rb_node_t *t) {
  rb_node_t *child;

  child = t->right != T->nil ? t->right : t->left;
  if (t->parent == T->nil)
    T->root = child;
  else {
    if (t->parent->left == t)
      t->parent->left = child;
    else
      t->parent->right = child;
  }
  child->parent = t->parent;

  return child;
}

void rb_delete(rb_tree_t *T, rb_node_t *t) {
  rb_node_t *y, *x, *sibling;
  char spliced_red;

  //fprintf(stderr, "deleting %d with l %d r %d p %d\n", t->key, t->left->key, t->right->key, t->parent->key);

  /* t has 2 children => replace t with next(t) and set x = child(next(t)) */
  if (t->left != T->nil && t->right != T->nil) {
    y = rb_next(T, t);
    spliced_red = y->red;
    x = _rb_splice(T, y);

    //fprintf(stderr, "replacing %d with %d\n", t->key, y->key);

    /* replace t with y */
    y->red = t->red;
    y->parent = t->parent;
    y->left = t->left;
    y->right = t->right;
    if (y->left != T->nil)
      y->left->parent = y;
    if (y->right != T->nil)
      y->right->parent = y;

    //fprintf(stderr, "root is now: %d l %d r %d\n", T->root->key, T->root->left->key, T->root->right->key);

    if (t == T->root) {
      //fprintf(stderr, "replacement becomes root\n");
      T->root = y;
    } else {
      //fprintf(stderr, "replacement becomes normal child of %d\n",t->parent->key);
      if (t->parent->left == t)
	t->parent->left = y;
      else
	t->parent->right = y;
    }
    if (x->parent == t)
      x->parent = y;
    //fprintf(stderr, "root is now: %d l %d r %d\n", T->root->key, T->root->left->key, T->root->right->key);
  }

  /* t has < 2 children => splice out t and set x = child(t) */
  else {
    spliced_red = t->red;
    x = _rb_splice(T, t);
    //fprintf(stderr, "<2 kids, splice out %d, set x %d\n", t->key, x->key);
  }

  //fprintf(stderr, "replaced %d with %d inspecting %d\n", t->key, y->key, x->key);
  free(t);

  /* if spliced out node was red then we're done */
  if (spliced_red) {
    //fprintf(stderr, "spliced something red, so we're done\n");
    T->nil->parent = T->nil;
    return;
  }

  /* red-black violation may have been introduced at x */
  //fprintf(stderr, "root is %d, inspecting the %s %d\n", T->root->key, x->red?"red":"black", x->key);
  while (!x->red && x != T->root) {
    //fprintf(stderr, "repair %d: p %d l %d r %d\n", x->key, x->parent->key, x->left->key, x->right->key);
    sibling = x == x->parent->right ? x->parent->left : x->parent->right;

    /* case 1 */
    if (sibling->red) {
      //fprintf(stderr, "case 1\n");
      //fprintf(stderr, "rotate(%d), red(%d), black (%d)\n", sibling->key, sibling->parent->key, sibling->key);
      sibling->red = 0;
      sibling->parent->red = 1;
      rb_rotate(T, sibling);
      sibling = x == x->parent->right ? x->parent->left : x->parent->right;
    }

    /* case 2 */
    if (!sibling->red && !sibling->left->red && !sibling->right->red) {
      //fprintf(stderr, "case 2\n");
      //fprintf(stderr, "red(%d)\n", sibling->key);
      sibling->red = 1;
      x = x->parent;
      continue;
    }

    /* case 3 */
    if (!sibling->red && ((x == x->parent->left && sibling->left->red) ||
			  (x == x->parent->right && sibling->right->red))) {
      //fprintf(stderr, "case 3\n");

      if (x == x->parent->left) {
	//fprintf(stderr, "black(%d)\n", sibling->left->key);
	//fprintf(stderr, "red(%d)\n", sibling->key);
	//fprintf(stderr, "rotate(%d)\n", sibling->left->key);
	sibling->left->red = 0;
	sibling->red = 1;
	rb_rotate(T, sibling->left);
      } else {
	//fprintf(stderr, "black(%d)\n", sibling->right->key);
	//fprintf(stderr, "red(%d)\n", sibling->key);
	//fprintf(stderr, "rotate(%d)\n", sibling->key);
	sibling->right->red = 0;
	sibling->red = 1;
	rb_rotate(T, sibling->right);
      }
      sibling = x == x->parent->right ? x->parent->left : x->parent->right;

      //fprintf(stderr, "x %d, parent %d, sibling %d l %d r %d\n", x->key, x->parent->key, sibling->key, sibling->left->key,sibling->right->key);
    }

    /* case 4 */
    if (!sibling->red && ((x == x->parent->left && sibling->right->red) ||
			  (x == x->parent->right && sibling->left->red))) {
      //fprintf(stderr, "case 4\n");
      //fprintf(stderr, "sibling is %d\n", sibling->key);
      //fprintf(stderr, "%s(%d)\n", x->parent->red ? "red" : "black", sibling->key);
      //fprintf(stderr, "black(%d)\n", x->parent->key);
      //fprintf(stderr, "black(%d)\n", x == x->parent->left ? sibling->right->key : sibling->left->key);
      //fprintf(stderr, "rotate(%d) (p %d)\n", sibling->key, sibling->parent->key);
      sibling->red = x->parent->red;
      x->parent->red = 0;
      if (x == x->parent->left)
	sibling->right->red = 0;
      else
	sibling->left->red = 0;
      rb_rotate(T, sibling);
      x = T->root;
    }
  }

  x->red = 0;
  T->nil->parent = T->nil;
  return;
}
