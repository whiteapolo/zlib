#ifndef AVL_H
#define AVL_H

typedef struct Z_Avl_Node {
  struct Z_Avl_Node *left;
  struct Z_Avl_Node *right;
  void *key;
  void *value;
  char height;
} Z_Avl_Node;

#endif