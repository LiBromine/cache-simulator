#include "policy.h"
#include <cstring>
// #include <iostream>

/* Direct method */
int DirectMap::getRank(int index, char action) {
    return index;
}

/* Btree method */
Btree::Btree() {
    this->data = new BtreeLine[ONE_WAY_LINE_NUM];
}

Btree::~Btree() {
    delete []this->data;
}

int Btree::getRank(const Cache *cache, long long tag, int index) {
    return this->data[index].access(cache, tag, index);
}

/* LRUstack method */
int LRUstack::getRank(int index, char action) {
    return index;
}

/* BtreeLine method */
BtreeLine::BtreeLine() {
    int lineSize = (WAY_NUM + 7) >> 3;
    memset(meta, 0, lineSize);
}

// protected
void BtreeLine::set(int k) {
    meta[k >> 3] |= 0x80 >> (k & 0x07);
}

bool BtreeLine::test(int k) {
    return meta[k >> 3] & (0x80 >> (k & 0x07));
}

void BtreeLine::clear(int k) {
    meta[k >> 3] &= ~(0x80 >> (k & 0x07));
}

int BtreeLine::access(const Cache* cache, long long tag, int index) {
    if (!this->test(0)) {
        // Not all valid, hit or allocate
        // std::cout << "not all valid" << std::endl;
        // in this case, WAY_NUM - i - 1 represents the ith line's allocation
        for (int i = 0; i < WAY_NUM; i++) {
            if (this->test(WAY_NUM - 1 - i)) {
                // compare with a allocated line
                // may not return 
                bool hit = cache->data[index + ONE_WAY_LINE_NUM * i].isHit(tag);
                if (hit) {
                    // std::cout << "hit with index " << index + ONE_WAY_LINE_NUM * i << std::endl;
                    return index + ONE_WAY_LINE_NUM * i;
                }
            } else {
                // allocate a free line
                // must return
                this->set(WAY_NUM - 1 - i);
                if (i == WAY_NUM - 1) { // go into All-valid state
                    memset(meta, 0, (WAY_NUM + 7) >> 3);
                    this->set(0);
                }
                // std::cout << "allocate with index " << index + ONE_WAY_LINE_NUM * i << std::endl;
                return (index + ONE_WAY_LINE_NUM * i);
            }
        }
    } else {
        // All valid, hit or replace
        // std::cout << "all valid" << std::endl;
        for (int i = 0; i < WAY_NUM; i++) {
            // compare with a allocated line
            // may not return
            bool hit = cache->data[index + ONE_WAY_LINE_NUM * i].isHit(tag);
            if (hit) {
                // update the BT
                int node = (i + WAY_NUM);
                for (int j = 0; j < BT_HEIGHT; j++) {
                    if (node & 0x1) {
                        this->clear(node >> 1);
                    } else {
                        this->set(node >> 1);
                    }
                    node >>= 1;
                }
                // std::cout << "hit with index " << index + ONE_WAY_LINE_NUM * i << std::endl;
                return index + ONE_WAY_LINE_NUM * i;
            } 
        }

        // not hit, must replace according to BtreeLine
        int k = 1;
        for (int j = 0; j < BT_HEIGHT; j++) {
            if (this->test(k)) {
                this->clear(k);
                k <<= 1;
                k += 1;
            } else {
                this->set(k);
                k <<= 1;
            }
        }
        k -= WAY_NUM;
        // std::cout << "replace index " << index + ONE_WAY_LINE_NUM * k << std::endl;
        return index + ONE_WAY_LINE_NUM * k;
    }

    // error
    exit(-1);
    return -1;
}