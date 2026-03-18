#ifndef CUSTOM_MUTATOR_H
#define CUSTOM_MUTATOR_H

#ifndef AFL_CM
  #include "afl-fuzz.h"
#endif

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <time.h>
#include <limits.h>

typedef struct my_mutator {

  #ifndef AFL_CM
    afl_state_t *afl;
  #endif
  
    char *out_buf; // The whole seed in a buffer
  
    uint64_t buf_size; // Full size in bytes of the APK
    uint64_t i; // Offset to fuzz from 
    uint64_t j; // Offset to fuzz to 

} my_mutator_t;

#endif // CUSTOM_MUTATOR_H
