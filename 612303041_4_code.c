#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define RED 1
#define BLACK 0

typedef struct node {
	time_t timestamp;
	struct node *left;
	struct node *right;
	struct node *parent;
	int color;
} node;

typedef struct rb_tree {
	node *root;
} rb_tree;

// Create a new red node with a given timestamp
node *create_node(time_t timestamp) {
	node *n = (node *) malloc(sizeof(node));

	// Initialize node fields
	n -> timestamp = timestamp;
	n -> color = RED;
	n -> left = n -> right = n -> parent = NULL;

	return n;
}

// Perform a left rotation around node x
node *left_rotate(node *x) {
	node *y = x -> right;

	// If no right child, no rotation
	if(!y)
		return x;

	// Move y's left subtree to x's right
	x -> right = y -> left;
	if(y -> left)
		y -> left -> parent = x;

	// Update y's parent to x's parent
	y -> parent = x -> parent;

	// Connect y to x's parent
	if(x -> parent) {
		if(x == x -> parent -> left)
			x -> parent -> left = y;
		else
			x -> parent -> right = y;
	}

	// Put x as left child of y
	y -> left = x;
	x -> parent = y;

	return y;
}

// Perform a right rotation around node x
node *right_rotate(node *x) {
	node *y = x -> left;

	// If no left child, no rotation
	if(!y)
		return x;

	// Move y's right subtree to x's left
	x -> left = y -> right;
	if(y -> right)
		y -> right -> parent = x;

	// Update y's parent to x's parent
	y -> parent = x -> parent;

	// Connect y to x's parent
	if(x -> parent) {
		if(x == x -> parent -> left)
			x -> parent -> left = y;
		else
			x -> parent -> right = y;
	}

	// Put x as right child of y
	y -> right = x;
	x -> parent = y;

	return y;
}

// Fix Red-Black Tree properties after inserting a node
void fixup(rb_tree *rbt, node *z) {
	// Repeat until parent is black or we reach the root
	while(z -> parent && z -> parent -> color == RED) {
		node *gp = z -> parent -> parent;

		// Case 1: Parent is left child of grandparent
		if(z -> parent == gp -> left) {
			node *uncle = gp -> right;

			// Uncle is red: recolor and move z up the tree
			if(uncle && uncle -> color == RED) {
				z -> parent -> color = BLACK;
				uncle -> color = BLACK;
				gp -> color = RED;
				z = gp;
			}
			
			// Uncle is black: rotation + recolor
			else {
				if(z == z -> parent -> right) {
					z = z -> parent;
					left_rotate(z);
				}
				z -> parent -> color = BLACK;
				gp -> color = RED;
				right_rotate(gp);
			}
		}
		// Case 2: Parent is right child of grandparent
		else {
			node *uncle = gp -> left;

			// Uncle is red: recolor and move z up
			if(uncle && uncle -> color == RED) {
				z -> parent -> color = BLACK;
				uncle -> color = BLACK;
				gp -> color = RED;
				z = gp;
			}
			
			// Uncle is black: rotation + recolor
			else {
				if(z == z -> parent -> left) {
					z = z -> parent;
					right_rotate(z);
				}
				z -> parent -> color = BLACK;
				gp -> color = RED;
				left_rotate(gp);
			}
		}
	}

	// make rbt -> root point to the root if not
	while(rbt -> root -> parent)
		rbt -> root = rbt -> root -> parent;

	// Root must always be black
	rbt -> root -> color = BLACK;
}

// Insert a new node with given timestamp into the Red-Black Tree
node *insert_node(rb_tree *rbt, time_t timestamp) {
	node *z = create_node(timestamp);
	node *p = rbt -> root;
	node *q = NULL;

	// Standard BST insert
	while(p != NULL) {
		q = p;
		if(timestamp < p -> timestamp)
			p = p -> left;
		else if(timestamp > p -> timestamp)
			p = p -> right;
		else {
			free(z);
			return rbt -> root;
		}
	}

	// Attach new node to the found parent
	z -> parent = q;
	if(q == NULL)
		rbt -> root = z;
	else if(timestamp < q -> timestamp)
		q -> left = z;
	else
		q -> right = z;

	// Fix color and balance violations
	fixup(rbt, z);

	return rbt -> root;
}

// Search for a node with a specific timestamp (BST search)
node *search_node(node *root, time_t timestamp) {
	if(!root)
		return NULL;

	if(timestamp < root -> timestamp)
		return search_node(root -> left, timestamp);
	else if(timestamp > root -> timestamp)
		return search_node(root -> right, timestamp);
	else
		return root;
}

// Inorder traversal, prints nodes in ascending order
void inorder(node *root) {
	if(!root)
		return;

	inorder(root -> left);
	printf("%s", ctime(&root -> timestamp));
	inorder(root -> right);
}

// Preorder traversal, root first, then subtrees
void preorder(node *root) {
	if(!root)
		return;

	printf("%s", ctime(&root -> timestamp));
	preorder(root -> left);
	preorder(root -> right);
}

// Postorder traversal — subtrees first, then root
void postorder(node *root) {
	if(!root)
		return;

	postorder(root -> left);
	postorder(root -> right);
	printf("%s", ctime(&root -> timestamp));
}

// Free all nodes in the tree recursively
void free_tree(node *root) {
	if(!root)
		return;

	free_tree(root -> left);
	free_tree(root -> right);
	free(root);
}

int main() {
	rb_tree rbt;
	rbt.root = NULL;

	// Seed random number generator
	srand(time(NULL));

	// Get current time as base timestamp
	time_t now = time(NULL);

	// Insert 10 random timestamps into the Red-Black Tree
	for(int i = 0; i < 10; i++) {
		rbt.root = insert_node(&rbt, now + rand() % 1000000);
	}

	// Display preorder traversal
	printf("Preorder Traversal:\n");
	preorder(rbt.root);

	// Display inorder traversal (sorted by timestamp)
	printf("\n\nInorder (Sorted by timestamp):\n");
	inorder(rbt.root);

	// Display postorder traversal
	printf("\n\nPostorder Traversal:\n");
	postorder(rbt.root);
	printf("\n");

	// Free allocated memory
	free_tree(rbt.root);

	return 0;
}

