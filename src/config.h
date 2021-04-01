#ifndef CONFIG_H
#define CONFIG_H
#include <cmath>

enum ReplacePolicy {
    LRU,
    BT,
    Direct,
    Score,
};

enum WritePolicy0 {
    WriteThrough,
    WriteBack,
};

enum WritePolicy1{
    WriteAlloc,
    WriteNonAlloc,
};

const int CACHE_SIZE = 0x20000;
const int ADDRESS_WIDTH = 64;

const int LINE_SIZE = 8; // variable
const int CACHE_LINE_NUM = CACHE_SIZE / LINE_SIZE;
const int WAY_NUM = 8;   // variable
const bool FULL = false; // variable
const ReplacePolicy rp = ReplacePolicy::Score; // variable
const WritePolicy0 w0 = WritePolicy0::WriteBack;  // variable
const WritePolicy1 w1 = WritePolicy1::WriteAlloc; // variable

const int ONE_WAY_LINE_NUM = CACHE_LINE_NUM / WAY_NUM;
const int OFFSET_BITS = log2(LINE_SIZE); // 3
const int INDEX_BITS = log2(ONE_WAY_LINE_NUM); // 11
const int TAG_BITS = ADDRESS_WIDTH - OFFSET_BITS - INDEX_BITS; // 50

// helper
const int BT_HEIGHT = log2(WAY_NUM);
const int LRU_LINE_SIZE = log2(WAY_NUM) * WAY_NUM;
#endif