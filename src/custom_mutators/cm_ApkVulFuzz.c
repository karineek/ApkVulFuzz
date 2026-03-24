#include "bitflip.h"
#include "cm_ApkVulFuzz.h"
#include "apk.h"

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

		// Free the file name
        if (data->fileout_name != NULL) {
	        free(data->fileout_name);
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

    // Check if broken input, commonly AFL will have these numbers odd
    if (!buf || buf_size < 5 || max_size < buf_size) {
#ifdef TEST_CM
	    WARNF(">>-6A Odd size of register is: %zu", buf_size);
#endif
	    AFL_CUSTOM_MUTATOR_FAILED; // We cannot work with this
    }
	
	// Check the buf contains an APK file to begin with!
	char path[512];
	size_t len = (buf_size < sizeof(path) - 1) ? buf_size : sizeof(path) - 1;
	memcpy(path, buf, len);
	path[len] = '\0';
	size_t plen = strlen(path);
	if (plen < 4 || strcmp(path + plen - 4, ".apk") != 0) {
	#ifdef TEST_CM
	    WARNF(">> Invalid file name: %s", path);
	#endif
	    AFL_CUSTOM_MUTATOR_FAILED;
	}

	// --- Load APK + offsets into mutator ---
    if (load_apk_into_mutator(data, path) != 0) {
#ifdef TEST_CM
		WARNF(">>-7A Error: Failed to load APK into mutator: %s", path); // Original apk file
#endif
        AFL_CUSTOM_MUTATOR_FAILED; // We cannot work with this
    }

	// --- Check APK + offsets into mutator and name are OKAY ---
	if ((!data->out_buf) || (!data->fileout_name)) {
#ifdef TEST_CM
		WARNF(">>-7B Error: no data from load_apk_into_mutator"); // New name of APK file
#endif
        AFL_CUSTOM_MUTATOR_FAILED; // We cannot work with this
    }
	size_t out_len = strlen(data->fileout_name);
	if (out_len + 1 > max_size) {
#ifdef TEST_CM
        WARNF(">>-7-C Bad generation for buffer with mutations.");
#endif
		AFL_CUSTOM_MUTATOR_FAILED;
	}
	
    // -- Mutate the APK
    bool mutations_rc = mutateBinary(data->out_buf, data);
    // Check if no buff returned
    if ((!data->out_buf) || (!mutations_rc)) {
#ifdef TEST_CM
        WARNF(">>-8-A Bad generation for buffer with mutations.");
#endif
		AFL_CUSTOM_MUTATOR_FAILED;
	}
	
	// --- Return filename to AFL ---
	char *new_buf = malloc(out_len + 1);
	if (!new_buf)
    	AFL_CUSTOM_MUTATOR_FAILED; // We cannot work with this

	// Else continue with the mutations
	memcpy(new_buf, data->fileout_name, out_len + 1);
	*out_buf = (u8 *)new_buf;
	return out_len;
}

// STAB
int main() {
	// Init data for STAB
    const char *path = "F-Droid.apk";
    uint8_t *buf = (uint8_t *)path;
    size_t file_size = strlen(path);
	u8 *out_buf = NULL;
    FILE *out = NULL;
    size_t max_size = 512;

    my_mutator_t *data = afl_custom_init(NULL, 0);
    if (!data) {
        fprintf(stderr, "Error: afl_custom_init failed\n");
        return 1;
    }

	// FUZZ
    size_t new_size = afl_custom_fuzz(
        data,
        buf,
        file_size,
        &out_buf,
        NULL,
        0,
        max_size
    );

	// Write results if all OK (first check)
    if (!out_buf || new_size == 0) {
        fprintf(stderr, "Error: afl_custom_fuzz failed\n");
        goto cleanup;
    }

    // safe null termination
    if (new_size >= max_size)
        new_size = max_size - 1;
    ((char *)out_buf)[new_size] = '\0';

    out = fopen((char *)out_buf, "wb");
    if (!out) {
        fprintf(stderr, "Error: Failed to create output file: %s\n",
                (char *)out_buf);
        goto cleanup;
    }

    if (fwrite(data->out_buf, 1, data->buf_size, out) != data->buf_size) {
        fprintf(stderr, "Error: Failed to write output file\n");
        goto cleanup;
    }

	printf(">> OUT FILE: %s\n", (char *)out_buf);
	
cleanup:
    if (out) fclose(out);
    if (out_buf) free(out_buf);
    if (data) afl_custom_deinit(data);

    return 0;
}
