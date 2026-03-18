#include <stdio.h>
#include "bitflip.h"
#include "cm_ApkVulFuzz.h"

bool mutateBinary(uint8_t *new_buf, my_mutator_t *data) {

    if (!new_buf || !data) {
        fprintf(stderr, "Error: null pointer\n");
        return false;
    }

    uint64_t buf_size = data->buf_size;
    uint64_t i = data->i;
    uint64_t j = data->j;

    if (j <= i) {
        fprintf(stderr, "Error: Need 0 <= i < j\n");
        return false;
    }

    if (j > buf_size) {
        fprintf(stderr, "Error: Range end j is past buffer size\n");
        return false;
    }

    //Initialise RNG once
    static int seeded = 0;
    if (!seeded) {
        srand((unsigned int)time(NULL));
        seeded = 1;
    }

    uint64_t off = i + (rand() % (j - i));
    int bit = rand() % 8;

    uint8_t mask = (uint8_t)(1u << bit);

    uint8_t old_byte = new_buf[off];
    uint8_t new_byte = (uint8_t)(old_byte ^ mask);
    new_buf[off] = new_byte;

    #ifdef TEST_CM
    printf("Flipped bit %d at byte offset %llu: 0x%02x -> 0x%02x\n",
           bit,
           (unsigned long long)off,
           old_byte,
           new_byte);
    #endif
  
    return true;   
}
