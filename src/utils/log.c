#include "log.h"
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

static FILE *log_file = NULL;

void log_init(const char* filename) {

    if (filename) {
        log_file = fopen(filename, "w");
    }
    if (!log_file) {
        log_file = stdout;
    }

    fprintf(log_file, "--- START OF ANALYSIS ---\n");   
}

void log_close() {
    if (log_file != NULL) {
        fprintf(log_file, "--- END OF ANALYSIS ---\n");
        fclose(log_file);
        log_file = NULL;
    }
}

void log_production(const char *format, ...) {
    char full_message[512];
    
    va_list args;
    va_start(args, format);
    vsnprintf(full_message, sizeof(full_message), format, args);
    va_end(args);
    
    if (log_file) {
        fprintf(log_file, "[PROD] %s\n", full_message);
        fflush(log_file);
    } else {
        printf("[PROD] %s\n", full_message);
        fflush(stdout);
    }
}