#include "sanitizer/asan_interface.h"
#include "shadow_memory_printing.h"
#include <stdint.h>
#include <stdio.h>


constexpr size_t array_size = 0x1000;
static char array[array_size];

int main() {

  // To begin, grab the shadow memory parameters for this architecture. On x64
  // the shadow memory offset is dynamic, on x86 it's always 0x30000000. Shadow
  // memory scale changing is not supported for MSVC, it will always return 3.
  __asan_get_shadow_mapping(&shadow_memory_scale, &shadow_memory_offset);

  // Poison the entire array. We recommended users poison their entire arena and
  // unpoison individual elements as needed.
  ASAN_POISON_MEMORY_REGION(array, array_size);
  PRINT_SHADOW_MEMORY(array);

  // Try to unpoison a small chunk at the end of a qword and two leading bytes
  // from the next qword. This ends up unpoisoning more of the first qword than
  // it should.
  ASAN_UNPOISON_MEMORY_REGION(array + 4, 6);
  PRINT_SHADOW_MEMORY(array);

  // NOTE: shadow memory snippets in the comments are from x86.
  // 30045000  00 02 f7 f7 f7 f7 f7 f7
  //            ^
  // by unpoisoning the end of the first qword,
  // we've inadvertently unpoisoned the entire region.

  // re-poison the area we just poisoned,
  // note the mistake from the first poisoning isn't fixed.
  // the leading bytes remain unpoisoned after this operation.
  ASAN_POISON_MEMORY_REGION(array + 4, 6);
  PRINT_SHADOW_MEMORY(array);
  // 30045000  04 f7 f7 f7 f7 f7 f7 f7

  // All of these attempts to unpoison the first few bytes
  // of the allocation will fail to poison anything.
  ASAN_POISON_MEMORY_REGION(array, 1);
  ASAN_POISON_MEMORY_REGION(array, 2);
  ASAN_POISON_MEMORY_REGION(array, 3);
  PRINT_SHADOW_MEMORY(array);
  // 30045000  04 f7 f7 f7 f7 f7 f7 f7

  array[0] = 0xff; // you might expect this access to be
                   // partially poisoned, but it's not!

  // Key Takeaway: ensuring the proper alignment of your elements
  //  and padding is essential when asan-izing your allocators.
}
