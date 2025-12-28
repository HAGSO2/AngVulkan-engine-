#include "utils/Astring.h"

#include <string.h>


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