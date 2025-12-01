#include "./gen_temps.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int *temp_busy = NULL;
int temp_count = 0;
int temps_in_use = 0; 

int *free_stack = NULL;
int free_stack_size = 0;
int free_stack_cap = 0; 

void ensure_temp_capacity(int min_size) {
    if (temp_count >= min_size) return;

    int old = temp_count;
    temp_count = (temp_count == 0) ?  128 : temp_count * 2;
    if (temp_count < min_size) temp_count = min_size;

    temp_busy = realloc(temp_busy, temp_count * sizeof(int));

    for (int i = old; i < temp_count; i++)
        temp_busy[i] = 0;
}

void ensure_free_stack_capacity(int min_size) {
    if (free_stack_cap >= min_size) return;

    int new_cap = (free_stack_cap == 0) ? 16 : free_stack_cap * 2;
    if (new_cap < min_size) new_cap = min_size;

    free_stack = realloc(free_stack, new_cap * sizeof(int));
    free_stack_cap = new_cap;
}

char* gen_new_temp() {
    int idx;

    if (free_stack_size > 0) {
        free_stack_size--;
        idx = free_stack[free_stack_size];
    } else {
        ensure_temp_capacity(temps_in_use + 1);
        idx = temps_in_use;
        temps_in_use++;
    }

    temp_busy[idx] = 1;


    char *buffer = malloc(8);
    sprintf(buffer, "$t%02d", idx + 1);

    return buffer;
}

void gen_release_temp(char *temp_name) {
    if (!temp_name) return;
    if (strncmp(temp_name, "$t", 2) != 0) return;

    int idx = atoi(temp_name + 2) - 1;
    if (idx < 0 || idx >= temps_in_use) return;

    if (temp_busy[idx] == 1) {
        temp_busy[idx] = 0;
        ensure_free_stack_capacity(free_stack_size + 1);
        free_stack[free_stack_size++] = idx;
    }
}

void free_temps() {
    free(temp_busy);
    free(free_stack);

    temp_busy = NULL;
    free_stack = NULL;

    temp_count = 0;
    free_stack_size = 0;
    free_stack_cap = 0;
}