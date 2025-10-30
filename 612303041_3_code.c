#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct node {
	char month[20];
	int height;
	struct node *left, *right;
} node;

int max(int a, int b) {
	return a > b ? a : b;
}

// Get height of node
int height(node *n) {
	return n ? n -> height : 0;
}

// Create a new AVL node
node *create_node(const char *month) {
	node *n = (node *) malloc(sizeof(node));
	strcpy(n -> month, month);
	n -> left = n -> right = NULL;
	n -> height = 1;
	return n;
}

// Right rotate around y
node *right_rotate(node *y) {
	node *x = y -> left;
	node *t2 = x -> right;

	x -> right = y;
	y -> left = t2;

	y -> height = max(height(y -> left), height(y -> right)) + 1;
	x -> height = max(height(x -> left), height(x -> right)) + 1;

	return x;
}

// Left rotate around x
node *left_rotate(node *x) {
	node *y = x -> right;
	node *t2 = y -> left;

	y -> left = x;
	x -> right = t2;

	x -> height = max(height(x -> left), height(x -> right)) + 1;
	y -> height = max(height(y -> left), height(y -> right)) + 1;

	return y;
}

// Get balance factor of a node
int get_balance(node *n) {
	if (!n)
		return 0;
	return height(n -> left) - height(n -> right);
}

// Insert a month into the AVL tree
node *insert_node(node *root, const char *month) {
	if(!root)
		return create_node(month);

	if(strcmp(month, root -> month) < 0)
		root -> left = insert_node(root -> left, month);
	else if(strcmp(month, root -> month) > 0)
		root -> right = insert_node(root -> right, month);
	else
		// duplicates not allowed
		return root;

	root -> height = 1 + max(height(root -> left), height(root -> right));

	int balance = get_balance(root);

	// Left Left
	if(balance > 1 && strcmp(month, root -> left -> month) < 0)
		return right_rotate(root);

	// Right Right
	if(balance < -1 && strcmp(month, root -> right -> month) > 0)
		return left_rotate(root);

	// Left Right
	if(balance > 1 && strcmp(month, root -> left -> month) > 0) {
		root -> left = left_rotate(root -> left);
		return right_rotate(root);
	}

	// Right Left
	if(balance < -1 && strcmp(month, root -> right -> month) < 0) {
		root -> right = right_rotate(root -> right);
		return left_rotate(root);
	}

	return root;
}

// Print tree structure with indentation
void display_tree(node *root, int space) {
	if(!root)
		return;
	space += 10;
	display_tree(root -> right, space);
	printf("\n");
	for(int i = 10; i < space; i++) {
		printf(" ");
	}
	printf("%s\n", root -> month);
	display_tree(root -> left, space);
}

// Inorder traversal (sorted)
void inorder(node *root) {
	if(!root)
		return;
	inorder(root -> left);
	printf("%s ", root -> month);
	inorder(root -> right);
}

void free_tree(node *root) {
	if(!root)
		return;
	free_tree(root -> left);
	free_tree(root -> right);
	free(root);
}

int main() {
	node *root = NULL;
	const char *months[] = {
		"December", "January", "April", "March", "July",
		"August", "October", "February", "November", "May", "June"
	};
	
	int n = sizeof(months) / sizeof(months[0]);

	for(int i = 0; i < n; i++) {
		root = insert_node(root, months[i]);
	}
	
	printf("AVL Tree (sideways view):\n");
	display_tree(root, 0);

	printf("\nInorder Traversal (Sorted):\n");
	inorder(root);
	printf("\n");

	free_tree(root);
	return 0;
}
