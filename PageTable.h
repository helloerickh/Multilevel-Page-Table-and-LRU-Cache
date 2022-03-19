#ifndef PAGETABLE_H
#define PAGETABLE_H

#include "Level.h"
#include "Map.h"
#include <vector>

#define ADDRESS_SIZE 32

class PageTable{
    public:
    int levelCount;
    std::vector<unsigned int> bitmasks;
    std::vector<unsigned int> shiftInfo;
    std::vector<unsigned int> numEntriesPerLevel;
    class Level* root;
    PageTable(std::vector<unsigned int> pageSizes);
    void printBitMasks();
    void printBitShifts();
    void printEntriesPerLevel();
};

unsigned int generateBitShift(unsigned int addressSize, unsigned int pageSize, int currBit);
unsigned int generateBitMask(unsigned int length, unsigned int start);

#endif

