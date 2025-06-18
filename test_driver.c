#include <stddef.h>
#include <stdio.h>
#include "malloc_shim.h"
#include "compress.h"

#undef malloc
#undef calloc

static unsigned char decompressed_file[65536];
static unsigned char compressed_file[65536];
static unsigned char decompressed_file_again[65536];

int main(int argc, char *argv[]) {
  int successes = 0;
  int failures = 0;
  int skipped = 0;

  for (int i = 1; i < argc; ++i) {
    printf("%s\n", argv[i]);
    
    // Read the whole file
    FILE *f = fopen(argv[i], "rb");
    if (!f) {
      fprintf(stderr, "Skipping %s - couldn't open the file\n", argv[i]);
      skipped += 4;
      continue;
    }
    size_t decompressed_size = fread(decompressed_file, 65536, 1, f);
    if (ferror(f)) {
      fprintf(stderr, "Skipping %s - couldn't read the file after opening it\n", argv[i]);
      fclose(f);
      skipped += 4;
      continue;
    }
    fclose(f);
    memset(&decompressed_file[decompressed_size], 0xAA, 65536 - decompressed_size);
    
    for (int settings_combo = 0; settings_combo < 4; ++settings_combo) {
      pack_options_t options;
      options.optimal = !!(settings_combo & 2);
      options.fast = !!(settings_combo & 1);
      
      size_t compressed_size = 0;
      
      // Try to compress it with these settings (assume that it will eventually succeed)
      for (int allocation_limit = 0; compressed_size == 0; ++allocation_limit) {
        exhal_shim_init(allocation_limit);
        compressed_size = exhal_pack2(decompressed_file, decompressed_size, compressed_file, &options);
      }
      memset(&compressed_file[compressed_size], 0xAA, 65536 - compressed_size);
      
      size_t roundtrip_size = exhal_unpack(compressed_file, decompressed_file_again, NULL);
      if (roundtrip_size != decompressed_size || memcmp(decompressed_file, decompressed_file_again, decompressed_size)) {
        fprintf(stderr, "FAILED: %s / settings %d\n", argv[i], settings_combo);
        ++failures;
      } else {
        ++successes;
      }
    }
  }
  
  printf("Tests completed; %d successes, %d failures, %d skipped.\n", successes, failures, skipped);
  return 0;
}
