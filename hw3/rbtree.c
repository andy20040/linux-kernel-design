#include <stdio.h>
#include <stdint.h>
#include <string.h>

struct __attribute__((packed)) rb_node {
    uintptr_t parent_color;
};

int main() {
    // 建立緩衝區
    char buffer[64];
    
    // 強制取一個奇數位址
    struct rb_node *node_ptr = (struct rb_node *)&buffer[1];

    uintptr_t real_address = (uintptr_t)node_ptr;
    printf("Real Address (Unaligned): %p\n", (void*)real_address);

    // 標記顏色 (RED = 0)
    node_ptr->parent_color = real_address | 0; 

    // 模擬還原位址 (& ~3UL)
    uintptr_t recovered_address = node_ptr->parent_color & ~3UL;
    printf("Recovered Address:        %p\n", (void*)recovered_address);

    if (real_address != recovered_address) {
        printf("\n[FAILURE] 原始位址是 %lx，還原後變成 %lx\n", 
               real_address, recovered_address);
    }

    return 0;
}