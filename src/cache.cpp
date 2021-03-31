#include "cache.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <string>

// using namespace std;

Cache::Cache(WritePolicy0 w0_, WritePolicy1 w1_) {
    total_count = 0;
    miss_count = 0;
    hot_hit = false;
    w0 = w0_;
    w1 = w1_;

    data = new CacheLine[CACHE_LINE_NUM];
}

Cache::~Cache() {
    delete []data;
}

bool Cache::access(int rank, long long tag, char action, bool print) {
    
    // 1. is hit
    bool valid = data[rank].isValid();
    if (valid) {
        // std::cout << "I am valid" << std::endl;
        hot_hit = data[rank].isHit(tag);
    } else {
        // std::cout << "I am not valid" << std::endl;
        hot_hit = false;
    }

    // 2. alloc or not
    if (!hot_hit) {
        // std::cout << "I am not hit" << std::endl;
        bool cond = false;
        if (action == 'r') cond = true;
        else {
            if (w1 == WritePolicy1::WriteAlloc) cond = true;
        }
        if (cond) {
            data[rank].allocate(tag);
        }
    }
    
    // 3. dirty or not
    if (action == 'w' && w0 == WritePolicy0::WriteBack && hot_hit) {
        data[rank].setDirty();
    }

    // 4. print log or not
    if (print) printLog();

    // 5. count 
    if (!hot_hit) {
        miss_count++;
    }
    total_count++;

    return hot_hit;
}

void Cache::printLog() {
    if (hot_hit) {
        std::cout << "Hit\n";
    } else {
        std::cout << "Miss\n";
    }
}

void Cache::printInfo() {
    std::cout << "Total Count: " << total_count << std::endl;
    std::cout << "Miss  Count: " << miss_count << std::endl;
    std::cout << "Miss   Rate: " << double(miss_count) / total_count * 100 << " %" << std::endl;
}

/* -----  Cache line method ----- */

CacheLine::CacheLine() {
    memset(meta, 0, N);
}

bool CacheLine::isValid() {
    return this->test(0);
}

void CacheLine::allocate(long long tag) {
    this->set(0); // set valid
    for (int i = 0; i < TAG_BITS; i++) {
        int flag = tag & 0x01;
        if (flag) {
            this->set(i + 1);
        } else {
            this->clear(i + 1);
        }
        tag >>= 1;
    }
}

void CacheLine::setDirty() {
    this->set(TAG_BITS + 1);
}

bool CacheLine::isHit(long long tag) {
    if (!this->isValid()) {
        return false;
    }
    // std::cout << "I am vaild" << std::endl;
    for (int i = 0; i < TAG_BITS; i++) {
        bool flag = (tag & 0x01) ^ this->test(i + 1);
        if (flag) {
            // std::cout << "non-consistent at position " << i << std::endl;
            return false;
        }
        tag >>= 1;
    }
    return true;
}

// protected
void CacheLine::set(int k) {
    meta[k / 8] |= 0x80 >> (k & 0x07);
}

bool CacheLine::test(int k) {
    return meta[k / 8] & (0x80 >> (k & 0x07));
}

void CacheLine::clear(int k) {
    meta[k >> 3] &= ~(0x80 >> (k & 0x07));
}
