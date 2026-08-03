#include "CatCore.h"

int cat_has_marker(const char* buffer, const char* marker) {
    if (!buffer || !marker) return 0;
    return strstr(buffer, marker) != NULL;
}

int cat_file_exists(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file) {
        fclose(file);
        return 1;
    }
    return 0;
}
