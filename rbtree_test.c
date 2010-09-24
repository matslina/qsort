#include <stdlib.h>
#include <stdio.h>
#include <check.h>
#include "rbtree.h"

#define BLACK 0
#define RED 1

void printtree(rb_tree_t *T, rb_node_t *t) {
  fprintf(stderr,"%d l %d r %d p %d (%s)\n",t->key,t->left->key,t->right->key,
	  t->parent->key,t->red ? "red" : "black");
  if (t->left != T->nil)
    printtree(T, t->left);
  if (t->right != T->nil)
    printtree(T, t->right);
}

/* Fails is tree is not a proper red black binary tree. */
#define REDBLACK(T)					\
  do{							\
    redblackerr = is_red_black(T, T->root, 0, 0);		\
    if(redblackerr)					\
      fail(redblackerr);	\
  }while(0)
char *redblackerr;
char *is_red_black(rb_tree_t *T, rb_node_t *x, int blackdepth, int depth) {
  static int leafblackdepth;
  static char msg[4096];
  char *err;

  if (x == T->root) {
    if (x == T->nil)
      return NULL;
    if (x->red) {
      snprintf(msg, sizeof(msg), "root can't be red\n");
      return msg;
    }
    leafblackdepth = -1;
  }

  if (x->red) {
    if ((x->left && x->left->red) || (x->right && x->right->red)) {
      snprintf(msg, sizeof(msg), "red node with key %d has red %s child "
	       "(depth %d)\n", x->key, x->left->red ? "left" : "right", depth);
      return msg;
    }
  } else
    blackdepth ++;

  if (x->left == T->nil || x->right == T->nil) {

    if (leafblackdepth < 0)
      leafblackdepth = blackdepth + 1;

    if (leafblackdepth != blackdepth + 1) {
      snprintf(msg, sizeof(msg), "leaf below key %d has blackdepth %d but "
	       "previous had %d (depth %d)\n",
	       x->key, blackdepth + 1, leafblackdepth, depth);
      return msg;
    }
  }

  if (x->left != T->nil) {
    if (x->left->key >= x->key) {
      snprintf(msg, sizeof(msg),
               "node %d has left child with too large key %d (depth %d)\n",
	       x->key, x->left->key, depth);
      return msg;
    }
    if (x->left->parent != x) {
      snprintf(msg, sizeof(msg),
               "node %d left child %d believs it's parent is %d (depth %d)\n",
	       x->key, x->left->key, x->left->parent->key, depth);
      return msg;
    }
    err = is_red_black(T, x->left, blackdepth, depth + 1);
    if (err) return err;
  }
  if (x->right != T->nil) {
    if (x->right->key <= x->key) {
      snprintf(msg, sizeof(msg),
               "node %d has right child with too large key %d (depth %d)\n",
	       x->key, x->right->key, depth);
      return msg;
    }
    if (x->right->parent != x) {
      snprintf(msg, sizeof(msg),
               "node %d right child %d believs it's parent is %d (depth %d)\n",
	       x->key, x->right->key, x->right->parent->key, depth);
      return msg;
    }
    err = is_red_black(T, x->right, blackdepth, depth + 1);
    if (err) return err;;
  }

  return NULL;
}

/* Searches depth first and fails on diff between E and T.
 *
 * The macro serves to make libcheck report the error where DIFF was
 * invoked, rather than where in diff_tree() the error was found,
 * which would be the case if diff_tree invoked fail itself. */
#define DIFF(E, T)							\
  do{									\
    differr = diff_tree(E, E->root, T, T->root, 0);			\
    if (differr) fail(differr);						\
  }while(0)
