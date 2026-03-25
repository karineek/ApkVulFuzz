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
