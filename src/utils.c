#include "header.h"
#include <ctype.h>
#include <string.h>

// Check if the input string contains only digits
int isNumber(const char* str) {
    if (str == NULL || *str == '\0') return 0;
    while (*str) {
        if (!isdigit((unsigned char)*str)) return 0;
        str++;
    }
    return 1;
}

// Validate if the account type is one of the allowed types
int isValidAccountType(const char* type) {
    const char* validTypes[] = {"current", "savings", "fixed01", "fixed02", "fixed03"};
    size_t numTypes = sizeof(validTypes) / sizeof(validTypes[0]);
    for (size_t i = 0; i < numTypes; i++) {
        if (strcmp(type, validTypes[i]) == 0) {
            return 1;
        }
    }
    return 0;
}