char *differr;
char *diff_tree(rb_tree_t *T, rb_node_t *t, rb_tree_t *U, rb_node_t *u, int depth) {
  static char err[4096];
  char *e;

  if (t->key != u->key) {
    snprintf(err, sizeof(err), "expected key %d but found %d (depth %d)",
	     t->key, u->key, depth);
    return err;
  }

  if (t->red != u->red) {
    snprintf(err, sizeof(err), "expected %s but found %s at key %d (depth %d)",
	     t->red ? "red" : "black", u->red ? "red" : "black", t->key, depth);
    return err;
  }

  if ((t->left == T->nil) ^ (u->left == U->nil)) {
    snprintf(err, sizeof(err),
	     "expected left %s but found %s at key %d (depth %d)",
	     t->left == T->nil ? "NIL" : "non-NIL",
	     u->left == U->nil ? "NIL" : "non-NIL", t->key, depth);
    return err;
  }
  if ((t->right == T->nil) ^ (u->right == U->nil)) {
    snprintf(err, sizeof(err),
	     "expected right %s but found %s at key %d (depth %d)",
	     t->right == T->nil ? "NIL" : "non-NIL",
	     u->right == U->nil ? "NIL" : "non-NIL", t->key, depth);
    return err;
  }

  if (t->left != T->nil) {
    e = diff_tree(T, t->left, U, u->left, depth + 1);
    if (e) return e;
  }
  if (t->right != T->nil) {
    e = diff_tree(T, t->right, U, u->right, depth + 1);
    if (e) return e;
  }

  return NULL;
}

/* Replaces NULL children with sentinel. */
void set_nil(rb_tree_t *T, rb_node_t *x) {
  if (x->left && x->left != T->nil)
    set_nil(T, x->left);
  else
    x->left = T->nil;
  if (x->right && x->right != T->nil)
    set_nil(T, x->right);
  else
    x->right = T->nil;
}

/* Creates a new node with (optional) children. */
rb_node_t *node(int key, char red, rb_node_t *l, rb_node_t *r) {
  rb_node_t *x;

  x = malloc(sizeof(rb_node_t));
  x->key = key;
  x->value = 1000 + key;
  x->parent = NULL;
  x->left = l;
  x->right = r;
  x->red = red;

  if (x->left) x->left->parent = x;
  if (x->right) x->right->parent = x;

  return x;
}

/* Constructs tree from a node. */
rb_tree_t *tree(rb_node_t *root) {
  rb_tree_t *T;

  T = rb_new();
  if (root) {
    T->root = root;
    T->root->parent = T->nil;
    set_nil(T, T->root);
  }
  return T;
}

/* Returns a mirrored copy of T. */
#define MIRROR(T) mirror(T, T->root, NULL, NULL)
rb_tree_t *mirror(rb_tree_t *T, rb_node_t *t, rb_tree_t *N, rb_node_t *n) {

  if (!N) {
    N = tree(node(0, BLACK, NULL, NULL));
    n = N->root;
  }

  n->red = t->red;
  n->key = -1 * t->key;
  n->value = t->value;

  if (t->left != T->nil) {
    n->right = malloc(sizeof(rb_node_t));
    n->right->parent = n;
    mirror(T, t->left, N, n->right);
  } else
    n->right = N->nil;

  if (t->right != T->nil) {
    n->left = malloc(sizeof(rb_node_t));
    n->left->parent = n;
    mirror(T, t->right, N, n->left);
  } else
    n->left = N->nil;

  return N;
}

START_TEST (test_new) {
  rb_tree_t *T;

  T = rb_new();
  fail_unless(T->nil != NULL, "tree should have a sentinel node");
  fail_unless(T->root == T->nil, "empty tree has sentinel node as root");
  fail_unless(!T->nil->red, "sentinel must be black");
  fail_unless(T->nil->left == T->nil &&
	      T->nil->right == T->nil &&
	      T->nil->parent == T->nil,
	      "sentinel can't have family");
}
END_TEST

