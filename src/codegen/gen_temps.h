#ifndef GEN_TEMPS_H
#define GEN_TEMPS_H

void ensure_temp_capacity(int min_size);
void ensure_free_stack_capacity(int min_size);

char* gen_new_temp();
int is_temporary(const char* addr);
void gen_release_temp(char *temp_name);
void free_temps();

#endif