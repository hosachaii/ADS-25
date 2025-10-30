#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// 26 alphabets + 1 special character
#define ALPHABET_SIZE 27

// Define maximum word length for buffer arrays
#define MAX_WORD_LEN 50

// Trie node structure
typedef struct trie_node {
    // Each node has pointers to possible child nodes for each character
    struct trie_node *children[ALPHABET_SIZE];
} trie_node;

// Creates and initializes a new Trie node
trie_node *create_node() {
    trie_node *node = (trie_node *)malloc(sizeof(trie_node));

    // Initialize all child pointers to NULL
    for(int i = 0; i < ALPHABET_SIZE; i++) {
        node -> children[i] = NULL;
    }

    return node;
}

// Inserts a given word into the Trie
void insert(trie_node *root, char *word) {

    // Start traversal from root node
    trie_node *curr = root;
    int len = strlen(word);

    // Process each character in the word
    for(int i = 0; i < len; i++) {
        char c = tolower(word[i]);

        // Ignore non-alphabetic characters
        if(c < 'a' || c > 'z')
            continue;

        // Calculate index (0 for 'a', 25 for 'z')
        int index = c - 'a';

        // Create new node if path doesn’t exist
        if(!curr -> children[index])
            curr -> children[index] = create_node();

        // Move to next node
        curr = curr -> children[index];
    }

    // Mark the end of the word by using the 27th (index 26) pointer
    if(!curr -> children[26])
        curr -> children[26] = create_node();
}

// Recursively prints all word completions for a given prefix
void print_suggestions(trie_node *root, char *prefix, char *current, int depth) {

    // If the end-of-word marker exists, print the full word suggestion
    if(root -> children[26]) {
        current[depth] = '\0';
        printf("%s%s\n", prefix, current);
    }

    // Explore all possible next letters (a–z)
    for(int i = 0; i < 26; i++) {
        if(root->children[i]) {
            // Add the next character to the current suffix
            current[depth] = 'a' + i;

            // Recurse deeper into the Trie
            print_suggestions(root -> children[i], prefix, current, depth + 1);
        }
    }
}

// Searches for a prefix in the Trie and prints all suggestions
void auto_suggest(trie_node *root, char *prefix) {

    // Start traversal from root
    trie_node *curr = root;
    int len = strlen(prefix);

    // Buffer to hold the suffix being formed
    char current[MAX_WORD_LEN];

    // Traverse the Trie along the prefix path
    for(int i = 0; i < len; i++) {
        char c = tolower(prefix[i]);  // Normalize to lowercase

        // Invalid prefix if it contains non-letters
        if(c < 'a' || c > 'z') {
            printf("No suggestions found.\n");
            return;
        }

        int index = c - 'a';

        // Prefix not found if child does not exist
        if(!curr -> children[index]) {
            printf("No suggestions found.\n");
            return;
        }

        // Move deeper in the Trie
        curr = curr -> children[index];
    }

    // If prefix exists, recursively print all word completions
    print_suggestions(curr, prefix, current, 0);
}

int main(void) {

    // Dictionary of words to load into the Trie
    char *dictionary[] = {
        "apple", "application", "apply", "appreciate", "approach", "appropriate",
        "banana", "band", "bank", "bar", "base", "basic", "basis", "basket",
        "ball", "balance", "battle", "beach", "bear", "beat", "beautiful",
        "beauty", "because", "become", "before", "begin", "behavior", "behind",
        "believe", "benefit", "best", "better", "between", "beyond", "bird",
        "birth", "bit", "black", "blood", "blue", "board", "boat", "body",
        "book", "born", "both", "bottle", "bottom", "box", "boy", "brain",
        "branch", "bread", "break", "breakfast", "bridge", "bright", "bring",
        "brother", "brown", "build", "building", "burn", "business", "busy",
        "button", "buy", "cake", "call", "camera", "camp", "can", "cancel",
        "cancer", "candidate", "capacity", "capital", "captain", "capture",
        "car", "card", "care", "career", "careful", "carry", "case", "cash",
        "cat", "catch", "category", "cause", "celebrate", "cell", "center",
        "central", "century", "certain", "chair", "challenge", "chance",
        "change", "character", "charge", "chart", "cheap", "check", "cheese",
        "chemical", "chest", "chicken", "child", "chocolate", "choice",
        "choose", "church", "circle", "citizen", "city", "civil", "claim",
        "class", "classic", "classroom", "clean", "clear", "climb", "close",
    };

    // Buffer to store user-input prefix
    char prefix[MAX_WORD_LEN];

    // Calculate number of words in the dictionary
    int dict_size = sizeof(dictionary) / sizeof(dictionary[0]);

    // Create root node of Trie
    trie_node *root = create_node();

    // Insert all dictionary words into the Trie
    for(int i = 0; i < dict_size; i++) {
        insert(root, dictionary[i]);
    }

    // Ask user for a prefix
    printf("Enter a word prefix: ");
    fgets(prefix, MAX_WORD_LEN, stdin);

    // Remove trailing newline character from fgets
    prefix[strcspn(prefix, "\n")] = '\0';

    // Display suggestions
    printf("\nSuggestions:\n");
    auto_suggest(root, prefix);

    return 0;
}

