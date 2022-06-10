#include "arena_allocator.h"

int main() {
  struct unaligned_struct {
    uint8_t data[7];
  };

  typedef struct unaligned_struct arena_item;
  ArenaAllocator<arena_item> myAllocator;

  // make some arenas, sized 1 to 8 elements
  arena_item *arenas[8];
  for (int i = 0; i < 8; i++) {
    arenas[i] = myAllocator.allocate(i + 1);
  }

  arena_item *arena_two = arenas[1];
  arena_item *arena_four = arenas[3];

  // trigger an ASan report.
  // the report will show the shadow memory layout.
  arena_two[2].data[0] = 0xff;
  // accessing arena_two[1].data[7] would generate the same report

  // comment out line 16 to observe uaf of a deallocated arena
  myAllocator.deallocate(arena_four, 4);
  // trigger another ASan report.
  arena_four[0].data[5] = 0xff;
}

/*
Snippet of asan dump from the oob access on line 16:
                     one   two      three       four
=>0x03ccf5084040: f7 07 f7 00[06]f7 00 00 05 f7 00 00 00 04 f7 00
Note:
 - The arena with 1 struct with a size of 7 bytes has a shadow
    value of 0x07.
 - The arena with size 14 has two bytes in the shadow memory:
    - One byte with value 0x00
    - One with value 0x06, aka ( 2 * 7 ) % 8
 - The padding regions have value 0xF7, indicating the region
    was poisoned by the user.
*/