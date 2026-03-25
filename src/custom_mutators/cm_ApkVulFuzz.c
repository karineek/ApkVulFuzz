#include "bitflip.h"
#include "cm_ApkVulFuzz.h"
#include "apk.h"
#include "black_box_fuzz.h"

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

    // --- Write the new data
    FILE *out = fopen(data->fileout_name, "wb");
    if (!out) {
#ifdef TEST_CM
        WARNF(">>-9-A Error: Failed to create output file: %s.", data->fileout_name);
#endif
        AFL_CUSTOM_MUTATOR_FAILED;
    }
    if (fwrite(data->out_buf, 1, data->buf_size, out) != data->buf_size) {
#ifdef TEST_CM
        WARNF(">>-9-B Error: Failed to write output file.");
#endif
        if (out) fclose(out);
        AFL_CUSTOM_MUTATOR_FAILED;
    }
    if (out) fclose(out);
    
    // --- Return filename to AFL ---
    char *new_buf = malloc(out_len + 1);
    if (!new_buf) {
#ifdef TEST_CM
        WARNF(">>-9-C Error: Failed to allocate output register for AFL.");
#endif
        AFL_CUSTOM_MUTATOR_FAILED; // We cannot work with this
    }
    
    // Else continue with the mutations
    memcpy(new_buf, data->fileout_name, out_len + 1);
    *out_buf = (u8 *)new_buf;
    return out_len;
}

// STAB
int main(int argc, char **argv) {
    // Check we have the folders we need to fuzz
    if (argc < 3) {
        fprintf(stderr, ">> (ApkVulFuzz) Usage: %s <in_folder> <out_folder>\n", argv[0]);
        return 1;
    }
    const char *in_folder = argv[1];  // First arg
    const char *out_folder = argv[2]; // Second arg

    // The sub-folders for the fuzzing AFL style
    char queue_folder[1024];
    char hang_folder[1024];
    char crash_folder[1024];
    snprintf(queue_folder, sizeof(queue_folder), "%s/queue", out_folder);
    snprintf(hang_folder, sizeof(hang_folder), "%s/hang", out_folder);
    snprintf(crash_folder, sizeof(crash_folder), "%s/crash", out_folder);

    // Create out_folder, queue_folder, hang_folder and crash_folder if not exists
    if (ensure_dir(out_folder) != 0) return 1;
    if (ensure_dir(queue_folder) != 0) return 1;
    if (ensure_dir(hang_folder) != 0) return 1;
    if (ensure_dir(crash_folder) != 0) return 1;

    printf(">> (ApkVulFuzz) Input folder : %s\n", in_folder);
    printf(">> (ApkVulFuzz) Output folder: %s\n", out_folder);
    printf(">> (ApkVulFuzz) Queue folder : %s\n", queue_folder);
    printf(">> (ApkVulFuzz) Hang folder  : %s\n", hang_folder);
    printf(">> (ApkVulFuzz) Crash folder : %s\n", crash_folder);

    // Set fuzzing target:
    const char *sut = "/users/kevenmen/droidbot/start.py -aa" 
    char path_apk[1024]; // To be used by the fuzzing loop
    const char *sut_args = "-d emulator-5554 -policy bfs_greedy -count 100 -interval 2 -timeout 50 -o results/fdroid_run -is_emulator -grant_perm"
    // Run: per input file, put its name (file name) in path, and then run: sut + path + sut_args
    DIR *dir = opendir(in_folder);
    if (!dir) {
        perror("opendir");
        return 1;
    }
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        /* skip "." and ".." */
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        /* build full input path */
        int n = snprintf(path_apk, sizeof(path_apk), "%s/%s", in_folder, entry->d_name);
        if (n < 0 || n >= (int)sizeof(path_apk)) {
            fprintf(stderr, "Path too long, skipping: %s\n", entry->d_name);
            continue;
        }

        /* only run on regular files */
        if (!is_regular_file(path_apk)) {
            continue;
        }

        printf("Running target on: %s\n", path_apk);

        if (run_target(path_apk) != 0) {
            fprintf(stderr, "Failed to run target on: %s\n", path_apk);
        }
    }
  
  
    // Init data for STAB
    const char *path = "F-Droid.apk";
    uint8_t *buf = (uint8_t *)path;
    size_t file_size = strlen(path);
    u8 *out_buf = NULL;
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

    printf(">> OUT FILE: %s\n", (char *)out_buf);
    
cleanup:
    if (out_buf) free(out_buf);
    if (data) afl_custom_deinit(data);

    return 0;
}
