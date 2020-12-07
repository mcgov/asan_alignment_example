#define __SANITIZE_ADDRESS__ 
#include "sanitizer/asan_interface.h"


static __declspec(align(0x1000)) char array[0x100000];

int main()
{
    // start the program by poisoning the whole region.
    ASAN_POISON_MEMORY_REGION(array, 0x100000);

    // try to unpoison a small chunk at the end of a qword 
    //   and two leading bytes from the next qword.
    // This ends up unpoisoning more than it should.
    ASAN_UNPOISON_MEMORY_REGION(array+4, 6);

    // try to re-poison the same area, 
    //  which sort of works but doesn't fix the mistake from the last unpoison.
    ASAN_POISON_MEMORY_REGION(array+4, 6);

    // re-poison the first four bytes of the region.
    ASAN_POISON_MEMORY_REGION(array, 4);

    ASAN_UNPOISON_MEMORY_REGION(array+6, 3);

    ASAN_UNPOISON_MEMORY_REGION(array+11, 2);

}