#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <time.h>
#include <limits.h>
#include <stdint.h>

#ifndef _APK_DS_H
#define _APK_DS_H

typedef struct {
    const char *apk_name;
    uint64_t start_offset;
    uint64_t end_offset;
} ApkEntry;
extern ApkEntry apk_table[];
extern const size_t apk_table_size;

const ApkEntry* find_apk(const char *name);
char *build_output_filename(const char *input_path);

#endif // _APK_DS_H
