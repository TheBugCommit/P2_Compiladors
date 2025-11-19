#ifndef LOG_H
#define LOG_H

void log_init(const char* filename);
void log_close();
void log_production(const char* format, ...);

#endif