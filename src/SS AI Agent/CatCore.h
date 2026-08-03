#ifndef CAT_CORE_H
#define CAT_CORE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Low-level C utilities for the "Extraordinary Cat" operations.
 * Focused on performance and direct memory manipulation.
 */

typedef enum {
    CAT_SUCCESS = 0,
    CAT_ERROR_FILE = 1,
    CAT_ERROR_MEMORY = 2,
    CAT_ERROR_MARKER = 3
} CatStatus;

/**
 * @brief Checks if a buffer contains a specific SSQLM marker.
 */
int cat_has_marker(const char* buffer, const char* marker);

/**
 * @brief Verifies if a file exists (POSIX style).
 */
int cat_file_exists(const char* filename);

#ifdef __cplusplus
}
#endif

#endif // CAT_CORE_H
