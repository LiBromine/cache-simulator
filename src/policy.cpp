#include "policy.h"
#include <cstring>
#include <iostream>

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

void BtreeLine::printLine() {
    for (int i = 0; i < WAY_NUM; i++) {
        std::cout << this->test(i);
    }
    std::cout << std::endl;
}

void BtreeLine::insert(int k) {
    int node = (k + WAY_NUM);
    for (int j = 0; j < BT_HEIGHT; j++) {
        if (node & 0x1) {
            this->clear(node >> 1);
        } else {
            this->set(node >> 1);
        }
        node >>= 1;
    }
}

int BtreeLine::find() {
    int k = 1;
    for (int j = 0; j < BT_HEIGHT; j++) {
        if (this->test(k)) {
            k <<= 1;
            k += 1;
        } else {
            k <<= 1;
        }
    }
    return k - WAY_NUM;
}

int BtreeLine::access(const Cache* cache, long long tag, int index) {
    if (!this->test(0)) {
        int flag = 1;
        for (int i = 0; i < WAY_NUM; i++) {
            int tmpIndex = index + i * ONE_WAY_LINE_NUM;
            if (!cache->data[tmpIndex].isValid()) {
                flag = 0;
                break;
            }
        }
        if (flag) this->set(0);
    }


    if (!this->test(0)) {
        // Not all valid, hit or allocate
        // std::cout << "not all valid ";
        for (int i = 0; i < WAY_NUM; i++) {
            // compare with a allocated line
            // may not return
            bool hit = cache->data[index + ONE_WAY_LINE_NUM * i].isHit(tag);
            if (hit) {
                // update the BT
                this->insert(i);
                // std::cout << "hit with index " << index + ONE_WAY_LINE_NUM * i << ' ';
                // printLine();
                return index + ONE_WAY_LINE_NUM * i;
            } 
        }

        for (int i = 0; i < WAY_NUM; i++) {
            // allocate a free line
            int tmpIndex = index + i * ONE_WAY_LINE_NUM;
            if (!cache->data[tmpIndex].isValid()) {
                // find a free line
                this->insert(i);
                return index + ONE_WAY_LINE_NUM * i;
            }
        }
        
    } else {
        // All valid, hit or replace
        // std::cout << "all valid" << ' ';
        for (int i = 0; i < WAY_NUM; i++) {
            // compare with a allocated line
            // may not return
            bool hit = cache->data[index + ONE_WAY_LINE_NUM * i].isHit(tag);
            if (hit) {
                // update the BT
                this->insert(i);
                // std::cout << "hit with index " << index + ONE_WAY_LINE_NUM * i << ' ';
                // printLine();
                return index + ONE_WAY_LINE_NUM * i;
            } 
        }

        // not hit, must replace according to BtreeLine
        int k = this->find();
        this->insert(k);
        // std::cout << "replace index " << index + ONE_WAY_LINE_NUM * k << ' ';
        // printLine();
        return index + ONE_WAY_LINE_NUM * k;
    }

    // error
    exit(-1);
    return -1;
}

/* ScoreLine method */
int ScoreLine::initial = 32;
int ScoreLine::inc = 40;
int ScoreLine::dec = 1;
int ScoreLine::threhold = 24;
int ScoreLine::seed = 1;
int ScoreLine::rand() {
    return ++seed;
}

ScoreLine::ScoreLine() {
    memset(this->meta, 0, (WAY_NUM * bit + 7) >> 3);
}

// protected
void ScoreLine::set(int k) {
    meta[k >> 3] |= 0x80 >> (k & 0x07);
}

bool ScoreLine::test(int k) {
    return meta[k >> 3] & (0x80 >> (k & 0x07));
}

void ScoreLine::clear(int k) {
    meta[k >> 3] &= ~(0x80 >> (k & 0x07));
}

void ScoreLine::setTuple(int pos, int val) {
    // assert: 0 <= pos <= 7, 0 <= val < 2^bit
    pos &= 0x7;
    for (int i = 0; i < bit; i++) {
        if (val & (1 << (bit - i - 1))) this->set(pos * bit + i);
        else this->clear(pos * bit + i);
    }
}

int ScoreLine::getTuple(int pos) {
    // assert: 0 <= pos <= 7
    int ret = 0;
    for (int i = 0; i < bit; i++) {
        ret <<= 1;
        ret |= this->test(pos * bit + i);
    }
    return ret & ((1 << bit) - 1);
}

int ScoreLine::findTuple(int val) {
    return -1;
}

void ScoreLine::decTuple(int pos) {
    int val = this->getTuple(pos);
    if (val - ScoreLine::dec >= 0) val -= ScoreLine::dec;
    else val = 0;
    this->setTuple(pos, val);
}

void ScoreLine::incTuple(int pos) {
    int val = this->getTuple(pos);
    if (val + ScoreLine::inc < (1 << bit)) val += ScoreLine::inc;
    else val = (1 << bit) - 1;
    this->setTuple(pos, val);
}

int ScoreLine::access(const Cache *cache, long long tag, int index) {
    // try to hit, may not return
    for (int i = 0; i < WAY_NUM; i++) {
        int tmpIndex = index + i * ONE_WAY_LINE_NUM;
        if (cache->data[tmpIndex].isHit(tag)) {
            // hit, maintain the score
            for (int j = 0; j < WAY_NUM; j++)
                if (j != i) this->decTuple(j);
            this->incTuple(i);
            return tmpIndex;
        }
    }

    // try to find a unallocated line, may not return
    for (int i = 0; i < WAY_NUM; i++) {
        int tmpIndex = index + i * ONE_WAY_LINE_NUM;
        if (!cache->data[tmpIndex].isValid()) {
            // find a free line, maintain the score
            for (int j = 0; j < WAY_NUM; j++)
                if (j != i) this->decTuple(j);
            this->setTuple(i, ScoreLine::initial);
            return tmpIndex;
        }
    }

    // replace
    int count = 0;
    int minRank = -1;
    for (int i = 0; i < WAY_NUM; i++) { // scanning
        if (this->getTuple(i) <= ScoreLine::threhold) {
            count++;
        }
        if (minRank == -1 || this->getTuple(i) < this->getTuple(minRank)) {
            minRank = i;
        }
    }

    int targetRank = -1;
    if (!count) {
        // target line is the line with the min score
        targetRank = minRank;
    } else {
        // randomize a line with score lowering to threhold
        int rand = ScoreLine::rand() % count;
        for (int i = 0; i < WAY_NUM; i++) {
            if (this->getTuple(i) <= ScoreLine::threhold) {
                if (!rand) {
                    targetRank = i;
                    break;
                }
                rand--;
            }
        }
    }
    // update , and replace the line with min score
    for (int j = 0; j < WAY_NUM; j++)
        if (j != targetRank) this->decTuple(j);
    this->setTuple(targetRank, ScoreLine::initial); // varibale, initial or increase
    return index + targetRank * ONE_WAY_LINE_NUM;
}

/* Score method */
Scorer::Scorer() {
    this->data = new ScoreLine[ONE_WAY_LINE_NUM];
}

Scorer::~Scorer() {
    delete []this->data;
}

int Scorer::getRank(const Cache *cache, long long tag, int index) {
    return this->data[index].access(cache, tag, index);
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
    case ReplacePolicy::Score:
        this->S = new Scorer();
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
    case ReplacePolicy::Score:
        delete this->S;
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
    case ReplacePolicy::Score:
        return this->S->getRank(cache, tag, index);
    default:
        exit(-1);
    }
}