#include "arena_allocator.h"

int main () {
    
    ArenaAllocator<size_t> myAllocator;
    size_t *one = myAllocator.allocate(1);
    size_t *two = myAllocator.allocate(2);
    size_t *three = myAllocator.allocate(3);
    size_t *four = myAllocator.allocate(4);
    
    two[2] = 0xFF;
    three[-1] = 0xFF;
    three[3] = 0xFF;
    four[-1] = 0xFF;


    return 1;
}