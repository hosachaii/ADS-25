#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ASCII 128
#define TEXT_SIZE 1000

typedef struct node {
	char c;
	int freq;
	struct node *lchild;
	struct node *rchild;
} node;

// Represents a min-heap (used for building the Huffman tree)
typedef struct min_heap {
	int size;
	node **heap;
} min_heap;


// Allocates and initializes a new node
node *create_node(char c, int freq) {
	node *nn = (node *)malloc(sizeof(node));
	nn -> c = c;
	nn -> freq = freq;
	nn -> lchild = NULL;
	nn -> rchild = NULL;
	return nn;
}

// Initializes an empty min heap
void create_heap(min_heap *h) {
	h -> heap = (node **)calloc(ASCII, sizeof(node *));
	h -> size = 0;
}

// Swaps two node pointers (used in heap operations)
void swap(node **p, node **q) {
	node *r = *p;
	*p = *q;
	*q = r;
}

// Insert node and restore minheap property
void insert(min_heap *h, node *n) {
	int i = h -> size++;
	h -> heap[i] = n;

	// Swap until parent has smaller frequency
	while(i && h -> heap[i] -> freq < h -> heap[(i - 1)/2] -> freq) {
		swap(&h -> heap[i], &h -> heap[(i - 1)/2]);
		i = (i - 1)/2;
	}
}

// Maintains the min-heap property starting from index i
void heapify(min_heap* h, int i) {
	int smallest = i;
	int l = 2*i + 1, r = 2*i + 2;

	// Check if left or right child is smaller
	if (l < h -> size && (h -> heap[l] -> freq < h -> heap[smallest] -> freq))
		smallest = l;
	if (r < h -> size && (h -> heap[r] -> freq < h -> heap[smallest] -> freq))
		smallest = r;

	// If violated, swap and recurse
	if (smallest != i) {
		swap(&h -> heap[i], &h -> heap[smallest]);
		heapify(h, smallest);
	}
}

// Removes and returns the smallest node (min frequency) from heap
node *extract_min(min_heap *h) {
	node *minode = h -> heap[0];
	h -> heap[0] = h -> heap[h -> size - 1];
	h -> size--;
	heapify(h, 0);
	return minode;
}

// Builds the Huffman tree from frequency table
node *build_huffman_tree(min_heap *h, int *freq) {
	// Insert all non-zero frequency characters into the heap
	for(int i = 0; i < ASCII; i++) {
		if(freq[i] > 0)
			insert(h, create_node((char)i, freq[i]));
	}

	// Combine two smallest nodes repeatedly until one root remains
	while(h -> size > 1) {
		node *left = extract_min(h);
		node *right = extract_min(h);

		// Create an internal node with frequency = left + right
		node *top = create_node('$', left -> freq + right -> freq);
		top -> lchild = left;
		top -> rchild = right;

		insert(h, top);
	}

	// Return the root node (last remaining element)
	return extract_min(h);
}

// Compresses text using the generated Huffman codes
char *compress(char *text, char **codes) {
	int total_len = 0;

	// Calculate total length of compressed bit string
	for(int i = 0; text[i]; i++) {
		if (codes[text[i]])
			total_len += strlen(codes[text[i]]);
	}

	// Allocate output string
	char *compressed_text = (char *)malloc(total_len + 1);
	compressed_text[0] = '\0';

	// Append each character’s binary code
	for(int i = 0; text[i]; i++) {
		if (codes[text[i]])
			strcat(compressed_text, codes[text[i]]);
	}

	return compressed_text;
}

// Decompresses a binary string back into text using the Huffman tree
char *decompress(node *root, char *bin_text) {
	char text[TEXT_SIZE];
	int tlen = 0;
	long blen = 0;
	node *curr = root;

	while(bin_text[blen]) {
		// Traverse left on '0' and right on '1'
		if (bin_text[blen] == '0')
			curr = curr -> lchild;
		else
			curr = curr -> rchild;

		// Leaf node reached: output its character
		if(!curr -> lchild && !curr -> rchild) {
			text[tlen++] = curr -> c;
			// Start again from root
			curr = root;
		}
		blen++;
	}

	text[tlen] = '\0';

	// Print compression ratio (original bits vs encoded bits)
	double compression_ratio = ((double)(tlen * 8) / blen);
	printf("compression ratio: %lf\n", compression_ratio);

	return strdup(text);
}

// Recursively traverses the Huffman tree to assign binary codes
void get_codes(node *root, char **codes, char *path, int depth) {
	if (!root)
		return;

	// Reached leaf node, store the current path as its code
	if (!root -> lchild && !root -> rchild) {
		path[depth] = '\0';
		codes[root -> c] = strdup(path);
		printf("%c: %s\n", root -> c, codes[root -> c]);
		return;
	}

	// Traverse left with '0' and right with '1'
	path[depth] = '0';
	get_codes(root -> lchild, codes, path, depth + 1);
	path[depth] = '1';
	get_codes(root -> rchild, codes, path, depth + 1);
}


int main(void) {
    char *text = (char *)calloc(TEXT_SIZE, sizeof(char));
	int len = 0;
	char buf;

	// Read text input (until EOF)
	printf("Enter text (press Ctrl+D to stop): ");
    while((buf = getchar()) != EOF) {
    	text[len++] = buf;
    }
    text[len] = '\0';

    // Count frequency of each character
    int freq[ASCII] = {0};
    for(int i = 0; text[i]; i++) {
        freq[text[i]]++;
	}

    // Build Huffman tree
    min_heap h;
    create_heap(&h);
    node *root = build_huffman_tree(&h, freq);

    // Generate codes
    char **codes = (char **)calloc(ASCII, sizeof(char *));
    char path[ASCII];
    printf("Huffman codes:\n");
    get_codes(root, codes, path, 0);

    // Compress and decompress
    char *compressed = compress(text, codes);
    char *decompressed = decompress(root, compressed);

    // Display results
    printf("\nOriginal text: %s\n", text);
    printf("\nCompressed: %s\n", compressed);
    printf("\nDecompressed: %s\n", decompressed);

    free(compressed);
    free(decompressed);
    return 0;
}

