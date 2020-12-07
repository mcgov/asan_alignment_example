#define __SANITIZE_ADDRESS__ 
#include "sanitizer/asan_interface.h"


static __declspec(align(0x1000)) char array[0x100000];

int main()
{
    // start the program by poisoning the whole region.
    ASAN_POISON_MEMORY_REGION(array, 0x100000);

    // try to unpoison a small chunk at the end of a qword 
    //   and two leading bytes from the next qword.
    // This ends up unpoisoning more of the first qword 
    //  than it should.
    ASAN_UNPOISON_MEMORY_REGION(array+4, 6);

    // try to re-poison the same area, 
    //  Note, this sort of works but isn't commutative.
    // ie the mistake from the first poisoning isn't fixed.
    // the leading bytes remain unpoisoned after the operation.
    ASAN_POISON_MEMORY_REGION(array+4, 6);

    array[0] = 0xff; //this access should be poisoned, but it's not!
}

