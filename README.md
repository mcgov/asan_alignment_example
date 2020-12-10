## Manual ASan poisoning and alignment

   ASan poisoning has alignment requirements, we must align padding such that the padding ends on a byte boundary in the shadow memory, effectively the size + padding must align on a 16 byte boundary.

   See the included examples for a little background and an example implementation using std::aligned_storage.