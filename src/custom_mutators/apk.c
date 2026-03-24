#include "apk.h"

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
