#ifndef POLICY_H
#define POLICY_H
#include "config.h"
#include "cache.h"

class BtreeLine {
public:
    char meta[(WAY_NUM + 7) >> 3];
    BtreeLine();
    int access(const Cache *cache, long long tag, int index);

protected:
    bool test(int k);
    void set(int k);
    void clear(int k);
};

class Btree {
public:
    BtreeLine* data;
    int lineSize = (WAY_NUM + 7) >> 3;

    Btree();
    ~Btree();
    int getRank(const Cache *cache, long long tag, int index);
};

// only for WAY_NUM = 8
class LRULine {
public:
    char meta[(8 * 3 + 7) >> 3];
    LRULine();
    int access(const Cache *cache, long long tag, int index);

protected:
    bool test(int k);
    void set(int k);
    void clear(int k);
    void setTuple(int pos, int val);
    int getTuple(int pos);
    int findTuple(int val);
};

class LRUstack {
public:
    LRULine* data;
    LRUstack();
    ~LRUstack();
    int getRank(const Cache *cache, long long tag, int index);
};

class DirectMap {
public:
    int getRank(const Cache *cache, long long tag, int index);
};


class Selector {
public:
    ReplacePolicy rp;
    DirectMap *D;
    LRUstack *LRU;
    Btree *BT;

    int getRank(const Cache *cache, long long tag, int index);
    Selector(ReplacePolicy);
    ~Selector();
};
#endif