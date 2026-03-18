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

  char* out_buff; // The whole args in buffers

  char* file_name_types; // The name of the file with types to mutate

  char* input_args; // Keeps the input arguments for mutating

  char* input_digit; // Buffer for register mutations

} my_mutator_t;


#endif // CUSTOM_MUTATOR_H
