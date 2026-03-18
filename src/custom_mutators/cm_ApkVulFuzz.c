// TODO

#include "bitflip.h"
#include "cm_ApkVulFuzz.h"

// Start adding AFL functions
my_mutator_t *afl_custom_init(afl_state_t *afl, unsigned int seed) {

  srand(seed);  // needed also by surgical_havoc_mutate()

  my_mutator_t *data = calloc(1, sizeof(my_mutator_t));
  if (!data) {
    perror(">>-1 afl_custom_init alloc");
    return NULL;
  }

  if ((data->out_buff = (char *)malloc(MAX_CMDLINE_SIZE)) == NULL) {
    perror(">>-2 afl_custom_init malloc");
    return NULL;
  }

  if ((data->file_name_types = (char *)malloc(MAX_FILE_NAME_SIZE)) == NULL) {
    perror(">>-3 afl_custom_init malloc");
    return NULL;
  }

  if ((data->input_args = (char *)malloc(MAX_ARGS_SIZE)) == NULL) {
    perror(">>-4 afl_custom_init malloc");
    return NULL;
  }

  if ((data->input_digit = (char *)malloc(MAX_DATA_SIZE)) == NULL) {
    perror(">>-5 afl_custom_init malloc");
    return NULL;
  }

  data->afl = afl;

  return data;
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
    my_mutator_t data;
    
    data.buf_size = size;
    data.i = i;
    data.j = j;
    
    mutateBinary(buf, &data);

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
