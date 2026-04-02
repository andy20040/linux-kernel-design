#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ITER 100000000 


volatile uint64_t global_atomic = 0;

void* bench_global(void* arg) {
    for (long i = 0; i < ITER; i++) {
        __sync_fetch_and_add(&global_atomic, 1);
    }
    return NULL;
}


struct percpu_ctr {
    uint64_t val;
    char padding[56]; // 湊滿 64 bytes，確保佔據獨立的 Cache Line
} __attribute__((aligned(64)));

struct percpu_ctr counters[128]; // 假設最多 128 核

void* bench_percpu(void* arg) {
    long id = (long)arg;
    for (long i = 0; i < ITER; i++) {
        counters[id].val++;
        __asm__ volatile("" : : "g"(counters[id].val) : "memory"); //避免優化
    }
    return NULL;
}



int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("使用方法: %s <global|percpu> <線程數>\n", argv[0]);
        return 1;
    }

    char *mode = argv[1];
    int num_threads = atoi(argv[2]);
    pthread_t threads[num_threads];

    if (strcmp(mode, "global") == 0) {
        printf("開始測試: 全域原子計數器 (線程數: %d)\n", num_threads);
        for (long i = 0; i < num_threads; i++)
            pthread_create(&threads[i], NULL, bench_global, NULL);
    } else {
        printf("開始測試: Per-CPU 計數器 (線程數: %d)\n", num_threads);
        for (long i = 0; i < num_threads; i++)
            pthread_create(&threads[i], NULL, bench_percpu, (void*)i);
    }

    for (int i = 0; i < num_threads; i++)
        pthread_join(threads[i], NULL);

    return 0;
}