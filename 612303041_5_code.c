#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_LEVEL 5
// probability for random level generation
#define P 0.5

typedef struct node {
    int key;
    struct node *forward[MAX_LEVEL];
} node;

typedef struct skip_list {
    int level;
    node *header;
} skip_list;

// create node
node *create_node(int key, int level) {
    node *n = (node *) malloc(sizeof(node));
    n -> key = key;
    for (int i = 0; i < MAX_LEVEL; i++)
        n -> forward[i] = NULL;
    return n;
}

// random level generator
int random_level() {
    int lvl = 0;
    while ((rand() < P * RAND_MAX) && lvl < MAX_LEVEL - 1)
        lvl++;
    return lvl;
}

// create empty skip list
skip_list *create_skip_list() {
    skip_list *list = (skip_list *) malloc(sizeof(skip_list));
    list -> level = 0;
    // assign header with dummy key
    list -> header = create_node(-1, MAX_LEVEL);
    return list;
}

// insert key
void insert(skip_list *list, int key) {
    node *update[MAX_LEVEL];
    node *p = list -> header;

    // move down levels to find position
    for (int i = list -> level; i >= 0; i--) {
        while (p -> forward[i] && p -> forward[i] -> key < key)
            p = p -> forward[i];
        update[i] = p;
    }

    p = p -> forward[0];

    if (!p || p -> key != key) {
        int lvl = random_level();
        if (lvl > list -> level) {
            for (int i = list -> level + 1; i <= lvl; i++)
                update[i] = list -> header;
            list -> level = lvl;
        }

        node *new_node = create_node(key, lvl + 1);
        for (int i = 0; i <= lvl; i++) {
            new_node -> forward[i] = update[i] -> forward[i];
            update[i] -> forward[i] = new_node;
        }
    }
}

// search key
node *search(skip_list *list, int key) {
    node *p = list -> header;
    for (int i = list -> level; i >= 0; i--) {
        while (p -> forward[i] && p -> forward[i] -> key < key)
            p = p -> forward[i];
    }
    p = p -> forward[0];
    if (p && p -> key == key)
        return p;
    return NULL;
}

// delete key
void delete(skip_list *list, int key) {
    node *update[MAX_LEVEL];
    node *p = list -> header;

    for (int i = list -> level; i >= 0; i--) {
        while (p -> forward[i] && p -> forward[i] -> key < key)
            p = p -> forward[i];
        update[i] = p;
    }

    p = p -> forward[0];
    if (p && p -> key == key) {
        for (int i = 0; i <= list -> level; i++) {
            if (update[i] -> forward[i] != p)
                break;
            update[i] -> forward[i] = p -> forward[i];
        }
        free(p);

        while (list -> level > 0 && list -> header -> forward[list -> level] == NULL)
            list -> level--;
    }
}

// display list
void display(skip_list *list) {
    printf("\nSkip List:\n");
    for (int i = list -> level; i >= 0; i--) {
        node *p = list -> header -> forward[i];
        printf("Level %d: ", i);
        while (p) {
            printf("%d ", p -> key);
            p = p -> forward[i];
        }
        printf("\n");
    }
}

int main() {
    srand(time(0));

    skip_list *list = create_skip_list();

    insert(list, 3);
    insert(list, 6);
    insert(list, 7);
    insert(list, 9);
    insert(list, 12);
    insert(list, 19);
    insert(list, 17);
    insert(list, 26);
    insert(list, 21);
    insert(list, 25);

    display(list);

    printf("\nSearching for 19: %s\n", search(list, 19) ? "Found" : "Not Found");
    printf("Deleting 19...\n");
    delete(list, 19);
    display(list);

    return 0;
}
