#ifndef OS_H
#define OS_H

#define MAX_PROCESSES 128
#define TIME_SLICE 10

void os_init(void);
int loader(const char *program_file, const char *data_file);
void scheduler(void);
void shell(void);
int os_should_exit(void);

#endif