START_TEST (test_search) {
  rb_tree_t *T;

  /* empty tree */
  T = tree(NULL);
  fail_unless(rb_search(T, 17) == T->nil,
	      "searching in empty tree should give nil");

  /* non-empty tree */
  T = tree(node(5, BLACK,
		node(3, RED,
		     node(1, BLACK, NULL, NULL),
		     node(4, BLACK, NULL, NULL)),
		node(7, RED,
		     node(6, BLACK, NULL, NULL),
		     node(8, BLACK, NULL, NULL))));
  fail_unless(rb_search(T, 5) == T->root,
	      "search should have found the root");
  fail_unless(rb_search(T, 6) == T->root->right->left,
	      "search should have gone right-left");
  fail_unless(rb_search(T, 4) == T->root->left->right,
	      "search should have gone left-right");
  fail_unless(rb_search(T, 17) == T->nil,
	      "searching for key not in tree should give nil");
}
END_TEST

START_TEST (test_insert) {
  rb_tree_t *T, *E, *Tm;

  /* insertion into empty tree */
  T = tree(NULL);
  E = tree(node(4, BLACK, NULL, NULL));
  rb_insert(T, 4, 1004);
  DIFF(E, T);
  REDBLACK(T);

  /* insertion with black parent */
  T = tree(node(4, BLACK, NULL, NULL));
  E = tree(node(4, BLACK,
		node(2, RED, NULL, NULL),
		node(5, RED, NULL, NULL)));
  rb_insert(T, 5, 1005);
  rb_insert(T, 2, 1002);
  DIFF(E, T);
  REDBLACK(T);

  /* insertion with red parent and red uncle */
  T = tree(node(4, BLACK,
		node(2, RED, NULL, NULL),
		node(5, RED, NULL, NULL)));
  rb_insert(T, 3, 1001);
  E = tree(node(4, BLACK,
		node(2, BLACK,
		     NULL,
		     node(3, RED, NULL, NULL)),
		node(5, BLACK, NULL, NULL)));
  DIFF(E, T);
  REDBLACK(T);

  /* insertion with red parent and black uncle with uncle on "opposing side",
   * i.e. uncle = grandparent->left && newnode = parent->right or vice versa.
   * NB: T is not a valid red-black tree */
  T = tree(node(4, BLACK,
		node(2, RED,
		     NULL,
		     node(3, BLACK, NULL, NULL)),
		node(5, BLACK, NULL, NULL)));
  E = tree(node(2, BLACK,
		node(1, RED, NULL, NULL),
		node(4, RED,
		     node(3, BLACK, NULL, NULL),
		     node(5, BLACK, NULL, NULL))));
  rb_insert(T, 1, 1001);
  DIFF(E, T);

  /* insertion with red parent and black uncle on "same side" */
  T = tree(node(4, BLACK,
		node(2, RED, NULL, NULL),
		NULL));
  E = tree(node(3, BLACK,
		node(2, RED, NULL, NULL),
		node(4, RED, NULL, NULL)));
  Tm = MIRROR(T);
  rb_insert(T, 3, 1003);
  rb_insert(Tm, 3, 1003);
  DIFF(E, T);
  REDBLACK(T);
  REDBLACK(Tm);

  /* a bunch of insertion doesn't break red-blackness */
  T = tree(NULL); REDBLACK(T);
  rb_insert(T, 14, 1014); REDBLACK(T);
  rb_insert(T, 3, 1003); REDBLACK(T);
  rb_insert(T, 5, 1005); REDBLACK(T);
  rb_insert(T, 9, 1009); REDBLACK(T);
  rb_insert(T, 6, 1006); REDBLACK(T);
  rb_insert(T, 45, 1045); REDBLACK(T);
  rb_insert(T, 12, 1012); REDBLACK(T);
  rb_insert(T, 46, 1046); REDBLACK(T);
  rb_insert(T, 47, 1047); REDBLACK(T);
  rb_insert(T, 48, 1048); REDBLACK(T);
  rb_insert(T, 49, 1049); REDBLACK(T);
  rb_insert(T, 15, 1015); REDBLACK(T);
  rb_insert(T, 34, 1034); REDBLACK(T);
  rb_insert(T, 2, 1002); REDBLACK(T);
  rb_insert(T, 7, 1007); REDBLACK(T);
  rb_insert(T, 1, 1001); REDBLACK(T);
  rb_insert(T, 14, 1014); REDBLACK(T);
}
END_TEST

