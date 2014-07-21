#ifndef RUNTIME_TREE_H_
#define RUNTIME_TREE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "tree_sitter/runtime.h"

typedef enum {
  TSTreeOptionsHidden = 1,
  TSTreeOptionsExtra = 2,
  TSTreeOptionsWrapper = 4,
} TSTreeOptions;

struct TSTree {
  TSSymbol symbol;
  TSTreeOptions options;
  size_t ref_count;
  size_t offset;
  size_t size;
  union {
    struct {
      size_t child_count;
      struct TSTree **children;
      size_t visible_child_count;
    };
    struct {
      size_t expected_input_count;
      const TSSymbol *expected_inputs;
      char lookahead_char;
    };
  };
};

typedef struct {
  TSTree *tree;
  size_t position;
} TSChildWithPosition;

static inline int ts_tree_is_extra(const TSTree *tree) {
  return (tree->options & TSTreeOptionsExtra);
}

static inline int ts_tree_is_visible(const TSTree *tree) {
  return !(tree->options & TSTreeOptionsHidden);
}

static inline void ts_tree_set_extra(TSTree *tree) {
  tree->options = (TSTreeOptions)(tree->options | TSTreeOptionsExtra);
}

static inline int ts_tree_is_wrapper(const TSTree *tree) {
  return (tree->options & TSTreeOptionsWrapper);
}

static inline size_t ts_tree_visible_child_count(const TSTree *tree) {
  if (tree->symbol == ts_builtin_sym_error)
    return 0;
  else
    return tree->visible_child_count;
}

static inline TSChildWithPosition * ts_tree_visible_children(const TSTree *tree, size_t *count) {
  if (tree->symbol == ts_builtin_sym_error || tree->visible_child_count == 0) {
    if (count) *count = 0;
    return NULL;
  } else {
    if (count) *count = tree->visible_child_count;
    return (TSChildWithPosition *)(tree + 1);
  }
}

TSTree * ts_tree_make_leaf(TSSymbol symbol, size_t size, size_t offset, int is_hidden);
TSTree * ts_tree_make_node(TSSymbol symbol, size_t child_count, TSTree **children, int is_hidden);
TSTree * ts_tree_make_error(char lookahead_char, size_t expected_input_count, const TSSymbol *expected_inputs, size_t size, size_t offset);
void ts_tree_retain(TSTree *tree);
void ts_tree_release(TSTree *tree);
int ts_tree_equals(const TSTree *tree1, const TSTree *tree2);
char * ts_tree_string(const TSTree *tree, const char **names);
char * ts_tree_error_string(const TSTree *tree, const char **names);
TSTree ** ts_tree_children(const TSTree *tree, size_t *count);
size_t ts_tree_total_size(const TSTree *tree);

#ifdef __cplusplus
}
#endif

#endif  // RUNTIME_TREE_H_
