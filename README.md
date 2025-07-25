1. Data structures and headers
Node and Tree (in binary_Trees.h)

c
Copy
Edit
typedef struct Node {
    char *val;
    struct Node *left;
    struct Node *right;
} Node, *Tree;
A Node holds a string val and pointers to left/right subtrees.

A Tree is just a pointer to a Node.

Function declarations

binary_Trees.h declares core tree operations: allocation, freeing, (de)serialization, grafting (expansion), building from input, and search.

graft.h re–exports expansion and adds a generic copy() that clones a tree.

saage.h declares just serialize and deserialize (same prototypes as in binary_Trees.h).

2. Building and destroying trees
allocate_node(char *s)
c
Copy
Edit
Node *new_node = malloc(sizeof(Node));
new_node->val   = strdup(s);
new_node->left  = NULL;
new_node->right = NULL;
Allocates memory for a node, copies the string s, and initializes children to NULL.

free_tree(Tree *A)
c
Copy
Edit
if (*A == NULL) return;
free((*A)->val);
free_tree(&(*A)->left);
free_tree(&(*A)->right);
free(*A);
Recursively frees the entire subtree, including each node’s string.

3. Constructing a tree from user input
construct_tree(Tree *has)
Prompts the user to enter a space‑separated list in prefix (“depth-first”) format, e.g. A B 0 0 C 0 0 (where 0 denotes “no node”).

Calls create_tree_from_input() to parse that string into a Tree.

create_tree_from_input(char *input)
c
Copy
Edit
token = strtok(input, " \n");
if (!token || strcmp(token, "0")==0) return NULL;

root = allocate_node(token + 1);
root->left  = create_tree_from_input(NULL);
root->right = create_tree_from_input(NULL);
Uses strtok to walk the sequence.

A token "0" means “no node here” → return NULL.

Otherwise it strips off a leading character (e.g. a marker) and makes a node; then recurses for left and right children.

4. Serialization and deserialization
serialize(char *filename, Tree A)
c
Copy
Edit
FILE *f = fopen(filename, "w");
if (A) {
  fprintf(f, "Value: %s\n", A->val);
  fprintf(f, "LEFT :\n");
  serialize(filename, A->left);
  fprintf(f, "RIGHT :\n");
  serialize(filename, A->right);
}
fclose(f);
Traverses pre‑order, writing each node’s value and markers for left/right.

Bug note: It reopens the same file on each recursive call instead of using the already‑open FILE*, so it actually overwrites previous content. (You’d normally pass FILE* down rather than the filename.)

deserialize(char *filename, Tree *A)
c
Copy
Edit
FILE *f = fopen(filename, "r");
fscanf(f, "Value: %[^\n]\n", val);
*A = allocate_node(val);
// if next marker is “LEFT :” then recursively deserialize into (*A)->left
// likewise for “RIGHT :”
fclose(f);
Reads nodes in the same pre‑order.

Uses fscanf to detect the markers and rebuild the tree.

5. Copying and grafting (“expansion”)
copy(Tree *dest, Tree source)
Recursively clones source into *dest by:

Allocating a new node with the same string.

Recursively copying left and right subtrees.

Returns 1 on success, 0 on failure (and frees any partial work).

expansion(Tree *A, Tree B)
Find in tree *A a node whose label matches B->val, using search_node().

Clone all of B into copy_B.

Graft:

Attach copy_B->left onto the found node’s existing left subtree, and same for the right.

Replace the found node’s label with B->val.

Return 1 on success, 0 if anything fails.

c
Copy
Edit
Node *found = NULL;
search_node(*A, B->val, &found);
copy(&copy_B, B);
copy_B->left  = found->left;   found->left  = copy_B->left;
copy_B->right = found->right;  found->right = copy_B->right;
free(found->val);
found->val = strdup(B->val);
6. Searching
search_node(Tree root, char *value, Node **found)
Standard pre‑order search: if root->val matches value, writes the address into *found; otherwise recurses left then right.

7. main(): two modes
c
Copy
Edit
if (argv[1] == "-E") {
  // “Export” mode: read a tree from stdin → serialize it to a .saage file
} else if (argv[1] == "-G") {
  // “Graft” mode: read two .saage files → graft the second onto the first → write “result.wise”
} else {
  // usage error
}
-E source.saage

Builds a tree from your console input.

Serializes it into source.saage.

-G source.saage graft.saage

Deserializes both files into two trees.

Calls expansion(&source_tree, graft_tree).

Serializes the modified source_tree to result.wise.

In a nutshell
You can export any prefix‑formatted tree into a file.

You can graft one tree onto another by matching labels.

Core operations: allocate/free, (de)serialize, search, copy, and graft.
