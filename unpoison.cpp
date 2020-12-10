#define __SANITIZE_ADDRESS__ 
#include "sanitizer/asan_interface.h"


constexpr size_t array_size = 0x1000;
static char array[array_size];

int main()
{
    // start the program by poisoning the whole region.
    ASAN_POISON_MEMORY_REGION(array, array_size);

    // try to unpoison a small chunk at the end of a qword 
    //   and two leading bytes from the next qword.
    // This ends up unpoisoning more of the first qword 
    //  than it should.
    ASAN_UNPOISON_MEMORY_REGION(array+4, 6);

    // 30045000  00 02 f7 f7 f7 f7 f7 f7
    //            ^
    // by unpoisoning the end of the first qword,
    // we've inadvertently unpoisoned the entire region.

    // re-poison the area we just poisoned,
    // note the mistake from the first poisoning isn't fixed.
    // the leading bytes remain unpoisoned after this operation.
    ASAN_POISON_MEMORY_REGION(array+4, 6);

    // 30045000  04 f7 f7 f7 f7 f7 f7 f7

    // all of these attempts to unpoison the first few bytes 
    // of the allocation will fail to poison anything.
    ASAN_POISON_MEMORY_REGION(array,1);
    ASAN_POISON_MEMORY_REGION(array,2);
    ASAN_POISON_MEMORY_REGION(array,3);

    array[0] = 0xff; //you might expect this access to be 
    // partially poisoned, but it's not!
}

