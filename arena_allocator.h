#pragma once
#include "sanitizer/asan_interface.h"
#include <memory>
#include <new>      // bad_alloc, bad_array_new_length
#include <stdlib.h> //size_t, malloc, free

// NOTE: This is a non-thread-safe, completely contrived example to
//  illustrate ASan's alignment requirements. Do not attempt to use
//  this for anything other than this example!

// The allocator will have a fixed size and impose the correct padding restrictions.
    typedef std::aligned_storage<sizeof(size_t),16>::type aligned_t;
    constexpr size_t ArenaSize = 0x1000;
    aligned_t Arena[ArenaSize];
    size_t current_index = 1; //start one object inside the arena.


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
  if (n > ArenaSize) {
    throw std::bad_array_new_length();
  }
  if (current_index + n >= ArenaSize) {
    throw std::bad_alloc();
  }
  // poison each element, note we're unpoisoning sizeof(T) 
  // not alignof(T), we want to leave padding poisoned.
  for (int i = 0; i < n; i++){
      void *each_alloc = &Arena[current_index+i];
      ASAN_UNPOISON_MEMORY_REGION(each_alloc, sizeof(T));
  }
  void* returned_alloc = &Arena[current_index];
  current_index += n;
  return static_cast<T *>(returned_alloc);
}

template <class T>
void ArenaAllocator<T>::deallocate(T *const p, size_t s) const noexcept {
  ASAN_POISON_MEMORY_REGION(p, s * alignof(T));
}