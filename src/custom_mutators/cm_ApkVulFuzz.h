#ifndef CUSTOM_MUTATOR_H
#define CUSTOM_MUTATOR_H

#ifdef AFL_CM
  #include "afl-fuzz.h"
#else
    typedef void afl_state_t;
    typedef uint8_t u8;
#endif

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <time.h>
#include <limits.h>
#include <stdint.h>

#define AFL_CUSTOM_MUTATOR_FAILED { *out_buf = NULL; return 0; } // We cannot work with this

typedef struct my_mutator {

  #ifdef AFL_CM
    afl_state_t *afl;
  #endif
  
    char *out_buf; // The whole seed in a buffer
  
    uint64_t buf_size; // Full size in bytes of the APK
    uint64_t i; // Offset to fuzz from 
    uint64_t j; // Offset to fuzz to 

} my_mutator_t;

typedef struct {
    const char *apk_name;
    uint64_t start_offset;
    uint64_t end_offset;
} ApkEntry;
extern ApkEntry apk_table[];
extern const size_t apk_table_size;

// AFL++ Interface functions
my_mutator_t *afl_custom_init(afl_state_t *afl, unsigned int seed);
void afl_custom_deinit(my_mutator_t *data);

size_t afl_custom_fuzz(my_mutator_t *data, uint8_t *buf, size_t buf_size,
                       u8 **out_buf, uint8_t *add_buf,
                       size_t add_buf_size,  // add_buf can be NULL
                       size_t max_size);


#endif // CUSTOM_MUTATOR_H
