#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct node {
	struct node *parent;
	struct node *child;
	struct node *right_sibling;
	int degree;
	int data;
}node;

typedef struct binomial_heap {
	node *root;
}binomial_heap;

void init_binomial_heap(binomial_heap *bh) {
	bh -> root = NULL;
}

node *create_node(int data) {
	node *new_node = (node *)malloc(sizeof(node));
	new_node -> parent = NULL;
	new_node -> child = NULL;
	new_node -> right_sibling = NULL;
	new_node -> degree = 0;
	new_node -> data = data;
	return new_node;
}

// Merge two binomial heaps into one (maintaining degree order)
node *merge(node *h1, node *h2) {
	if(!h1 || !h2)
		return !h1 ? h2 : h1;
	
	node *head = NULL, *tail = NULL;
	if(h1 -> degree <= h2 -> degree) {
		head = h1;
		h1 = h1 -> right_sibling;
	}
	else {
		head = h2;
		h2 = h2 -> right_sibling;
	}
	tail = head;
	while(h1 && h2) {
		if(h1 -> degree <= h2 -> degree) {
			tail -> right_sibling = h1;
			h1 = h1 -> right_sibling;
		} else {
			tail -> right_sibling = h2;
			h2 = h2 -> right_sibling;
		}
		tail = tail -> right_sibling;
	}
	tail -> right_sibling = h1 ? h1 : h2;
	
	node *prev = NULL, *curr = head, *next = head -> right_sibling;
	while(next) {
		if(curr -> degree != next -> degree ||
		   (next -> right_sibling && next -> right_sibling -> degree == curr -> degree)) {
			prev = curr;
			curr = next;
		}
		else {
			if(curr -> data <= next -> data) {
				curr -> right_sibling = next -> right_sibling;
				next -> right_sibling = curr -> child;
				next -> parent = curr;
				curr -> child = next;
				curr -> degree++;
			} else {
				if(prev)
					prev -> right_sibling = next;
				else
					head = next;
				curr -> right_sibling = next -> child;
				next -> child = curr;
				curr -> parent = next;
				next -> degree++;
				curr = next;
			}
		}
		next = curr -> right_sibling;
	}
	return head;
}

// Find minimum value in heap (O(log n))
int find_minimum(binomial_heap *bh) {
	node *curr = bh -> root;
	int min = curr -> data;
	while(curr) {
		if(curr -> data < min)
			min = curr -> data;
		curr = curr -> right_sibling;
	}
	return min;
}

// Reverse child list for merging after extraction
node *reverse_ll(node *head) {
	if(!head || !head -> right_sibling)
		return head;
	node *new_head = reverse_ll(head -> right_sibling);
	head -> right_sibling -> right_sibling = head;
	head -> right_sibling = NULL;
	head -> parent = NULL;
	return new_head;
}

// Extract and return minimum value (O(log n))
int extract_minimum(binomial_heap *bh) {
	node *curr = bh -> root;
	node *prev = NULL, *prev_min = NULL;
	node *min_node = bh -> root;
	
	while(curr) {
		if(curr -> data < min_node -> data) {
			min_node = curr;
			prev_min = prev;
		}
		prev = curr;
		curr = curr -> right_sibling;
	}
	if(prev_min)
		prev_min -> right_sibling = min_node -> right_sibling;
	else
		bh -> root = min_node -> right_sibling;
	
	node *new_heap = reverse_ll(min_node -> child);
	bh -> root = merge(bh -> root, new_heap);
	int min_val = min_node -> data;
	free(min_node);
	return min_val;
}

// Insert new key (just merge a singleton tree)
void insert(binomial_heap *bh, int data) {
	bh -> root = merge(bh -> root, create_node(data));
}

// Swap data of two nodes
node *swap(node *a, node *b) {
	int temp = a -> data;
	a -> data = b -> data;
	b -> data = temp;
	return b;
}

// Decrease key of a node and maintain heap property
void decrease_key(binomial_heap *bh, node *h, int key) {
	if(!h || key >= h -> data)
		return;
	h -> data = key;
	while(h -> parent && h -> data < h -> parent -> data)
		h = swap(h, h -> parent);
}

// Recursive print (for debugging)
void print_heap(node *h) {
	if(!h)
		return;
	printf("%d ", h -> data);
	print_heap(h -> child);
	print_heap(h -> right_sibling);
}

int main(void) {
	binomial_heap h1, h2;
	init_binomial_heap(&h1);
	init_binomial_heap(&h2);

	// Insert keys
	insert(&h1, 24);
	insert(&h1, 16);
	insert(&h1, 4);
	insert(&h1, 89);
	insert(&h2, 69);
	insert(&h2, 67);
	insert(&h2, 94);

	printf("Min of h1: %d\n", find_minimum(&h1));
	printf("Min of h2: %d\n", find_minimum(&h2));

	// Unite two heaps
	binomial_heap united;
	united.root = merge(h1.root, h2.root);
	printf("After unite, min: %d\n", find_minimum(&united));

	// Extract minimum
	printf("Extracted min: %d\n", extract_minimum(&united));
	printf("New min: %d\n", find_minimum(&united));

	// Decrease key
	// pick a random node
	node *p = united.root -> right_sibling;
	decrease_key(&united, p, 1);
	printf("Min after decrease: %d\n", find_minimum(&united));

	return 0;
}

