#pragma once
#include "sanitizer/asan_interface.h"
#include <memory>
#include <new>      // bad_alloc, bad_array_new_length
#include <stdlib.h> //size_t, malloc, free

// NOTE: This is a non-thread-safe, completely contrived example to
//  illustrate ASan's alignment requirements. Do not attempt to use
//  this for anything other than this example!

// NOTE: This example does not apply padding in between items, since std::allocator
// doesn't allow for intra-object padding.

// The allocator will have a fixed size:
const static size_t ArenaSize = 0x10000;
// a fixed arena region which will contain the allocations.
static char alignas(16) Arena[ArenaSize];
// a cursor which will encode the start of the next allocation.
static size_t cursor = 0x10; // starting slightly inside the Arena
// and a minimum padding size, arbitrarily we'll use the size of a pointer.
static const size_t PaddingSize = sizeof(void *);

// This example allocator is based on the std::allocator interface,
// the example is also basically stolen from the 'mallocator' example
// https://devblogs.microsoft.com/cppblog/the-mallocator/

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
    `nmake aligned` to build the example with the correct padding.
    `nmake unaligned` to build without the correct padding.
  */

#ifndef IGNORE_PADDING_REQUIREMENT
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