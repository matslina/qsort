#include <stdlib.h>
#include <stdio.h>
#include <check.h>
#include "btree.h"

bt_node_t *new(int key, bt_node_t *l, bt_node_t *r) {
  bt_node_t *x;

  x = malloc(sizeof(bt_node_t));
  x->key = key;
  x->value = key + 1000;
  x->parent = NULL;
  x->left = l;
  x->right = r;
  if (l) l->parent = x;
  if (r) r->parent = x;

  return x;
}

#define LARGE_ENOUGH 2048
int test_walk_key[LARGE_ENOUGH];
int test_walk_depth[LARGE_ENOUGH];
int test_walk_count;
int test_walk_stop;

int test_walk_callback(bt_node_t *node, int depth) {
  test_walk_key[test_walk_count] = node->key;
  test_walk_depth[test_walk_count] = depth;
  test_walk_count++;
  return node->key == test_walk_stop;
}

START_TEST (test_walk_bf) {

  /* trivial single-node tree */
  test_walk_count = 0;
  bt_walk_bf(new(5, NULL, NULL), test_walk_callback);
  fail_unless(test_walk_count == 1, "only the root should have been visited");
  fail_unless(test_walk_key[0] == 5, "root should have key 5");
  fail_unless(test_walk_depth[0] == 0, "root should be at depth 0");

  /* non-trivial 7-node tree. */
  int i;
  int exp_key[] =   {1,7,3,2,5,4,6};
  int exp_depth[] = {0,1,2,3,3,4,4};
  bt_node_t *T = T = new(1,
			 NULL,
			 new(7,
			     NULL,
			     new(3,
				 new(2, NULL, NULL),
				 new(5,
				     new(4, NULL, NULL),
				     new(6, NULL, NULL)))));

  test_walk_count = 0;
  bt_walk_bf(T, test_walk_callback);

  fail_unless(test_walk_count == 7, "walk should span all 7 nodes");

  for (i=0; i<test_walk_count; i++) {
    fail_unless(test_walk_key[i] == exp_key[i],
		"expected key %d at step %d of walk, got %d",
		exp_key[i], i, test_walk_key[i]);
    fail_unless(test_walk_depth[i] == exp_depth[i],
		"expected node %d of walk to be at depth %d, got %d",
		i, exp_depth[i], test_walk_depth[i]);
  }

  /* same tree but stop half-way through */
  test_walk_count = 0;
  test_walk_stop = 2;
  bt_walk_bf(T, test_walk_callback);

  fail_unless(test_walk_count == 4, "walk should have stopped after 4 nodes");
  for (i=0; i<test_walk_count; i++) {
    fail_unless(test_walk_key[i] == exp_key[i],
		"expected key %d at step %d of walk, got %d",
		exp_key[i], i, test_walk_key[i]);
    fail_unless(test_walk_depth[i] == exp_depth[i],
		"expected node %d of walk to be at depth %d, got %d",
		i, exp_depth[i], test_walk_depth[i]);
  }
}
END_TEST

Suite *btree_suite() {
  TCase *tc;
  Suite *s;

  s = suite_create ("btree");

  tc = tcase_create ("walk_bf");
  tcase_add_test (tc, test_walk_bf);
  suite_add_tcase (s, tc);

  return s;
}

int main(void) {
  int number_failed;
  Suite *s = btree_suite();
  SRunner *sr = srunner_create(s);
  srunner_run_all (sr, CK_NORMAL);
  number_failed = srunner_ntests_failed (sr);
  srunner_free (sr);
  return (number_failed == 0) ? 0 : 1;
}
