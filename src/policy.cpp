#include "policy.h"
#include <cstring>
// #include <iostream>

/* Direct method */
int DirectMap::getRank(const Cache *cache, long long tag, int index) {
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
LRUstack::LRUstack() {
    this->data = new LRULine[ONE_WAY_LINE_NUM];
}

LRUstack::~LRUstack() {
    delete []this->data;
}
int LRUstack::getRank(const Cache *cache, long long tag, int index) {
    return this->data[index].access(cache, tag, index);
}

/* LRULine method */
LRULine::LRULine() {
    for (int i = 0; i < WAY_NUM; i++) {
        this->setTuple(i, WAY_NUM - 1 - i);
    }
}

// protected
void LRULine::set(int k) {
    meta[k >> 3] |= 0x80 >> (k & 0x07);
}

bool LRULine::test(int k) {
    return meta[k >> 3] & (0x80 >> (k & 0x07));
}

void LRULine::clear(int k) {
    meta[k >> 3] &= ~(0x80 >> (k & 0x07));
}

void LRULine::setTuple(int pos, int val) {
    // assert: 0 <= pos <= 7, 0 <= val <= 7
    pos &= 0x7;
    if (val & 0x4) this->set(pos * 3);
    else this->clear(pos * 3);
    if (val & 0x2) this->set(pos * 3 + 1);
    else this->clear(pos * 3 + 1);
    if (val & 0x1) this->set(pos * 3 + 2);
    else this->clear(pos * 3 + 2);
}

int LRULine::getTuple(int pos) {
    // assert: 0 <= pos <= 7
    int ret = 0;
    for (int i = 0; i < 3; i++) {
        ret <<= 1;
        ret |= this->test(pos * 3 + i);
    }
    return ret & 0x7;
}

int LRULine::findTuple(int val) {
    // assert 0 <= val <= 7
    for (int i = 0; i < WAY_NUM; i++) {
        if (this->getTuple(i) == val) {
            return i;
        }
    }
    // assert: code will not get here. 
    exit(-1);
}

int LRULine::access(const Cache* cache, long long tag, int index) {
    // try to hit directly, may not return
    int replace = -1;
    for (int i = 0; i < WAY_NUM; i++) {
        int tmpIndex = index + i * ONE_WAY_LINE_NUM;
        if (cache->data[tmpIndex].isHit(tag)) {
            // hit, maintain the stack
            // replace = this->getTuple(0);
            int rankInStack = this->findTuple(i);
            for (int j = rankInStack; j < WAY_NUM - 1; j++)
                this->setTuple(j, this->getTuple(j + 1)); 
            this->setTuple(WAY_NUM - 1, i);
            return tmpIndex;
        }
    }

    // try to find a unallocated line, may not return
    for (int i = 0; i < WAY_NUM; i++) {
        int tmpIndex = index + i * ONE_WAY_LINE_NUM;
        if (!cache->data[tmpIndex].isValid()) {
            // find a free line, maintain the index
            // replace = this->getTuple(0);
            int rankInStack = this->findTuple(i);
            for (int j = rankInStack; j < WAY_NUM - 1; j++)
                this->setTuple(j, this->getTuple(j + 1)); 
            this->setTuple(WAY_NUM - 1, i);
            return tmpIndex;
        }
    }

    // replace
    replace = this->getTuple(0);
    for (int j = 0; j < WAY_NUM - 1; j++)
        this->setTuple(j, this->getTuple(j + 1));
    this->setTuple(WAY_NUM - 1, replace); 
    return index + replace * ONE_WAY_LINE_NUM;
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

/* Selector method */
Selector::Selector(ReplacePolicy rp_) {
    this->rp = rp_;
    switch (this->rp) {
    case ReplacePolicy::Direct:
        this->D = new DirectMap();
        break;
    case ReplacePolicy::BT:
        this->BT = new Btree();
        break;
    case ReplacePolicy::LRU:
        this->LRU = new LRUstack();
        break;
    default:
        break;
    }
}

Selector::~Selector() {
    switch (this->rp) {
    case ReplacePolicy::Direct:
        delete this->D;
        break;
    case ReplacePolicy::BT:
        delete this->BT;
        break;
    case ReplacePolicy::LRU:
        delete this->LRU;
        break;
    default:
        break;
    }
}

int Selector::getRank(const Cache *cache, long long tag, int index) {
    switch (this->rp) {
    case ReplacePolicy::Direct:
        return this->D->getRank(cache, tag, index);
        break;
    case ReplacePolicy::BT:
        return this->BT->getRank(cache, tag, index);
        break;
    case ReplacePolicy::LRU:
        return this->LRU->getRank(cache, tag, index);
        break;
    default:
        exit(-1);
    }
}