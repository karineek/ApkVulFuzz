#ifndef CUSTOM_MUTATOR_H
#define CUSTOM_MUTATOR_H

#ifdef AFL_CM
  #include "afl-fuzz.h"
#else
    typedef void afl_state_t;
#endif

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <time.h>
#include <limits.h>

typedef struct my_mutator {

  #ifdef AFL_CM
    afl_state_t *afl;
  #endif
  
    char *out_buf; // The whole seed in a buffer
  
    uint64_t buf_size; // Full size in bytes of the APK
    uint64_t i; // Offset to fuzz from 
    uint64_t j; // Offset to fuzz to 

} my_mutator_t;

// AFL++ Interface functions
my_mutator_t *afl_custom_init(afl_state_t *afl, unsigned int seed);
void afl_custom_deinit(my_mutator_t *data);

size_t afl_custom_fuzz(my_mutator_t *data, uint8_t *buf, size_t buf_size,
                       u8 **out_buf, uint8_t *add_buf,
                       size_t add_buf_size,  // add_buf can be NULL
                       size_t max_size);


#endif // CUSTOM_MUTATOR_H
