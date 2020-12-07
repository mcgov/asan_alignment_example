#pragma once
#include "sanitizer/asan_interface.h"
#include <memory>
#include <new>      // bad_alloc, bad_array_new_length
#include <stdlib.h> //size_t, malloc, free

// NOTE: This is a non-thread-safe, completely contrived example to
//  illustrate ASan's alignment requirements.

const static size_t ArenaSize = 0x10000;
static char __declspec(align(16)) Arena[ArenaSize];
static size_t cursor = 0x10; // start slightly inside the Arena
static const size_t PaddingSize = sizeof(void *);

template <class T> struct ArenaAllocator {
  typedef T value_type;
  ArenaAllocator() noexcept {
    ASAN_POISON_MEMORY_REGION(Arena, ArenaSize);
  }

  // A converting copy constructor:
  template <class U> ArenaAllocator(const ArenaAllocator<U> &) noexcept {}
  template <class U> bool operator==(const ArenaAllocator<U> &) const noexcept {
    return true;
  }
  template <class U> bool operator!=(const ArenaAllocator<U> &) const noexcept {
    return false;
  }
  T *allocate(const size_t n) const;
  void deallocate(T *const p, size_t) const noexcept;
};

template <class T> T *ArenaAllocator<T>::allocate(const size_t n) const {
  if (n == 0) {
    return nullptr;
  }
  if (n > static_cast<size_t>(ArenaSize) / sizeof(T)) {
    throw std::bad_array_new_length();
  }
  if (cursor + n >= ArenaSize) {
    throw std::bad_alloc();
  }
  // get a pointer at the latest cursor position.
  void *new_alloc = Arena + cursor;
  // leave some padding in between allocated items.
  size_t size_plus_padding = (sizeof(T) * n) + PaddingSize;

  /*
   ASan poisoning has alignment requirements, we must align padding such that
   the padding ends on a byte boundary in the shadow memory.

   This requirement is a little clearer when you look at the shadow memory
   itself.

   When compiled on x86 and size_t size is 4, the shadow memory that is
   generated for this example in main.cpp will look something like this: f7 f7
   04 00 f7 00 04 00 00 f7 f7 |p1| p2 |  p3  | p4    |
   Element p1 is 4 bytes large, so it will only take up a nibble of shadow
   memory. Element p2 is 8 bytes large and will take up a full byte of shadow
   memory.

   The issue is that the padding for this element would normally only take up a
   nibble of shadow space. This would require asan to poison only the higher
   order nibble of this shadow memory, which is not supported.

   Note the poisoned region after p2 is a full byte, rather than just a nibble.
   Without this padding to ensure alignment, ASan will not just fail to poison
   the most significant nibble, it will do it silently! This will result in
   false positive and missed bug reports.

   When manually implementing shadow memory poisoning for a custom allocator,
   users must ensure that elements and padding (combined) are quadword (8 bytes)
   aligned.

  */
#ifdef APPLY_8_BYTE_ALIGNMENT
  if (size_plus_padding % 8 != 0)
    size_plus_padding += 8 - (size_plus_padding % 8);
#endif 
  cursor += size_plus_padding;
  // unpoison the new memory, not including the padding.
  ASAN_UNPOISON_MEMORY_REGION(new_alloc, n * sizeof(T));
  return static_cast<T *>(new_alloc);
}

template <class T>
void ArenaAllocator<T>::deallocate(T *const p, size_t s) const noexcept {
  ASAN_POISON_MEMORY_REGION(p, s * sizeof(T));
}