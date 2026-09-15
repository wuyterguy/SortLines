char* strchr(const char* s, const char c) {
    while (*s != '\0') {
        if (*s == c)
            return (char*)s;
        s++;
    }
    return NULL;
}

char* strrchr(const char* s, const char c) {
    char* result = NULL;
    while (*s != '\0') {
        if (*s == c)
            result = (char*)s;
        s++;
    }
    return result;
}


size_t strlen(const char* s) {
    const char* start = s;
    while (*s != '\0')
        s++;
    return s - start;
}


int Puts(const char* s) {
    int result = 0;
    while (*s != '\0') {
        if (putchar(*s++) == EOF)
            result = EOF;
    }
    return result;
}


char* strcpy (char* s1, const char* s2) {
    char* result = s1;
    while ((*s1++ = *s2++) != '\0');
    return result;
}

char* strncpy (char* s1, const char* s2, int n) {
    char* result = s1;
    while (((*s1++ = *s2++) != '\0') && (n-- > 0));
    while (n-- > 0)
        *s1++ = '\0';
    return result;
}


char* strcat (char* s1, const char* s2) {
    char* result = s1;
    while (*s1 != '\0')
        s1++;
    while (*s2 != '\0')
        *s1++ = *s2++;
    return result;
}

char* strncat (char* s1, const char* s2, int n) {
    char* result = s1;
    while (*s1 != '\0')
        s1++;
    while ((*s2 != '\0') && (n-- > 0))
        *s1++ = *s2++;
    while (n-- > 0)
        *s1++ = '\0';
    return result;
}


int strcmp(const char* s1, const char* s2){
    while ((*s1 != '\0') || (*s2 != '\0')) {
        if (*s1 != *s2)
            return *s1 - *s2;
        s1++;
        s2++;
    }
    return 0;
}

int strncmp(const char* s1, const char* s2, int n){
    while (((*s1 != '\0') || (*s2 != '\0')) && (n-- > 0)) {
        if (*s1 != *s2)
            return *s1 - *s2;
        s1++;
        s2++;
    }
    return 0;
}
