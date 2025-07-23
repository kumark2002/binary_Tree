#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "binary_Trees.h"
#include "graft.h"
#include "saage.h"

// Function declarations
Node *allocate_node(char *s);
void free_tree(Tree *A);
void construct_tree(Tree *has);
int serialize(char *filename, Tree A);
int deserialize(char *filename, Tree *A);
int expansion(Tree *A, Tree B);
Tree create_tree_from_input(char *input);
void search_node(Tree root, char *value, Node **found);

// Allocate memory for a node
Node *allocate_node(char *s) {
    Node *new_node = (Node *)malloc(sizeof(Node));
    if (new_node != NULL) {
        new_node->val = strdup(s);
        new_node->left = NULL;
        new_node->right = NULL;
    }
    return new_node;
}

// Free memory used by a tree
void free_tree(Tree *A) {
    if (*A == NULL) return;
    free((*A)->val);
    free_tree(&((*A)->left));
    free_tree(&((*A)->right));
    free(*A);
}

// Construct a tree from user input
void construct_tree(Tree *has) {
    printf("Enter the tree nodes in prefix depth traversal format:\n");
    char input[100];
    fgets(input, sizeof(input), stdin);
    *has = create_tree_from_input(input);
}

// Serialize a tree to a file
int serialize(char *filename, Tree A) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) return 0;
    if (A != NULL) {
        fprintf(file, "Value: %s\n", A->val);
        fprintf(file, "LEFT :\n");
        serialize(filename, A->left);
        fprintf(file, "RIGHT :\n");
        serialize(filename, A->right);
    }
    fclose(file);
    return 1;
}

// Deserialize a tree from a file
int deserialize(char *filename, Tree *A) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) return 0;
    char val[100];
    if (fscanf(file, "Value: %[^\n]\n", val) != 1) {
        fclose(file);
        return 0;
    }
    *A = allocate_node(val);
    if (*A == NULL) {
        fclose(file);
        return 0;
    }

    if (fscanf(file, "LEFT :\n") != EOF) {
        deserialize(filename, &((*A)->left));
    }
    if (fscanf(file, "RIGHT :\n") != EOF) {
        deserialize(filename, &((*A)->right));
    }
    fclose(file);
    return 1;
}

// Copy the source tree to destination
int copy(Tree *dest, Tree source) {
    if (source == NULL)
        return 0;

    *dest = allocate_node(source->val);
    if (*dest == NULL)
        return 0;

    // Recursively copy left and right subtrees
    if (!copy(&((*dest)->left), source->left) || !copy(&((*dest)->right), source->right)) {
        free_tree(dest);
        return 0;
    }

    return 1;
}

// Apply graft operation: graft tree B onto tree A
int expansion(Tree *A, Tree B) {
    if (*A == NULL || B == NULL)
        return 0;

    // Search for the node with the same label as the root of B in A
    Node *found = NULL;
    search_node(*A, B->val, &found);

    if (found == NULL)
        return 0;

    // Copy the tree B
    Tree copy_B = NULL;
    if (!copy(&copy_B, B))
        return 0;

    // Graft the left subtree of the found node with the left subtree of B
    copy_B->left = found->left;
    found->left = copy_B->left;

    // Graft the right subtree of the found node with the right subtree of B
    copy_B->right = found->right;
    found->right = copy_B->right;

    // Update the label of the found node to be the label of B
    free(found->val);
    found->val = strdup(B->val);

    return 1;
}

// Create a tree from input string
Tree create_tree_from_input(char *input) {
    char *token = strtok(input, " \n");
    if (token == NULL || strcmp(token, "0") == 0) {
        return NULL;
    }

    Tree root = allocate_node(token + 1);
    if (root == NULL) {
        return NULL;
    }

    root->left = create_tree_from_input(NULL);
    root->right = create_tree_from_input(NULL);

    return root;
}

// Search for a node with given value in the tree
void search_node(Tree root, char *value, Node **found) {
    if (root == NULL) {
        return;
    }
    if (strcmp(root->val, value) == 0) {
        *found = root;
        return;
    }
    search_node(root->left, value, found);
    search_node(root->right, value, found);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s [-E | -G] source_file.saage [graft_file.saage]\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "-E") == 0) {
        // Create a backup of the tree entered by the user
        Tree tree = NULL;
        construct_tree(&tree);
        serialize(argv[2], tree);
        free_tree(&tree);
    } else if (strcmp(argv[1], "-G") == 0) {
        if (argc < 4) {
            printf("Usage: %s -G source_file.saage graft_file.saage\n", argv[0]);
            return 1;
        }
        // Apply the graft and display the result
        Tree source_tree = NULL;
        if (!deserialize(argv[2], &source_tree)) {
            printf("Error: Failed to read source tree from file %s\n", argv[2]);
            return 1;
        }

        Tree graft_tree = NULL;
        if (!deserialize(argv[3], &graft_tree)) {
            printf("Error: Failed to read graft tree from file %s\n", argv[3]);
            free_tree(&source_tree);
            return 1;
        }

        if (!expansion(&source_tree, graft_tree)) {
            printf("Error: Grafting operation failed\n");
            free_tree(&source_tree);
            free_tree(&graft_tree);
            return 1;
        }

        // Display the result
        serialize("result.wise", source_tree);

        // Free memory
        free_tree(&source_tree);
        free_tree(&graft_tree);
    } else {
        printf("Invalid option: %s\n", argv[1]);
        return 1;
    }

    return 0;
}
