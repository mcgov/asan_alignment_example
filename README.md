# Manual ASan poisoning and alignment

ASan poisoning has alignment requirements, we must align padding such that the padding ends on a byte boundary in the shadow memory. This means **the size + padding for each element must align on a 8 byte boundary.**

See the included examples for a little background. One is a small program to show what can go wrong with manual shadow memory poisoning. The second is an example implementation of manual poising using `std::aligned_storage` and `std::allocator`.

To build and run the allocator example:

   `nmake allocator & allocator.exe`

To build the 'bad unpoisoning' example:

   `nmake unpoison`
This example is better illustrated by stepping through in a debugger.

