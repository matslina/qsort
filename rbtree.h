#ifndef RBTREE_H_12j312j31j23j123k1
#define RBTREE_H_12j312j31j23j123k1

typedef int rb_key_t;
typedef int rb_val_t;

#define rb_key_cmp(a,b) ((a)-(b))

typedef struct rb_node_s rb_node_t;
typedef struct rb_tree_s rb_tree_t;

struct rb_node_s {
  rb_key_t key;
  rb_val_t value;
  rb_node_t *left;
  rb_node_t *right;
  rb_node_t *parent;
  char red;
};

struct rb_tree_s {
  rb_node_t *root;
  rb_node_t *nil;
};

rb_tree_t *rb_new();
rb_node_t *rb_search(rb_tree_t *T, rb_key_t key);
rb_node_t *rb_insert(rb_tree_t *T, rb_key_t key, rb_val_t value);
rb_node_t *rb_min(rb_tree_t *T, rb_node_t *t);
rb_node_t *rb_max(rb_tree_t *T, rb_node_t *t);
rb_node_t *rb_next(rb_tree_t *T, rb_node_t *t);
rb_node_t *rb_prev(rb_tree_t *T, rb_node_t *t);
void rb_delete(rb_tree_t *T, rb_node_t *t);

#endif
