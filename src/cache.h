#ifndef CACHE_H
#define CACHE_H
#include "config.h"
#include <string>

const int N = 8;
const int valid_pos = 0;
const int dirty_pos = 1 + TAG_BITS;

class CacheLine {
public:
    char meta[N];
    CacheLine();
    bool isHit(long long tag);
    void allocate(long long tag);
    void setDirty();
    bool isValid();

protected:
    bool test(int k);
    void set(int k);
    void clear(int k);
};

class Cache {
public:
    CacheLine* data;
    int total_count;
    int miss_count;
    bool hot_hit;

    WritePolicy0 w0;
    WritePolicy1 w1;

    Cache(WritePolicy0, WritePolicy1);
    ~Cache();
    bool access(int rank, long long tag, char action, bool print);
    void printInfo();
    void printLog();
};

#endif