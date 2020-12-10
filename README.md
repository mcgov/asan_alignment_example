## Manual ASan poisoning and alignment

   ASan poisoning has alignment requirements, we must align padding such that the padding ends on a byte boundary in the shadow memory.

   This requirement is a little more clear when you look at the shadow memory itself.

   After compiling for x86 where sizeif(size_t) is 4, the shadow memory that is generated for this example in main.cpp will look something like this: 

```
   f7 f7 04 00 f7 00 04 00 00 f7
```

Each bit in the shadow memory encodes the accesibility of a byte of actual memory. This example has :

    16 bytes of padding to start (f7 f7).
    4 bytes of accesible memory with 4 bytes of padding (04)
    8 bytes of accessible memory (00)
    8 bytes of padding (f7)
    12 bytes of accesible memory followed by 4 bytes of padding (00 04)
    16 bytes of accessible memory (00 00)
    8 bytes of padding (f7)
   
Element p1 is 4 bytes large, so it will only take up a nibble of shadow
   memory, this is the 04 you see when reading from the left. This 04 means that only the 4 most significant bytes of the qword are accessable. The rest of the bytes in that qword are unaddressable.
   Let's use a small byte array as an example, we'll set it at address 0 and use a 32 bit address space for this example.

   0x00000000 : [ 0x00 0x11 0x22 0x33 0x44 0x55 0x66 0x77 ] ...

Say that we wanted to mark half of this array as being inaccessible. We could use the ASAN_POISON_MEMORY_REGION macro to poison address 0, sized 4.

``` 
    0 0 0 0 X X X X
```
 Element p2 is 8 bytes large and will take up a full byte of shadow
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