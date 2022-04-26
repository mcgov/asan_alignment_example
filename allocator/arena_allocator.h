#pragma once
#include "sanitizer/asan_interface.h"
#include <memory>
#include <new> // bad_alloc, bad_array_new_length
#include <stdint.h>
#include <stdlib.h> //size_t, malloc, free

/* NOTE: This is a non-thread-safe, completely contrived example to illustrate
 * ASan's alignment requirements. Do not attempt to use this for anything other
 * than this example! */

// The allocator will use a fixed arena and allocate smaller sub-arrays from
// that storage.

/* A function to calculate the padding needed per allocation. Adding padding per
 * element breaks array iteration (in this scenario). So we will only pad the
 * ends of each array that is allocated. */

size_t CalculateSizePlusPadding(size_t allocation_size, size_t padding) {
  size_t combined_size = allocation_size + padding;
  if (combined_size % 8 != 0) {
    return combined_size +
           (8 - (combined_size % 8)); // pad to a qword boundary.
  }
  return combined_size;
}

// This example allocator is based on the std::allocator interface,
// the example is also basically stolen from the 'mallocator' example
// https://devblogs.microsoft.com/cppblog/the-mallocator/

// keep a non-const cursor for the current data position in the arena.
// totally contrived, sample-code-only pattern.
// start is one qword inside the arena.
size_t arena_cursor = 0x10;

template <class T> struct ArenaAllocator {
  typedef T value_type;
  size_t ArenaSize = 0x1000 * sizeof(value_type);

  // add (at least) a qword of padding per allocation.
  size_t padding_size = 0x10;

  // allocate the aligned arena
  uint8_t *Arena = (uint8_t *)_aligned_malloc(ArenaSize, 0x10);

  // poison upon construction.
  ArenaAllocator() noexcept { ASAN_POISON_MEMORY_REGION(Arena, ArenaSize); }

  // std::allocator members
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
  if (n > ArenaSize) {
    throw std::bad_array_new_length();
  }
  if (arena_cursor + n >= ArenaSize) {
    throw std::bad_alloc();
  }
  // get pointer to next allocation in area.
  void *new_alloc = (void *)&Arena[arena_cursor];

  // calculate allocation size
  size_t allocation_size = sizeof(T) * n;
  // unposion the allocation
  ASAN_UNPOISON_MEMORY_REGION(new_alloc, allocation_size);

  // move cursor to next aligned item start
  arena_cursor += CalculateSizePlusPadding(allocation_size, padding_size);

  return static_cast<T *>(new_alloc);
}

template <class T>
void ArenaAllocator<T>::deallocate(T *const p, size_t s) const noexcept {
  // unpoison the allocation.
  ASAN_POISON_MEMORY_REGION(p, s * sizeof(T));
  // NOTE: This will not unpoison the entire allocation if a different
  // size is passed in than was allocated.
}