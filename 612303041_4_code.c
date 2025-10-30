#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define RED 1
#define BLACK 0

typedef struct node {
	time_t timestamp;
	struct node *left, *right, *parent;
	// 1 -> Red, 0 -> Black
	int color;
} node;

typedef struct rb_tree {
	node *root;
} rb_tree;

// Create a new red node with a given timestamp
node *create_node(time_t timestamp) {
	node *n = (node *) malloc(sizeof(node));
	n -> timestamp = timestamp;
	n -> color = RED;
	n -> left = n -> right = n -> parent = NULL;
	return n;
}

// Left rotation around node x
node *left_rotate(node *x) {
	node *y = x -> right;
	if (!y) return x;

	x -> right = y -> left;
	if (y -> left)
		y -> left -> parent = x;

	y -> parent = x -> parent;
	if (x -> parent) {
		if (x == x -> parent -> left)
			x -> parent -> left = y;
		else
			x -> parent -> right = y;
	}
	y -> left = x;
	x -> parent = y;
	return y;
}

// Right rotation around node x
node *right_rotate(node *x) {
	node *y = x -> left;
	if (!y) return x;

	x -> left = y -> right;
	if (y -> right)
		y -> right -> parent = x;

	y -> parent = x -> parent;
	if (x -> parent) {
		if (x == x -> parent -> left)
			x -> parent -> left = y;
		else
			x -> parent -> right = y;
	}
	y -> right = x;
	x -> parent = y;
	return y;
}

// Fix Red-Black Tree properties after insertion
void fixup(rb_tree *rbt, node *z) {
	while (z -> parent && z -> parent -> color == RED) {
		node *gp = z -> parent -> parent;  // grandparent
		if (z -> parent == gp -> left) {
			node *uncle = gp -> right;  // uncle node
			if (uncle && uncle -> color == RED) {
				// Case 1: Uncle is red — recolor
				z -> parent -> color = BLACK;
				uncle -> color = BLACK;
				gp -> color = RED;
				z = gp;
			} else {
				// Case 2/3: Uncle is black — rotation and recolor
				if (z == z -> parent -> right) {
					z = z -> parent;
					left_rotate(z);
				}
				z -> parent -> color = BLACK;
				gp -> color = RED;
				right_rotate(gp);
			}
		} else {
			node *uncle = gp -> left;
			if (uncle && uncle -> color == RED) {
				// Mirror Case 1: Recolor if uncle is red
				z -> parent -> color = BLACK;
				uncle -> color = BLACK;
				gp -> color = RED;
				z = gp;
			} else {
				// Mirror Case 2/3: Rotation and recolor
				if (z == z -> parent -> left) {
					z = z -> parent;
					right_rotate(z);
				}
				z -> parent -> color = BLACK;
				gp -> color = RED;
				left_rotate(gp);
			}
		}
	}
	// Move up to the root and ensure it's black
	while (rbt -> root -> parent)
		rbt -> root = rbt -> root -> parent;
	rbt -> root -> color = BLACK;
}

// Insert a node with given timestamp into the red-black tree
node *insert_node(rb_tree *rbt, time_t timestamp) {
	node *z = create_node(timestamp);
	node *p = rbt -> root;
	node *q = NULL;

	// Standard BST insert
	while (p != NULL) {
		q = p;
		if (timestamp < p -> timestamp)
			p = p -> left;
		else if (timestamp > p -> timestamp)
			p = p -> right;
		else {
			// Duplicate timestamps are ignored
			free(z);
			return rbt -> root;
		}
	}

	// Link new node to its parent
	z -> parent = q;
	if (q == NULL)
		rbt -> root = z;  // Tree was empty
	else if (timestamp < q -> timestamp)
		q -> left = z;
	else
		q -> right = z;

	// Fix potential color violations
	fixup(rbt, z);
	return rbt -> root;
}

// Search a node by timestamp (standard BST search)
node *search_node(node *root, time_t timestamp) {
	if (!root)
		return NULL;
	if (timestamp < root -> timestamp)
		return search_node(root -> left, timestamp);
	else if (timestamp > root -> timestamp)
		return search_node(root -> right, timestamp);
	else
		return root;
}

// Inorder traversal (ascending order by timestamp)
void inorder(node *root) {
	if (!root) return;
	inorder(root -> left);
	printf("%s", ctime(&root -> timestamp));
	inorder(root -> right);
}

// Preorder traversal (root-left-right)
void preorder(node *root) {
	if (!root) return;
	printf("%s", ctime(&root -> timestamp));
	preorder(root -> left);
	preorder(root -> right);
}

// Postorder traversal (left-right-root)
void postorder(node *root) {
	if (!root) return;
	postorder(root -> left);
	postorder(root -> right);
	printf("%s", ctime(&root -> timestamp));
}

// Recursively free all nodes in the tree
void free_tree(node *root) {
	if (!root) return;
	free_tree(root -> left);
	free_tree(root -> right);
	free(root);
}

int main() {
	rb_tree rbt;
	rbt.root = NULL;
	srand(time(NULL));

	time_t now = time(NULL);

	// Insert random timestamps into the tree
	for (int i = 0; i < 10; i++) {
		rbt.root = insert_node(&rbt, now + rand() % 1000000);
	}

	printf("Preorder Traversal:\n");
	preorder(rbt.root);
	printf("\n\nInorder (Sorted by timestamp):\n");
	inorder(rbt.root);
	printf("\n\nPostorder Traversal:\n");
	postorder(rbt.root);
	printf("\n");

	free_tree(rbt.root);
	return 0;
}
