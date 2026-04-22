#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sched.h>
#include <unistd.h>

#define ITERATIONS 100000000 // 1億次 iter

// 對照組：發生 False Sharing
struct node_unaligned {
    volatile unsigned long color; // 模擬 rb_node 的 color 欄位
} __attribute__((packed));

// 實驗組：消除 False Sharing (Aligned)
struct node_aligned {
    volatile unsigned long color;
} __attribute__((aligned(64)));

struct node_unaligned nodes_fs[12];
struct node_aligned nodes_ok[12];

int thread_count;
int use_alignment;

void* benchmark(void* arg) {
    int tid = *(int*)arg;
    
    // 1. 強制將執行緒綁定到特定邏輯核心
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(tid, &cpuset);
    pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);

    // 2. 進行壓力測試 (模擬 PrWr)
    for (int i = 0; i < ITERATIONS; i++) {
        if (use_alignment) {
            nodes_ok[tid].color = i; // 每個執行緒改自己的 Cache Line
        } else {
            nodes_fs[tid].color = i; // 多個執行緒改同一個 Cache Line
        }
    }
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Usage: %s <thread_count> <use_alignment 0|1>\n", argv[0]);
        return 1;
    }

    thread_count = atoi(argv[1]);
    use_alignment = atoi(argv[2]);

    pthread_t threads[12];
    int tids[12];

    for (int i = 0; i < thread_count; i++) {
        tids[i] = i;
        pthread_create(&threads[i], NULL, benchmark, &tids[i]);
    }

    for (int i = 0; i < thread_count; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}