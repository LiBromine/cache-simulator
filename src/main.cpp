#include <iostream>
#include <string>
#include <bitset>
#include <fstream>
#include "config.h"
#include "cache.h"
#include "policy.h"
#include "utils.h"
// #define DEBUG

using namespace std;

int main() {
    
    // other config
    string input = "../test_trace/1.trace"; // var
    // string output = "../out/8B-8way-BT-1.log"; // var
    string output = "../out/debug.log";
    bool print = false;
    const int COUNTER_MAX = 1000;

    // init
    Btree selector;
    ifstream f(input);
    ofstream of(output);
    Cache cache(w0, w1);

    // local var
    string line;
    long long addr;
    char action;
    long long tag = 0;
    int index = 0;
    int offset = 0;
    int rank = 0;
    int counter = 0;

    // print config
    printConfig(input);

    
    // access memory
    if (f.is_open()) {
        while (getline(f, line)) {
#ifdef      DEBUG
            // 0. debug counter
            if (counter++ >= COUNTER_MAX) {
                break;
            }
#endif

            // 1. read line and parse, get addr and action
            int i = 2;
            addr = 0;
            while (true) {
                if (line[i] == '\t') {
                    break;
                }

                long long flag = (line[i] == '1') ? 1: 0;
                addr <<= 1;
                addr |= flag;
                i++;
            }
            action = line[++i];
        
#ifdef      DEBUG
            printBinary(addr, ADDRESS_WIDTH);
            cout << "  " << action << endl; 
#endif

            // 2. get tag, index and offset
            offset = getOffset(addr, OFFSET_BITS);
            index = getIndex(addr, INDEX_BITS, OFFSET_BITS);
            tag = getTag(addr, TAG_BITS, OFFSET_BITS + INDEX_BITS);

#ifdef      DEBUG
            // printLineBinary(offset, OFFSET_BITS);
            // printLineBinary(index, INDEX_BITS);
            // printLineBinary(tag, TAG_BITS);
#endif

            // 3. choose one block
            rank = selector.getRank(&cache, tag, index);
#ifdef      DEBUG
            cout << rank << endl;
#endif

            // 4. access the block
            bool hit = cache.access(rank, tag, action, false); 
            if (print) {
                if (hit) of << "Hit\n";
                else of << "Miss\n";
            }
        }
    }
    f.close();
    of.close();
    cache.printInfo();
}