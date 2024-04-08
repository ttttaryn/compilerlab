#include "utils.h"

FILE* set_input_file(const char *filename) {
    FILE* fp;
    fp = fopen(filename, "r");
    if (!fp) {
        perror(filename);
        exit(1);
    }
    return fp;
}
