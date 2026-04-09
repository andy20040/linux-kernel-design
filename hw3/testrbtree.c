#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <malloc.h>

#define N 1000000        
#define FANOUT 16       
#define PIVOTS (FANOUT - 1)

//紅黑樹節點 
struct rb_node {
    uint64_t key;
    struct rb_node *left, *right;
};

// B-Tree 節點 
struct __attribute__((aligned(64))) btree_node {
    uint64_t pivots[PIVOTS];    // 15 個 pivots (120 bytes)
    void *slots[FANOUT];        // 16 個 slots (128 bytes)
    uint16_t count;
    bool is_leaf;
}; // 總大小約 256 bytes (4 條 64B Cache lines)


double get_time() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

// RBT 搜尋邏輯 (指標追蹤) 
bool rbt_search(struct rb_node *root, uint64_t key) {
    struct rb_node *curr = root;
    while (curr) {
        if (key == curr->key) return true;
        if (key < curr->key) curr = curr->left;
        else curr = curr->right;
    }
    return false;
}

//B-Tree 搜尋邏輯 節點內線性掃描 + 跨節點跳躍
bool btree_search(struct btree_node *node, uint64_t key) {
    while (node) {
        int i = 0;
        // 在一個 Cache Line 內的連續空間進行掃描
        while (i < node->count && key >= node->pivots[i]) {
            if (key == node->pivots[i]) return true;
            i++;
        }
        if (node->is_leaf) return false;
        node = (struct btree_node *)node->slots[i];
    }
    return false;
}

// 樹狀結構建立 
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

// 簡易 B-Tree 建立 
struct btree_node* create_btree_node(bool is_leaf) {
    struct btree_node* node = memalign(64, sizeof(struct btree_node));
    node->count = 0;
    node->is_leaf = is_leaf;
    for(int i=0; i<FANOUT; i++) node->slots[i] = NULL;
    return node;
}


int main(int argc, char **argv) {
    uint64_t *data = malloc(N * sizeof(uint64_t));
    for (int i = 0; i < N; i++) data[i] = rand() % (N * 10);

    printf("實驗規模: %d 元素\n", N);

    // 建立紅黑樹
    struct rb_node *rbt_root = NULL;
    for (int i = 0; i < N; i++) rbt_root = insert_rbt(rbt_root, data[i]);

    //  建立 B-Tree (簡化版：僅填充以達成樹高 ~5)
    // 僅模擬搜尋路徑的 Cache 效應
    struct btree_node *bt_root = create_btree_node(false);
    bt_root->count = PIVOTS;
    for(int i=0; i<FANOUT; i++) {
        bt_root->slots[i] = create_btree_node(true);
        ((struct btree_node*)bt_root->slots[i])->count = PIVOTS;
    }


    double start, end;

    // RBT 測試
    start = get_time();
    for (int i = 0; i < N; i++) rbt_search(rbt_root, data[i]);
    end = get_time();
    printf("Red-Black Tree (Pointer Chasing) 耗時: %f 秒\n", end - start);

    // B-Tree 測試
    start = get_time();
    for (int i = 0; i < N; i++) btree_search(bt_root, data[i]);
    end = get_time();
    printf("B-Tree (Cache-aligned, Fan-out 16) 耗時: %f 秒\n", end - start);

    free(data);
    return 0;
}