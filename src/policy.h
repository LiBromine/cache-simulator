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



// TODO
class LRUstack {
public:
    int getRank(int index, char action);
};

class DirectMap {
public:
    int getRank(int index, char action);
};

#endif