#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <malloc.h>
#include <string.h>

#define N 1000000        // 元素數量
#define FANOUT 16        // B-tree 分支度
#define PIVOTS (FANOUT - 1)


struct rb_node {
    uint64_t key;
    struct rb_node *left, *right;
};

struct __attribute__((aligned(64))) btree_node {
    uint64_t pivots[PIVOTS];
    void *slots[FANOUT];
    uint16_t count;
    bool is_leaf;
};


double get_time() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

bool rbt_search(struct rb_node *root, uint64_t key) {
    struct rb_node *curr = root;
    while (curr) {
        if (key == curr->key) return true;
        if (key < curr->key) curr = curr->left;
        else curr = curr->right;
    }
    return false;
}

bool btree_search(struct btree_node *node, uint64_t key) {
    while (node) {
        int i = 0;
        while (i < node->count && key >= node->pivots[i]) {
            if (key == node->pivots[i]) return true;
            i++;
        }
        if (node->is_leaf) return false;
        node = (struct btree_node *)node->slots[i];
    }
    return false;
}

struct rb_node* insert_rbt(struct rb_node* root, uint64_t key) {
    if (!root) {
        struct rb_node* node = malloc(sizeof(struct rb_node));
        node->key = key; node->left = node->right = NULL;
        return node;
    }
    if (key < root->key) root->left = insert_rbt(root->left, key);
    else root->right = insert_rbt(root->right, key);
    return root;
}

struct btree_node* create_btree_node(bool is_leaf) {
    struct btree_node* node = memalign(64, sizeof(struct btree_node));
    node->count = 0; node->is_leaf = is_leaf;
    for(int i=0; i<FANOUT; i++) node->slots[i] = NULL;
    return node;
}


int main(int argc, char **argv) {
    if (argc < 2) {
        printf("使用方法: %s [rbt|btree]\n", argv[0]);
        return 1;
    }

uint64_t *data = malloc(N * sizeof(uint64_t));
for (int i = 0; i < N; i++) data[i] = i * 10;

// 打亂順序以防止樹狀結構退化
    srand(time(NULL));
    for (int i = N - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        uint64_t temp = data[i];
        data[i] = data[j];
        data[j] = temp;
    }
    double start, end;

    if (strcmp(argv[1], "rbt") == 0) {
        // 測試紅黑樹
        struct rb_node *rbt_root = NULL;
        for (int i = 0; i < N; i++) rbt_root = insert_rbt(rbt_root, data[i]);
        
        printf("開始測試 Red-Black Tree 隨機查詢...\n");
        start = get_time();
        for (int i = 0; i < N; i++) rbt_search(rbt_root, data[i]);
        end = get_time();
        printf("搜尋耗時: %f 秒\n", end - start);

    } else if (strcmp(argv[1], "btree") == 0) {
        // 測試 B-Tree
        struct btree_node *bt_root = create_btree_node(false);
        bt_root->count = PIVOTS;
        for(int i=0; i<FANOUT; i++) {
            bt_root->slots[i] = create_btree_node(true);
            ((struct btree_node*)bt_root->slots[i])->count = PIVOTS;
            for(int j=0; j<PIVOTS; j++) 
                ((struct btree_node*)bt_root->slots[i])->pivots[j] = (i * FANOUT + j) * 10;
        }

        printf("開始測試 B-Tree (Fan-out 16) 隨機查詢...\n");
        start = get_time();
        for (int i = 0; i < N; i++) btree_search(bt_root, data[i]);
        end = get_time();
        printf("搜尋耗時: %f 秒\n", end - start);
    }

    free(data);
    return 0;
}