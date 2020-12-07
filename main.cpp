#include "arena_allocator.h"

int main () {
    
    ArenaAllocator<size_t> myAllocator;
    size_t *p1 = myAllocator.allocate(1);
    size_t *p2 = myAllocator.allocate(2);
    size_t *p3 = myAllocator.allocate(3);
    size_t *p4 = myAllocator.allocate(4);
    
    //myAllocator.deallocate(p3,3);
    p2[2] = 0xFF;
    p3[-1] = 0xFF;
    p3[3] = 0xFF;
    p4[-1] = 0xFF;


    return 1;
}