#include <stdlib.h>

#ifndef _BLACK_BOX_FUZZ_H
#define _BLACK_BOX_FUZZ_H

static int ensure_dir(const char *path);
static int is_regular_file(const char *path);
static int run_target(const char *input_path);

#endif // _BLACK_BOX_FUZZ_H
