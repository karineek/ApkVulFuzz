#include "cm_ApkVulFuzz.h"
#include "black_box_fuzz.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <dirent.h>
#include <sys/wait.h>
#include <unistd.h>

// This function was written with ChatGPT, 
// nothing interesting here, just creating 
// folders if not already exist (Date: 25/03/2026)
static int ensure_dir(const char *path) {
    struct stat st;
    if (stat(path, &st) == 0) {
        if (S_ISDIR(st.st_mode)) {
            return 0; // already exists
        }
        fprintf(stderr, "Path exists but is not a directory: %s\n", path);
        return -1;
    }
    if (mkdir(path, 0755) != 0) {
        if (errno == EEXIST) {
            return 0;
        }
        perror(path);
        return -1;
    }
    return 0;
}

// This function was written with ChatGPT, 
// nothing interesting here, just checking 
// folders still exist (Date: 25/03/2026)
static int is_regular_file(const char *path) {
    struct stat st;
    if (stat(path, &st) != 0) {
        return 0;
    }
    return S_ISREG(st.st_mode);
}

// This function was written with ChatGPT, 
// nothing interesting here, just executing  
// cmd style with args (Date: 25/03/2026)
static int run_target(const char *input_path) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return -1;
    }

    if (pid == 0) {
        /*
         * Safer than system():
         * python3 /users/kevenmen/droidbot/start.py -aa <input_path> ...
         *
         * If start.py is executable with a shebang, you could also exec it directly.
         */
        char *const argv[] = {
            "python3",
            "/users/kevenmen/droidbot/start.py",
            "-aa",
            (char *)input_path,
            "-d", "emulator-5554",
            "-policy", "bfs_greedy",
            "-count", "100",
            "-interval", "2",
            "-timeout", "50",
            "-o", "results/fdroid_run",
            "-is_emulator",
            "-grant_perm",
            NULL
        };

        execvp("python3", argv);

        perror("execvp");
        _exit(127);
    }

    int status = 0;
    if (waitpid(pid, &status, 0) < 0) {
        perror("waitpid");
        return -1;
    }

    if (WIFEXITED(status)) {
        printf("Finished: %s (exit=%d)\n", input_path, WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        printf("Crashed: %s (signal=%d)\n", input_path, WTERMSIG(status));
    } else {
        printf("Ended alearly: %s\n", input_path);
    }

    return 0;
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
