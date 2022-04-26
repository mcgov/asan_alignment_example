#include "arena_allocator.h"

int main() {
  struct unaligned_struct {
    uint8_t data[7];
  };

  typedef struct unaligned_struct arena_item;
  ArenaAllocator<arena_item> myAllocator;

  arena_item *one = myAllocator.allocate(1);
  arena_item *two = myAllocator.allocate(2);
  arena_item *three = myAllocator.allocate(3);
  arena_item *four = myAllocator.allocate(4);

  two[2].data[0] = 0xff; // trigger an ASan report.
  // the report will show the shadow memory layout.
  // comment out line 16 to observe a deallocated access.

  myAllocator.deallocate(four, 4);
  four[0].data[5] = 0xff; // trigger another ASan report.
}
/*Example dump from line 16 oob access:
                        one      two         three          four
=>0x043cdf294040: f7 f7 07 f7 f7 00[06]f7 f7 00 00 05 f7 f7 00 00
  0x043cdf294050: 00 04 f7 f7 f7 f7 f7 f7 f7 f7 f7 f7 f7 f7 f7 f7

*/