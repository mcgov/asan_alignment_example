#pragma once

static char *formatted_shadow_memory_print_str =
    "%0p: %02hhx %02hhx %02hhx %02hhx %02hhx %02hhx %02hhx %02hhx\n";
size_t shadow_memory_scale, shadow_memory_offset;

void __declspec(no_sanitize_address) print_shadow_memory(void *address) {
  uint8_t *shadow_memory = (uint8_t *)((size_t)address >> shadow_memory_scale) +
                           shadow_memory_offset;
  printf(formatted_shadow_memory_print_str, address, shadow_memory[0],
         shadow_memory[1], shadow_memory[2], shadow_memory[3], shadow_memory[4],
         shadow_memory[5], shadow_memory[6], shadow_memory[7]);
}
#define PRINT_SHADOW_MEMORY(address)                                           \
  printf("Shadow Memory at " __FILE__ ":%d\n", __LINE__);                      \
  print_shadow_memory(address);
