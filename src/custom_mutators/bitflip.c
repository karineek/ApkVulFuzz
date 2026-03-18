#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//bool mutateBinary(uint8_t *new_buf, my_mutator_t *data) {

bool mutateBinary(uint8_t* new_buf,
                          uint64_t buf_size,
                          uint64_t i,
                          uint64_t j) {
    if (!new_buf) {
        fprintf(stderr, "Error: new_buf is null\n");
        return false;
    }

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

    printf("Flipped bit %d at byte offset %llu: 0x%02x -> 0x%02x\n",
           bit,
           (unsigned long long)off,
           old_byte,
           new_byte);
  
    return true;   
}


// STAB - remove later
int main() {
    const char *path = "F-Droid.apk";
    uint64_t i = 7401341;
    uint64_t j = i + 8921;

    FILE *in = fopen(path, "rb");
    if (!in) {
        fprintf(stderr, "Error: Failed to open file: %s\n", path);
        return 1;
    }

    // Get file size
    if (fseek(in, 0, SEEK_END) != 0) {
        fprintf(stderr, "Error: fseek failed\n");
        fclose(in);
        return 1;
    }

    long size = ftell(in);
    if (size < 0) {
        fprintf(stderr, "Error: ftell failed\n");
        fclose(in);
        return 1;
    }

    rewind(in);

    // Allocate buffer
    uint8_t *buf = (uint8_t *)malloc(size);
    if (!buf) {
        fprintf(stderr, "Error: malloc failed\n");
        fclose(in);
        return 1;
    }

    // Read file
    if (fread(buf, 1, size, in) != (size_t)size) {
        fprintf(stderr, "Error: Failed to read file\n");
        free(buf);
        fclose(in);
        return 1;
    }

    fclose(in);

    // Mutate
    flip_random_bit_in_range(buf, size, i, j);

    // Write output
    FILE *out = fopen("mutated.apk", "wb");
    if (!out) {
        fprintf(stderr, "Error: Failed to create output file\n");
        free(buf);
        return 1;
    }

    if (fwrite(buf, 1, size, out) != (size_t)size) {
        fprintf(stderr, "Error: Failed to write output file\n");
        fclose(out);
        free(buf);
        return 1;
    }

    fclose(out);
    free(buf);

    return 0;
}