START_TEST (test_min_max) {
  rb_tree_t *T;

  /* empty tree */
  T = tree(NULL);
  fail_unless(rb_min(T, T->root) == T->nil,
	      "empty tree has no minimal element");
  fail_unless(rb_max(T, T->root) == T->nil,
	      "empty tree has no maximal element");

  /* almost empty tree */
  T = tree(node(12, BLACK, NULL, NULL));
  fail_unless(rb_min(T, T->root)->key == 12,
	      "this single element tree has min 12");
  fail_unless(rb_max(T, T->root)->key == 12,
	      "this single element tree has max 12");

  /* more normal tree*/
  T = tree(node(12, BLACK,
		node(7, RED, NULL, NULL),
		node(31, RED, NULL, NULL)));
  fail_unless(rb_min(T, T->root)->key == 7,
	      "this three-element tree has min 7");
  fail_unless(rb_max(T, T->root)->key == 31,
	      "this three-element tree has max 7");

  /* incrementally built tree */
  T = tree(NULL);
  rb_insert(T, 14, 1014);
  rb_insert(T, 3, 1003);
  rb_insert(T, 5, 1005);
  rb_insert(T, 9, 1009);
  rb_insert(T, 6, 1006);
  rb_insert(T, 45, 1045);
  rb_insert(T, 12, 1012);
  rb_insert(T, 46, 1046);
  rb_insert(T, 47, 1047);
  rb_insert(T, 15, 1015);
  rb_insert(T, 34, 1034);
  fail_unless(rb_min(T, T->root)->key == 3,
	      "minimum should be 3 at this point");
  fail_unless(rb_max(T, T->root)->key == 47,
	      "maximum should be 47 at this point");
}
END_TEST

START_TEST (test_next_prev) {
  rb_tree_t *T;

  /* single node tree */
  T = tree(node(7, BLACK, NULL, NULL));
  fail_unless(rb_next(T, T->root) == T->nil,
	      "in a single node tree the root has no successor");
  fail_unless(rb_prev(T, T->root) == T->nil,
	      "in a single node tree the root has no predecessor");

  /* tree with all cases */
  T = tree(node(7, BLACK,
		node(5, BLACK,
		     NULL,
		     node(6, BLACK, NULL, NULL)),
		node(9, BLACK,
		     node(8, BLACK, NULL, NULL),
		     NULL)));
  fail_unless(rb_next(T, T->root) == T->root->right->left,
              "next should be smallest element in right sub-tree");
  fail_unless(rb_prev(T, T->root) == T->root->left->right,
              "prev should be largest element in left sub-tree");
  fail_unless(rb_next(T, T->root->left->right) == T->root,
              "next should be first ancestor with left child also ancestor");
  fail_unless(rb_prev(T, T->root->right->left) == T->root,
              "prev should be first ancestor with right child also ancestor");
}
END_TEST

