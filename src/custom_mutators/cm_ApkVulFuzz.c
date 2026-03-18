#include "bitflip.h"
#include "cm_ApkVulFuzz.h"

////////////////////////////////
// Start adding AFL functions //
////////////////////////////////
/**
 * Initialise this custom mutator
 *
 * @param[in] afl a pointer to the internal state object. Can be ignored for
 * now.
 * @param[in] seed A seed for this mutator - the same seed should always mutate
 * in the same way.
 * @return Pointer to the data object this custom mutator instance should use.
 *         There may be multiple instances of this mutator in one afl-fuzz run!
 *         Return NULL on error.
 */
my_mutator_t *afl_custom_init(afl_state_t *afl, unsigned int seed) {

  srand(seed);  // needed also by surgical_havoc_mutate()

  my_mutator_t *data = calloc(1, sizeof(my_mutator_t));
  if (!data) {
    perror(">> (afl_custom_init) afl_custom_init alloc");
    return NULL;
  }
  
  data->buf_size = 0;
  data->i=0;
  data->j=0;

  #ifdef AFL_CM
  data->afl = afl;
  #endif
  
  return data;
}

/**
 * Deinitialize everything
 *
 * @param data The data ptr from afl_custom_init
 */
void afl_custom_deinit(my_mutator_t *data) {
    // Check if data is not nullptr
    if (!data) {
        return; // nothing to do here
    } else {
        // Free out buffer if it is not nullptr
        if (data->out_buf != NULL) {
        free(data->out_buf);
    }
    
    // Reset afl variable if needed
    #ifdef AFL_CM
    data->afl = 0;
    #endif
    
    // Free the main structure (data)
    free(data);
    }
}

/**
 * Perform custom mutations on a given input
 *
 * (Optional for now. Required in the future)
 *
 * @param[in] data pointer returned in afl_custom_init for this fuzz case
 * @param[in] buf Pointer to input data to be mutated
 * @param[in] buf_size Size of input data
 * @param[out] out_buf the buffer we will work on. we can reuse *buf. NULL on
 * error.
 * @param[in] add_buf Buffer containing the additional test case
 * @param[in] add_buf_size Size of the additional test case
 * @param[in] max_size Maximum size of the mutated output. The mutation must not
 *     produce data larger than max_size.
 * @return Size of the mutated output.
 */
size_t afl_custom_fuzz(my_mutator_t *data, uint8_t *buf, size_t buf_size,
                       u8 **out_buf, uint8_t *add_buf,
                       size_t add_buf_size,  // add_buf can be NULL
                       size_t max_size) {

    // Check if broken input
    size_t actual_in_buf_size = strlen((const char *) buf);
    if ((buf_size < 11) || (actual_in_buf_size < 11)) {
	      WARNF(">>-6A Odd size of register is: %zu, %zu", actual_in_buf_size, buf_size);
        AFL_CUSTOM_MUTATOR_FAILED; // We cannot work with this
    }

    // Allocate a new buffer for the edits
    size_t new_size = actual_in_buf_size; // Size of APK should stay the same.
    uint8_t *new_buf = malloc(new_size);
    if (!new_buf) {
        WARNF(">>-7A Bad allocation for buffer for mutations. Could not allocate %zu size buffer.", new_size);
        AFL_CUSTOM_MUTATOR_FAILED; // We cannot work with this
    }
    // Copy the original input data
    memcpy(new_buf, buf, buf_size);

    if (!data) {
	      WARNF(">>-7B Bad allocation for data strucuture coming from AFL++");
        AFL_CUSTOM_MUTATOR_FAILED; // We cannot work with this
    }

    // KEM: here we can define 3 mutators: combine one, args mutator and binary mutator.
    bool mutations_rc = 0; // RC if mutations succ.
    mutations_rc = mutateBinary(new_buf, data);

    // Check if no buff returned
    if (!new_buf) {
#ifdef TEST_CM
        WARNF(">>-8-A Bad generation for buffer with mutations.");
#endif
        AFL_CUSTOM_MUTATOR_FAILED; // We cannot work with this
    }

    // Check if mutation succ. and check new_buf before declaring the mutation is okay
    if ((mutations_rc==0) || (countLines((const char *) new_buf,new_size) < 2)) {
#ifdef TEST_CM
        WARNF(">>-8-B Bad generation for buffer with mutations. Memory corrupted.");
#endif
        free(new_buf);
	      AFL_CUSTOM_MUTATOR_FAILED; // We cannot work with this
    } // Else continue with the mutations

    // Shrink the buffer till \0
    size_t actual_size = strlen(data->out_buf) + 1; // Add 1 for the null-terminator
    uint8_t *new_new_buf = malloc(actual_size);
    if ((!new_new_buf)) {
        WARNF(">>-9 Bad re-allocation for buffer for mutations. Could not allocate %zu size buffer.", actual_size);
        AFL_CUSTOM_MUTATOR_FAILED; // We cannot work with this
    }
    memcpy(new_new_buf, new_buf, actual_size);
    // Clear the old data
    free(new_buf);

    // Set it as output buff
    *out_buf = new_new_buf;

    // Return mutated
    return actual_size;
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
    my_mutator_t *data = afl_custom_init(NULL, 0);
    if (!data) {
        fprintf(stderr, "Error: afl_custom_init failed\n");
        free(buf);
        return 1;
    }
    
    data->buf_size = size;
    data->i = i;
    data->j = j;
    
    //mutateBinary(buf, data);
    // AFL-style mutation
    u8 *out_buf = NULL;

    size_t max_size = file_size + 4096;   // allow growth (important for fuzzing)

    size_t new_size = afl_custom_fuzz(
        data,
        buf,
        file_size,
        &out_buf,
        NULL,
        0,
        max_size
    );

    if (!out_buf || new_size == 0) {
        fprintf(stderr, "Error: afl_custom_fuzz failed\n");
        afl_custom_deinit(data);
        free(buf);
        return 1;
    }

	
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

    // cleanup
    afl_custom_deinit(data);
    fclose(out);
    free(buf);
  
    return 0;
}
