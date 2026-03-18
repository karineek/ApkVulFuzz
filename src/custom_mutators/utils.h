#ifndef UTILS_H
#define UTILS_H

// Print error message to screen
void print_error(const char *msg, const char *data);

// Count the number of lines in a buffer to check no bad mutations are written
int countLines(const char *str, int _maxsize);

#endif // UTILS_H
