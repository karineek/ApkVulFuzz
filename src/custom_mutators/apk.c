#include "apk.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/////// A table for APK and offsets of AndroidManifest.xml
ApkEntry apk_table[] = {
    { "F-Droid.apk", 				7401341, 7401341 + 8921 },
    { "flashlight.apk", 			9226856, 9226856 + 7345 },
	{ "happymod.apk", 				9226856, 9226856 + 7345 },
    { "weather-shalltry-group.apk", 9226856, 9226856 + 7345 }
};

const size_t apk_table_size = sizeof(apk_table) / sizeof(apk_table[0]);

const ApkEntry* find_apk(const char *name) {
    for (size_t i = 0; i < apk_table_size; i++) {
        if (strstr(name, apk_table[i].apk_name) != NULL) {
            return &apk_table[i];
        }
    }
    return NULL;
}

// We populate the APK data based on what we have:
int load_apk_into_mutator(my_mutator_t *data, const char *path) {
	// OFFSETS: We first populate the offset to fuzz
	const ApkEntry *entry = find_apk(path);
	if (!entry) {
	    fprintf(stderr, "Error: APK not found in table: %s\n", path);
	    return -1;
	}
	data->i = entry->start_offset;
    data->j = entry->end_offset;

	// BINARY DATA: We then try to populate the data itself 
    FILE *in = fopen(path, "rb");
    if (!in) {
        fprintf(stderr, "Error: Failed to open file: %s\n", path);
        return -1;
    }
    if (fseek(in, 0, SEEK_END) != 0) {
        fprintf(stderr, "Error: fseek failed\n");
        fclose(in);
        return -1;
    }
    long size = ftell(in);
    if (size < 0) {
        fprintf(stderr, "Error: ftell failed\n");
        fclose(in);
        return -1;
    }
    rewind(in);
    uint8_t *buf = malloc(size);
    if (!buf) {
        fprintf(stderr, "Error: malloc failed\n");
        fclose(in);
        return -1;
    }
    if (fread(buf, 1, size, in) != (size_t)size) {
        fprintf(stderr, "Error: Failed to read file\n");
        free(buf);
        fclose(in);
        return -1;
    }
    fclose(in);

	// We now populate also the binary content to mutate
    data->buf = buf;
    data->buf_size = (size_t)size;

    // Last, we give a name to our output APK fuzzed file
	
    return 0;
}

// This is a pretty standard function - I wrote it with ChatGPT 24-March-2026
// It creates a new temp name from the input_path name, no big logic, tones of pointers!
char *build_output_filename(const char *input_path) {
    // Find last '/' (directory separator)
    const char *slash = strrchr(input_path, '/');

    const char *dir = "";
    const char *base = input_path;

    if (slash) {
        size_t dir_len = slash - input_path + 1;

        char *dir_buf = malloc(dir_len + 1);
        if (!dir_buf) return NULL;

        strncpy(dir_buf, input_path, dir_len);
        dir_buf[dir_len] = '\0';

        dir = dir_buf;
        base = slash + 1;
    }

    // Copy basename so we can modify it
    char name[512];
    strncpy(name, base, sizeof(name));
    name[sizeof(name) - 1] = '\0';

    // Remove ".apk"
    char *dot = strrchr(name, '.');
    if (dot && strcmp(dot, ".apk") == 0) {
        *dot = '\0';
    }

    // Timestamp
    time_t now = time(NULL);

    // Allocate final string
    size_t out_size = strlen(dir) + strlen(name) + 32;
    char *out = malloc(out_size);
    if (!out) {
        if (slash) free((void *)dir);
        return NULL;
    }

    snprintf(out, out_size, "%s%s_%ld.apk", dir, name, (long)now);

    if (slash) free((void *)dir);

    return out;
}
