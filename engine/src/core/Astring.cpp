#include "core/Astring.h"

#include <string.h>
#include <stdio.h>
#include <stdarg.h>

u64 string_length(const char* str) {
    return strlen(str);
}

char* string_duplicate(const char* str) {
    //Se encarga de alocar en memoria una string identica a la dada
    // y devuelve un puntero a a esta.
    return strdup(str);
}

// Case-sensitive string comparison. True if the same, otherwise false.
b8 strings_equal(const char* str0, const char* str1) {
    return strcmp(str0, str1) == 0;
}

i32 string_format_v(char* dest, const char* format, va_list arg_ptr) {
    if (dest) {
        char buffer[32000];

        i32 written = vsnprintf(buffer, sizeof(buffer), format, arg_ptr);

        if (written >= 0) {
            buffer[written] = '\0';
            memcpy(dest, buffer, written + 1);
        }

        return written;
    }
    return -1;
}

i32 string_format(char* dest, const char* format, ...) {
    if (dest) {
        __builtin_va_list arg_ptr;
        va_start(arg_ptr, format);
        i32 written = string_format_v(dest, format, arg_ptr);
        va_end(arg_ptr);
        return written;
    }
    return -1;
}