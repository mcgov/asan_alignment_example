# Manual ASan poisoning and alignment

ASan poisoning has alignment requirements: the user must add padding such that the padding ends on a byte boundary in the shadow memory. Since each bit in the ASan shadow memory encodes the state of a byte in real memory, this means that **the size + padding for each allocation must align on a 8 byte boundary.** If this requirement is not satisfied it can lead to incorrect bug reporting, including missed and false-positive reports.

See the included examples for a little background. One is a small program to show what can go wrong with manual shadow memory poisoning. The second is an example implementation of manual poisoning using the `std::allocator` interface.

To build and run the allocator example:

   `nmake allocator && allocator.exe`

To build the 'bad unpoisoning' example:

   `nmake unpoison && unpoison.exe`

For further information on the shadow bytes and asan see 

[AddressSanitizer: A Fast Address Sanity Checker](https://static.googleusercontent.com/media/research.google.com/en//pubs/archive/37752.pdf)

[Microsoft documentation for Sanitizers/Shadow Bytes](https://docs.microsoft.com/en-us/cpp/sanitizers/asan-shadow-bytes?view=msvc-170)
