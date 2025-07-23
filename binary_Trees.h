#ifndef BINARY_TREES_H
#define BINARY_TREES_H

// Binary tree structure
typedef struct Node {
    char *val;
    struct Node *left;
    struct Node *right;
} Node, *Tree;

// Function declarations
Node *allocate_node(char *s);
void free_tree(Tree *A);
void construct_tree(Tree *has);
int serialize(char *filename, Tree A);
int deserialize(char *filename, Tree *A);
int expansion(Tree *A, Tree B);
Tree create_tree_from_input(char *input);
void search_node(Tree root, char *value, Node **found);

#endif /* BINARY_TREES_H */
