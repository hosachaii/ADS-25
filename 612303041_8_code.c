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

typedef struct min_heap {
	int size;
	node **heap;
} min_heap;

node *create_node(char c, int freq) {
	node *nn = (node *)malloc(sizeof(node));
	nn -> c = c;
	nn -> freq = freq;
	nn -> lchild = NULL;
	nn -> rchild = NULL;
	return nn;
}

void create_heap(min_heap *h) {
	h -> heap = (node **)calloc(ASCII, sizeof(node *));
	h -> size = 0;
}

// Helper function to swap nodes in heap.
void swap(node **p, node **q) {
	node *r = *p;
	*p = *q;
	*q = r;
}

// Standard function to insert a node in min_heap.
void insert(min_heap *h, node *n) {
	int i = h -> size++;
	h -> heap[i] = n;
	while(i && h -> heap[i] -> freq < h -> heap[(i - 1)/2] -> freq) {
		swap(&h -> heap[i], &h -> heap[(i - 1)/2]);
		i = (i - 1)/2;
	}
}

// Restore heap property, if and where violated.
void heapify(min_heap* h, int i) {
	int smallest = i;
	int l = 2*i + 1, r = 2*i + 2;
	
	if (l < h -> size && (h -> heap[l] -> freq < h -> heap[smallest] -> freq))
		smallest = l;
	if (r < h -> size && (h -> heap[r] -> freq < h -> heap[smallest] -> freq))
		smallest = r;
	if (smallest != i) {
		swap(&h -> heap[i], &h -> heap[smallest]);
		heapify(h, smallest);
	}
}

node *extract_min(min_heap *h) {
	node *minode = h -> heap[0];
	h -> heap[0] = h -> heap[h -> size - 1];
	h -> size--;
	heapify(h, 0);
	return minode;
}

node *build_huffman_tree(min_heap *h, int *freq) {
	for(int i = 0; i < ASCII; i++) {
		if(freq[i] > 0)
			insert(h, create_node((char)i, freq[i]));
	}
	
	while(h -> size > 1) {
		node *left = extract_min(h);
		node *right = extract_min(h);
		
		node *top = create_node('$', left -> freq + right -> freq);
		top -> lchild = left;
		top -> rchild = right;
		insert(h, top);
	}
	return extract_min(h);
}

char *compress(char *text, char **codes) {
	int total_len = 0;
	for(int i = 0; text[i]; i++) {
		if (codes[text[i]])
			total_len += strlen(codes[text[i]]);
	}
	char *compressed_text = (char *)malloc(total_len + 1);
	compressed_text[0] = '\0';
	for(int i = 0; text[i]; i++) {
		if (codes[text[i]])
			strcat(compressed_text, codes[text[i]]);
	}
	return compressed_text;
}

char *decompress(node *root, char *bin_text) {
	char text[TEXT_SIZE];
	int tlen = 0;
	long blen = 0;
	node *curr = root;
	
	while(bin_text[blen]) {
		if (bin_text[blen] == '0')
			curr = curr -> lchild;
		else
			curr = curr -> rchild;
		
		if(!curr -> lchild && !curr -> rchild) {
			text[tlen++] = curr -> c;
			curr = root;
		}
		blen++;
	}
	text[tlen] = '\0';
	double compression_ratio = ((double)(tlen * 8) / blen);
	printf("compression ratio: %lf\n", compression_ratio);
	return strdup(text);
}

void get_codes(node *root, char **codes, char *path, int depth) {
	if (!root)
		return;
	if (!root -> lchild && !root -> rchild) {
		path[depth] = '\0';
		codes[root -> c] = strdup(path);
		printf("%c: %s\n", root -> c, codes[root -> c]);
		return;
	}
	path[depth] = '0';
	get_codes(root -> lchild, codes, path, depth + 1);
	path[depth] = '1';
	get_codes(root -> rchild, codes, path, depth + 1);
}

int main(void) {
    char *text = (char *)calloc(TEXT_SIZE, sizeof(char));
	int len = 0;
	char buf;
	
	printf("Enter text (press Ctrl+D to stop): ");
    while((buf = getchar()) != EOF) {
    	text[len++] = buf;
    }
    text[len] = '\0';

    int freq[ASCII] = {0};
    for(int i = 0; text[i]; i++) {
        freq[text[i]]++;
	}

    min_heap h;
    create_heap(&h);
    node *root = build_huffman_tree(&h, freq);

    char **codes = (char **)calloc(ASCII, sizeof(char *));

    char path[ASCII];
    printf("Huffman codes:\n");
    get_codes(root, codes, path, 0);

    char *compressed = compress(text, codes);
    char *decompressed = decompress(root, compressed);

    printf("\nOriginal text: %s\n", text);
    printf("\nCompressed: %s\n", compressed);
    printf("\nDecompressed: %s\n", decompressed);
    
    free(compressed);
    free(decompressed);
    return 0;
}
