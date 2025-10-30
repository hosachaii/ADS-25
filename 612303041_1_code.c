#include <stdio.h>
#include <stdlib.h>

// BST node structure
typedef struct node {
	int data;              
	struct node *left;
	struct node *right;
} node;

// Creates a new BST node with the given value
node *create_node(int data) {
	node *n = (node *) malloc(sizeof(node));
	n -> data = data;
	n -> left = n -> right = NULL;
	return n;
}

// Insert into the BST recursively
node *insert(node *root, int data) {
	if(!root)
		return create_node(data);

	// Recursively insert into the left or right subtree
	if(data < root -> data)
		root -> left = insert(root -> left, data);
	else if(data > root -> data)
		root -> right = insert(root -> right, data);

	return root;
}

// Finds the node with the maximum value in the BST
node *find_max(node *root) {
	if(!root)
		return NULL;

	// Maximum element lies in the rightmost node
	while(root -> right)
		root = root -> right;

	return root;
}

// Iterative preorder traversal (Root, Left, Right)
void preorder(node *root) {
	if(!root)
		return;

	node *stack[100];
	int top = -1;

	stack[++top] = root;

	while(top >= 0) {
		node *curr = stack[top--];
		printf("%d ", curr -> data); // Visit the node

		// Push right child first so left child is processed first (LIFO order)
		if(curr -> right)
			stack[++top] = curr -> right;
		if(curr -> left)
			stack[++top] = curr -> left;
	}
	printf("\n");
}

// Iterative inorder traversal (Left, Root, Right)
void inorder(node *root) {
	node *stack[100];
	int top = -1;
	node *curr = root;

	while(curr || top >= 0) {
		// Reach the leftmost node of the current subtree
		while(curr) {
			stack[++top] = curr;
			curr = curr -> left;
		}

		// Pop the minimum value from the stack.
		curr = stack[top--];
		printf("%d ", curr -> data);

		// Traverse the right subtree
		curr = curr -> right;
	}
	printf("\n");
}

// Iterative postorder traversal (Left, Right, Root)
void postorder(node *root) {
	node *stack1[100], *stack2[100];
	int top1 = -1, top2 = -1;

	if(!root)
		return;

	// stack1 is used for processing
	// stack2 stores the reverse order
	stack1[++top1] = root;

	while(top1 >= 0) {
		node *curr = stack1[top1--];
		stack2[++top2] = curr;

		// Push left and right children to stack1 for later processing
		if(curr -> left)
			stack1[++top1] = curr -> left;
		if(curr -> right)
			stack1[++top1] = curr -> right;
	}

	// Pop all nodes from stack2 to print postorder
	while(top2 >= 0)
		printf("%d ", stack2[top2--] -> data);

	printf("\n");
}

// Prints the BST in a rotated tree structure (right-subtree on top)
void print_tree(node *root, int space) {
	if(!root)
		return;
	 // Increase spacing for each level of depth
	space += 5;

	// Print right subtree first.
	print_tree(root -> right, space);

	// Print current node with indentation
	printf("\n");
	for(int i = 5; i < space; i++)
		printf(" ");
	printf("%d\n", root -> data);

	// Then print left subtree
	print_tree(root -> left, space);
}

int main() {
	node *root = NULL;

	// Example input values to construct the BST
	int values[] = {50, 30, 70, 20, 40, 60, 80};

	// Insert all values into the BST
	for(int i = 0; i < 7; i++)
		root = insert(root, values[i]);

	printf("Tree Structure:\n");
	print_tree(root, 0);

	printf("\nPreorder: ");
	preorder(root);

	printf("Inorder: ");
	inorder(root);

	printf("Postorder: ");
	postorder(root);

	// Find and print maximum value in the BST
	node *max = find_max(root);
	if(max)
		printf("\nMaximum Value: %d\n", max -> data);

	return 0;
}
