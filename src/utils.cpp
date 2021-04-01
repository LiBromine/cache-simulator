#include "utils.h"
#include "config.h"
#include <iostream>

extern ReplacePolicy rp;
extern WritePolicy0 w0;
extern WritePolicy1 w1;

long long getTag(long long addr, int width, int prefix_width) {
    addr >>= prefix_width;
    long long mask = (1 << width) - 1;
    return addr & mask;
}

int getIndex(long long addr, int width, int offset_width) {
    addr >>= offset_width;
    int mask = (1 << width) - 1;
    return addr & mask;
}

int getOffset(long long addr, int width) {
    int mask = (1 << width) - 1;
    return addr & mask;
}

void printConfig(std::string input) {
    std::cout << "-----  CONFIG  -----" << std::endl;
    std::cout << "Cache Size:                " << std::hex << "0x" << CACHE_SIZE << " Bytes." << std::endl;
    std::cout << "Address Width:             " << std::dec << ADDRESS_WIDTH << " Bits." << std::endl;
    std::cout << "Line/Block Size(variable): " << std::dec << LINE_SIZE << " Bytes." << std::endl;
    std::cout << "Way Number(variable):      " << std::dec << WAY_NUM << " ." << std::endl;
    std::cout << "Replace Policy(variable):  " << print(rp) << " ." << std::endl;
    std::cout << "Write Strategy(variable):  " << print(w0) << ", " << print(w1) << " ." << std::endl;

    std::cout << "-----  PARAMS  -----" << std::endl;
    std::cout << "Cache Line Number:                " << std::dec << CACHE_LINE_NUM << " ." << std::endl;
    std::cout << "Cache Line Number in One Way:     " << std::dec << ONE_WAY_LINE_NUM << " ." << std::endl;
    std::cout << "Offset Bits:                      " << std::dec << OFFSET_BITS << " Bits." << std::endl;
    std::cout << "Index Bits:                       " << std::dec << INDEX_BITS << " Bits." << std::endl;
    std::cout << "Tag Bits:                         " << std::dec << TAG_BITS << " Bits." << std::endl;
    std::cout << "Input File:                       " << input << " ." << std::endl;

    std::cout << "----- RW BEGIN -----" << std::endl;
}

std::string print(WritePolicy1 w) {
    switch (w)
    {
    case WritePolicy1::WriteAlloc: return "Write Alloc";
    case WritePolicy1::WriteNonAlloc: return "Write Non-Alloc";
    default:
        return "None";
    }
}

std::string print(WritePolicy0 w) {
    switch (w) {
    case WritePolicy0::WriteThrough: return "Write Through";
    case WritePolicy0::WriteBack: return "Write Back";
    default:
        return "None";
    }
}

std::string print(ReplacePolicy r) { 
    switch (r) {
    case ReplacePolicy::BT: return "Binary Tree";
    case ReplacePolicy::LRU: return "LRU stack";
    case ReplacePolicy::Direct: return "Direct Mapping";
    default:
        return "None";
    }
}

void printBinary(long long target, int width) {
    for (int i = 0; i < width; i++) {
        std::cout << ((target >> (width - 1 - i)) & 0x1);
    }
}

void printLineBinary(long long target, int width) {
    printBinary(target, width);
    std::cout << std::endl;
}

void printUsage() {
    std::cout << "Usage: ./main [src_trace]" << std::endl;
    std::cout << "Usage: ./main [src_trace] [rp] [w0] [w1]" << std::endl;
    std::cout << "Usage: ./main [src_trace] [rp] [w0] [w1] [dst]" << std::endl;
}

ReplacePolicy parseRp(std::string s) {
    if (s == "LRU") {
        return ReplacePolicy::LRU;
    } else if (s == "Direct") {
        return ReplacePolicy::Direct;
    } else if (s == "Score") {
        return ReplacePolicy::Score;
    } else {
        return ReplacePolicy::BT;
    } 
}

WritePolicy0 parseW0(std::string s) {
    if (s == "through") {
        return WritePolicy0::WriteThrough;
    } else {
        return WritePolicy0::WriteBack;
    } 
}

WritePolicy1 parseW1(std::string s) {
    if (s == "non-alloc") {
        return WritePolicy1::WriteNonAlloc;
    } else {
        return WritePolicy1::WriteAlloc;
    }
}