#include <stdio.h>
#include "utils.h"

// Code is taken from SearchGem5 repository: https://github.com/karineek/SearchGEM5/blob/main/src/custom_mutators/utils.c
// Print error message via perror with additional information
void print_error(const char *msg, const char *data) {
    if ((msg == NULL) || (data == NULL)) {
        if (msg != NULL) {
            perror(msg);
        } else if (data != NULL) {
            perror(data);
        } else {
	         perror(">>-17 Unkown error, both msg and data information are misisng");
        }
    } else { // We have both
        char error_message[300];  // Adjust the size as needed
        sprintf(error_message, "%s (%s)", msg, data);
        perror(error_message);
    }
}
