#!/bin/bash
echo "Threads, Alignment, Seconds, Store-Misses, IPC"
for align in 0 1; do
    for n in 1 2 4 6 8 12; do
        # 抓取執行時間、Store Misses (代表 I->M 轉換)、指令數
        res=$(perf stat -e L1-dcache-store-misses,instructions,cycles \
              ./cache_test $n $align 2>&1 | awk '/seconds user|L1-dcache-store-misses|insn per cycle/')
        echo "$n, $align, $res"
    done
done