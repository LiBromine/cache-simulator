#include "config.h"
#include <string>

long long getTag(long long addr, int width, int prefix_width);
int getIndex(long long addr, int width, int offset_width);
int getOffset(long long addr, int width);
void printConfig(std::string);

std::string print(WritePolicy0 w);
std::string print(WritePolicy1 w);
std::string print(ReplacePolicy rp);

void printBinary(long long target, int width);
void printLineBinary(long long target, int width);