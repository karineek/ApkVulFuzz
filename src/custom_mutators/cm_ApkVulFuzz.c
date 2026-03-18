// TODO

#include "bitflip.h"
#include "cm_ApkVulFuzz.h"

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