START_TEST (test_delete) {
  rb_tree_t *T, *E, *Tm;

  /* Deletions that shouldn't require fixup.
   */

  /* root with nil children */
  T = tree(node(4, BLACK, NULL, NULL));
  E = tree(NULL);

  rb_delete(T, T->root);
  DIFF(E, T);
  REDBLACK(T);

  /* root with 2 children and red successor */
  T = tree(node(3, BLACK,
		node(2, RED, NULL, NULL),
		node(4, RED, NULL, NULL)));
  E = tree(node(4, BLACK,
		node(2, RED, NULL, NULL),
		NULL));
  Tm = MIRROR(T);
  rb_delete(T, T->root);
  rb_delete(Tm, Tm->root);
  DIFF(E, T);
  REDBLACK(T);
  REDBLACK(Tm);

  /* red node with nil children */
  T = tree(node(4, BLACK,
		node(2, RED, NULL, NULL),
		NULL));
  E = tree(node(4, BLACK, NULL, NULL));
  Tm = MIRROR(T);
  rb_delete(T, T->root->left);
  rb_delete(Tm, Tm->root->right);
  DIFF(E, T);
  REDBLACK(T);
  REDBLACK(Tm);

  /* node with 1 child and red successor */
  T = tree(node(4, BLACK,
		node(2, BLACK, NULL, NULL),
		node(5, BLACK,
		     NULL,
		     node(6, RED, NULL, NULL))));
  E = tree(node(4, BLACK,
		node(2, BLACK, NULL, NULL),
		node(6, BLACK, NULL, NULL)));
  Tm = MIRROR(T);
  rb_delete(T, T->root->right);
  rb_delete(Tm, Tm->root->left);
  DIFF(E, T);
  REDBLACK(T);
  REDBLACK(Tm);

  /* node with 2 children and red successor */
  T = tree(node(4, BLACK,
		node(2, BLACK, NULL, NULL),
		node(6, BLACK,
		     node(5, RED, NULL, NULL),
		     node(7, RED, NULL, NULL))));
  E = tree(node(4, BLACK,
		node(2, BLACK, NULL, NULL),
		node(7, BLACK,
		     node(5, RED, NULL, NULL),
		     NULL)));
  Tm = MIRROR(T);
  rb_delete(T, T->root->right);
  rb_delete(Tm, Tm->root->left);
  DIFF(E, T);
  REDBLACK(T);
  REDBLACK(Tm);

  /* Deletions that require fixup.
   */

  /* root with 2 children and black successor (case 2) */
  T = tree(node(3, BLACK,
		node(2, BLACK, NULL, NULL),
		node(4, BLACK, NULL, NULL)));
  E = tree(node(4, BLACK,
		node(2, RED, NULL, NULL),
		NULL));
  Tm = MIRROR(T);
  rb_delete(T, T->root);
  rb_delete(Tm, Tm->root);
  DIFF(E, T);
  REDBLACK(T);
  REDBLACK(Tm);

  /* node with 2 children and black successor (cases 2, 1 and 2) */
  T = tree(node(9, BLACK,
		node(5, RED,
		     node(3, BLACK,
			  node(1, BLACK, NULL, NULL),
			  node(4, BLACK, NULL, NULL)),
		     node(7, BLACK,
			  node(6, BLACK, NULL, NULL),
			  node(8, BLACK, NULL, NULL))),
		node(13, BLACK,
		     node(12, BLACK, NULL, NULL),
		     node(14, BLACK, NULL, NULL))));
  E = tree(node(5, BLACK,
		node(3, BLACK,
		     node(1, BLACK, NULL, NULL),
		     node(4, BLACK, NULL, NULL)),
		node(9, BLACK,
		     node(7, RED,
			  node(6, BLACK, NULL, NULL),
			  node(8, BLACK, NULL, NULL)),
		     node(14, BLACK,
			  node(12, RED, NULL, NULL),
			  NULL))));
  Tm = MIRROR(T);
  rb_delete(T, T->root->right);
  rb_delete(Tm, Tm->root->left);
  DIFF(E, T);
  REDBLACK(T);
  REDBLACK(Tm);

  /* node with 2 children and black successor (case 4) */
  T = tree(node(4, BLACK,
		node(2, BLACK,
		     node(1, BLACK, NULL, NULL),
		     node(3, BLACK, NULL, NULL)),
		node(6, BLACK,
		     node(5, BLACK, NULL, NULL),
		     node(8, RED,
			  node(7, BLACK, NULL, NULL),
			  node(9, BLACK, NULL, NULL)))));
  E = tree(node(6, BLACK,
		node(4, BLACK,
		     node(3, BLACK,
			  node(1, RED, NULL, NULL),
			  NULL),
		     node(5, BLACK, NULL, NULL)),
		node(8, BLACK,
		     node(7, BLACK, NULL, NULL),
		     node(9, BLACK, NULL, NULL))));
  Tm = MIRROR(T);
  rb_delete(T, T->root->left);
  rb_delete(Tm, Tm->root->right);
  DIFF(E, T);
  REDBLACK(T);
  REDBLACK(Tm);

  /* node with 2 children and black successor (case 3, case 4) */
  T = tree(node(4, BLACK,
		node(2, BLACK,
		     node(1, BLACK, NULL, NULL),
		     node(3, BLACK, NULL, NULL)),
		node(8, BLACK,
		     node(6, RED,
			  node(5, BLACK, NULL, NULL),
			  node(7, BLACK, NULL, NULL)),
		     node(9, BLACK, NULL, NULL))));
  E = tree(node(6, BLACK,
		node(4, BLACK,
		     node(3, BLACK,
			  node(1, RED, NULL, NULL),
			  NULL),
		     node(5, BLACK, NULL, NULL)),
		node(8, BLACK,
		     node(7, BLACK, NULL, NULL),
		     node(9, BLACK, NULL, NULL))));
  Tm = MIRROR(T);
  rb_delete(T, T->root->left);
  rb_delete(Tm, Tm->root->right);
  DIFF(E, T);
  REDBLACK(T);
  REDBLACK(Tm);

  /* Some tree and several deletions. */
  T = tree(NULL);
  rb_insert(T, 14, 1014);
  rb_insert(T, 3, 1003);
  rb_insert(T, 5, 1005);
  rb_insert(T, 9, 1009);
  rb_insert(T, 6, 1006);
  rb_insert(T, 45, 1045);
  rb_insert(T, 12, 1012);
  rb_insert(T, 46, 1046);
  rb_insert(T, 47, 1047);
  rb_insert(T, 15, 1015);
  rb_insert(T, 34, 1034);
  rb_delete(T, rb_search(T, 14));
  fail_unless(rb_search(T, 14) == T->nil, "deleted node shouldn't be in tree");
  rb_delete(T, rb_search(T, 12));
  fail_unless(rb_search(T, 12) == T->nil, "deleted node shouldn't be in tree");
  rb_delete(T, rb_search(T, 47));
  fail_unless(rb_search(T, 47) == T->nil, "deleted node shouldn't be in tree");
  rb_delete(T, rb_search(T, 46));
  fail_unless(rb_search(T, 46) == T->nil, "deleted node shouldn't be in tree");
  rb_delete(T, rb_search(T, 3));
  fail_unless(rb_search(T, 3) == T->nil, "deleted node shouldn't be in tree");
  rb_delete(T, rb_search(T, 9));
  fail_unless(rb_search(T, 9) == T->nil, "deleted node shouldn't be in tree");
  rb_delete(T, rb_search(T, 45));
  fail_unless(rb_search(T, 45) == T->nil, "deleted node shouldn't be in tree");
  REDBLACK(T);
}
END_TEST


Suite *rbtree_suite() {
  TCase *tc;
  Suite *s;

  s = suite_create ("rbtree");

  tc = tcase_create ("new");
  tcase_add_test (tc, test_new);
  suite_add_tcase (s, tc);

  tc = tcase_create ("search");
  tcase_add_test (tc, test_search);
  suite_add_tcase (s, tc);

  tc = tcase_create ("insert");
  tcase_add_test (tc, test_insert);
  suite_add_tcase (s, tc);

  tc = tcase_create ("min_max");
  tcase_add_test (tc, test_min_max);
  suite_add_tcase (s, tc);

  tc = tcase_create ("next_prev");
  tcase_add_test (tc, test_next_prev);
  suite_add_tcase (s, tc);

  tc = tcase_create ("delete");
  tcase_add_test (tc, test_delete);
  suite_add_tcase (s, tc);

  return s;
}

int main(void) {
  int number_failed;
  Suite *s = rbtree_suite();
  SRunner *sr = srunner_create(s);
  srunner_run_all (sr, CK_NORMAL);
  number_failed = srunner_ntests_failed (sr);
  srunner_free (sr);
  return (number_failed == 0) ? 0 : 1;
}
