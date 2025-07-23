#ifndef SAAGE_H
#define SAAGE_H

#include "binary_trees.h"  // Include the header file containing the binary tree structure

// Function declarations for serialization and deserialization
int serialize(char *filename, Tree A);
int deserialize(char *filename, Tree *A);

#endif /* SAAGE_H */
