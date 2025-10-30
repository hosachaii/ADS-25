#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

#define MAX_DEG 50   // Max degree of a node in heap

// Node of Fibonacci Heap
typedef struct node {
	struct node *parent;
	struct node *child;
	struct node *left;
	struct node *right;
	int degree;
	bool mark;
	int data;
} node;

// Fibonacci Heap structure
typedef struct fheap {
	node *minode;  // Pointer to node with minimum key
	int nodes;     // Total number of nodes
} fheap;

// Create a new node with given key
node *create_node(int data) {
	node *n = (node *)malloc(sizeof(node));
	n -> parent = NULL;
	n -> child = NULL;
	n -> left = n;
	n -> right = n;
	n -> degree = 0;
	n -> mark = false;
	n -> data = data;
	return n;
}

// Initialize a new empty heap
void init_heap(fheap *fh) {
	fh -> minode = NULL;
	fh -> nodes = 0;
}

// Merge two root lists and return the resulting heap
fheap *merge(fheap *h1, fheap *h2) {
	if(!h1 || !h2)
		return !h1 ? h2 : h1;
	if(!h1 -> minode || !h2 -> minode)
		return !h1 -> minode ? h2 : h1;

	node *min1 = h1 -> minode;
	node *min2 = h2 -> minode;

	// Join root lists
	node *r1 = min1 -> right;
	node *l2 = min2 -> left;
	min1 -> right = min2;
	min2 -> left = min1;
	r1 -> left = l2;
	l2 -> right = r1;

	// Update minimum pointer
	h1 -> minode = (min1 -> data < min2 -> data) ? min1 : min2;
	h1 -> nodes += h2 -> nodes;
	return h1;
}

// Insert a new key into the heap
void insert(fheap *fh, int data) {
	node *n = create_node(data);

	if(!fh -> minode)
		fh -> minode = n;
	else {
		// Add to root list
		node *min = fh -> minode;
		n -> left = min;
		n -> right = min -> right;
		min -> right -> left = n;
		min -> right = n;

		if(n -> data < fh -> minode -> data)
			fh -> minode = n;
	}
	fh -> nodes++;
}

// Return the minimum key from the heap
int find_min(fheap *fh) {
	return fh -> minode ? fh -> minode -> data : INT_MIN;
}

// Combine trees of same degree to maintain Fibonacci Heap properties
void consolidate(fheap *fh) {
	node *arr[MAX_DEG] = {NULL};
	node *start = fh -> minode;
	node *p = start;

	do {
		node *x = p;
		int d = x -> degree;

		// Merge trees of equal degree
		while(arr[d]) {
			node *y = arr[d];
			if(x -> data > y -> data) {
				node *t = x;
				x = y;
				y = t;
			}

			// Remove y from root list
			y -> left -> right = y -> right;
			y -> right -> left = y -> left;

			// Make y child of x
			y -> parent = x;
			if(!x -> child)
				x -> child = y -> left = y -> right = y;
			else {
				y -> left = x -> child;
				y -> right = x -> child -> right;
				x -> child -> right -> left = y;
				x -> child -> right = y;
			}

			x -> degree++;
			y -> mark = false;
			arr[d] = NULL;
			d++;
		}

		arr[d] = x;
		p = p -> right;
	} while(p != start);

	// Rebuild root list and find new min
	fh -> minode = NULL;
	for(int i = 0; i < MAX_DEG; i++) {
		if(arr[i]) {
			if(!fh -> minode) {
				fh -> minode = arr[i];
				arr[i] -> left = arr[i];
				arr[i] -> right = arr[i];
			} else {
				node *min = fh -> minode;
				arr[i] -> left = min;
				arr[i] -> right = min -> right;
				min -> right -> left = arr[i];
				min -> right = arr[i];
				if(arr[i] -> data < fh -> minode -> data)
					fh -> minode = arr[i];
			}
		}
	}
}

// Remove and return the minimum key from the heap
int extract_minimum(fheap *fh) {
	if(!fh || !fh -> minode)
		return INT_MIN;

	node *z = fh -> minode;

	// Move z's children to root list
	if(z -> child) {
		node *c = z -> child;
		do {
			c -> parent = NULL;
			c = c -> right;
		} while(c != z -> child);

		node *zl = z -> left;
		node *cl = z -> child -> left;
		zl -> right = z -> child;
		z -> child -> left = zl;
		cl -> right = z -> right;
		z -> right -> left = cl;
	}

	// Remove z from root list
	z -> left -> right = z -> right;
	z -> right -> left = z -> left;

	int min_val = z -> data;

	// If last node, heap becomes empty
	if(z == z -> right)
		fh -> minode = NULL;
	else {
		fh -> minode = z -> right;
		consolidate(fh);
	}

	free(z);
	fh -> nodes--;
	return min_val;
}

// Cut node p from its parent q and move to root list
void cut(fheap *fh, node *p, node *q) {
	if(p -> right == p)
		q -> child = NULL;
	else {
		p -> left -> right = p -> right;
		p -> right -> left = p -> left;
		if(q -> child == p)
			q -> child = p -> right;
	}
	q -> degree--;

	// Add p to root list
	node *min = fh -> minode;
	p -> left = min;
	p -> right = min -> right;
	min -> right -> left = p;
	min -> right = p;
	p -> parent = NULL;
	p -> mark = false;
}

// Perform cascading cuts upwards
void cascading_cut(fheap *fh, node *q) {
	node *r = q -> parent;
	if(r) {
		if(!q -> mark)
			q -> mark = true;
		else {
			cut(fh, q, r);
			cascading_cut(fh, r);
		}
	}
}

// Decrease key of node p to a smaller value
void decrease_key(fheap *fh, node *p, int key) {
	if(!p || key > p -> data)
		return;

	p -> data = key;
	node *q = p -> parent;

	// If heap property violated, cut and cascade
	if(q && p -> data < q -> data) {
		cut(fh, p, q);
		cascading_cut(fh, q);
	}

	// Update min if necessary
	if(p -> data < fh -> minode -> data)
		fh -> minode = p;
}

int main() {
	fheap *h1 = (fheap *)malloc(sizeof(fheap));
	fheap *h2 = (fheap *)malloc(sizeof(fheap));
	init_heap(h1);
	init_heap(h2);

	// Insert keys into both heaps
	insert(h1, 10);
	insert(h1, 4);
	insert(h1, 7);
	insert(h2, 15);
	insert(h2, 3);

	printf("Min of h1: %d\n", find_min(h1));
	printf("Min of h2: %d\n", find_min(h2));

	// Unite both heaps
	fheap *merged = merge(h1, h2);
	printf("After unite, min: %d\n", find_min(merged));

	// Extract the minimum key
	int min_val = extract_minimum(merged);
	printf("Extracted min: %d\n", min_val);
	printf("New min: %d\n", find_min(merged));

	// Decrease key of an arbitrary node
	node *p = merged -> minode -> right;
	decrease_key(merged, p, 1);
	printf("Min after decrease: %d\n", find_min(merged));

	free(merged);
	return 0;
}

