// bitflip.h header
#include <stdint.h>
#include <stdbool.h>
#include "cm_ApkVulFuzz.h"

#ifndef BITFLIP_H
#define BITFLIP_H

// Mutation function
bool mutateBinary(uint8_t *new_buf, my_mutator_t *data);

#endif // BITFLIP_H
