#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>
#include <time.h>
#include <stdarg.h>
#include "os.h"
#include "compiler.h"
#include "memory.h"
#include "processor.h"

typedef enum { UNUSED=0, READY=1, RUNNING=2, WAITING=3 } State;

typedef struct {
    int pid;
    int proc_id;
    State state;
    char program_file[512];
    char data_file[512];
    char generated_program[512];
    char generated_map[512];
    char frequency_file[512];
} Task;

static Task tasks[MAX_PROCESSES];
static int next_pid = 1;
static int proc_pid[NP];
static int shell_exit = 0;
static char shell_line[1024];
static int shell_len = 0;
static int prompt_shown = 0;
static int demo_mode = 0;
static unsigned long scheduler_round = 0;

static void log_event(const char *fmt, ...)
{
    if (!fd_log) return;
    va_list ap;
    va_start(ap, fmt);
    vfprintf(fd_log, fmt, ap);
    va_end(ap);
    fflush(fd_log);
}

static void msleep(unsigned usec)
{
    struct timespec ts;
    ts.tv_sec = usec / 1000000U;
    ts.tv_nsec = (long)(usec % 1000000U) * 1000L;
    nanosleep(&ts, NULL);
}

static int find_free_proc(void)
{
    for (int i = 0; i < NP; ++i)
        if (proc_pid[i] == 0) return i;
    return -1;
}

static Task *find_task(int pid)
{
    for (int i = 0; i < MAX_PROCESSES; ++i)
        if (tasks[i].state != UNUSED && tasks[i].pid == pid) return &tasks[i];
    return NULL;
}

static int file_readable(const char *path)
{
    FILE *fp = fopen(path, "r");
    if (!fp) return 0;
    fclose(fp);
    return 1;
}

static int copy_file(const char *src, const char *dst)
{
    FILE *a = fopen(src, "rb");
    FILE *b;
    char buf[4096];
    size_t n;

    if (!a) return 0;
    b = fopen(dst, "wb");
    if (!b) { fclose(a); return 0; }

    while ((n = fread(buf, 1, sizeof(buf), a)) > 0) {
        if (fwrite(buf, 1, n, b) != n) {
            fclose(a); fclose(b); return 0;
        }
    }
    fclose(a);
    fclose(b);
    return 1;
}

static void sibling_path(const char *source, const char *name, char *out, size_t n)
{
    const char *slash = strrchr(source, '/');
    if (slash) {
        size_t k = (size_t)(slash - source);
        if (k + 1 + strlen(name) + 1 <= n) {
            memcpy(out, source, k);
            out[k] = '/';
            strcpy(out + k + 1, name);
            return;
        }
    }
    snprintf(out, n, "%s", name);
}

static void make_frequency_name(const char *program_file, int pid, char *out, size_t n)
{
    const char *slash = strrchr(program_file, '/');
    const char *name = slash ? slash + 1 : program_file;
    char base[480];
    const char *dot;
    snprintf(base, sizeof(base), "%.*s", (int)sizeof(base) - 1, name);
    dot = strrchr(base, '.');
    if (dot)
        base[dot - base] = '\0';
    snprintf(out, n, "%s_%d.freq", base, pid);
}

void os_init(void)
{
    memset(tasks, 0, sizeof(tasks));
    memset(proc_pid, 0, sizeof(proc_pid));
    next_pid = 1;
    shell_exit = 0;
    shell_len = 0;
    prompt_shown = 0;
    scheduler_round = 0;
    demo_mode = (getenv("OS_DEMO") != NULL);

    
    memory_system_init();

    fd_log = fopen("process.log", "w");
    if (!fd_log) {
        fprintf(stderr, "OS: warning: cannot open process.log\n");
    } else {
        log_event("============================================================\n");
        log_event("                 CS527 LAB 5 PROCESS LOG\n");
        log_event("============================================================\n");
        log_event("[OS] NP=%d, TIME_SLICE=%d instructions\n\n", NP, TIME_SLICE);
    }
}

int loader(const char *program_file, const char *data_file)
{
    int p = find_free_proc();
    int slot = -1;
    char generated[512] = {0};
    char generated_map[512] = {0};
    char program_byte[512];
    char map_file[512] = {0};
    char original_program[512];
    const char *dot = strrchr(program_file, '.');
    int is_source = dot && strcmp(dot, ".txt") == 0;

    snprintf(original_program, sizeof(original_program), "%s", program_file);

    if (!program_file || !data_file || !*program_file || !*data_file) {
        fprintf(stderr, "OS: program and data filenames are required\n");
        return -1;
    }
    if (!file_readable(program_file)) {
        fprintf(stderr, "[Loader] Cannot open program: %s\n", program_file);
        return -1;
    }
    if (!file_readable(data_file)) {
        fprintf(stderr, "[Loader] Cannot open data: %s\n", data_file);
        return -1;
    }
    if (next_pid > MAX_PROCESSES) {
        fprintf(stderr, "OS: process table full\n");
        return -1;
    }

    for (int i = 0; i < MAX_PROCESSES; ++i) {
        if (tasks[i].state == UNUSED) { slot = i; break; }
    }
    if (slot < 0) return -1;

    if (is_source) {
        sibling_path(program_file, "program.byte", program_byte, sizeof(program_byte));
        compile(program_file, program_byte);
        if (snprintf(generated, sizeof(generated), "%s.pid%d", program_byte, next_pid) >= (int)sizeof(generated) ||
            snprintf(map_file, sizeof(map_file), "%s.map", program_byte) >= (int)sizeof(map_file) ||
            snprintf(generated_map, sizeof(generated_map), "%s.map", generated) >= (int)sizeof(generated_map)) {
            fprintf(stderr, "OS: generated path is too long\n");
            return -1;
        }
        if (!copy_file(program_byte, generated) || !copy_file(map_file, generated_map)) {
            fprintf(stderr, "OS: cannot preserve compiled task files\n");
            return -1;
        }
        program_file = generated;
    }

    tasks[slot].pid = next_pid++;
    tasks[slot].proc_id = -1;
    tasks[slot].state = (p >= 0) ? RUNNING : WAITING;
    snprintf(tasks[slot].program_file, sizeof(tasks[slot].program_file), "%s", program_file);
    snprintf(tasks[slot].data_file, sizeof(tasks[slot].data_file), "%s", data_file);
    snprintf(tasks[slot].generated_program, sizeof(tasks[slot].generated_program), "%s", generated);
    snprintf(tasks[slot].generated_map, sizeof(tasks[slot].generated_map), "%s", generated_map);
    make_frequency_name(original_program, tasks[slot].pid, tasks[slot].frequency_file, sizeof(tasks[slot].frequency_file));

    if (p >= 0) {
        






        if (initialize(p, program_file, data_file) == 0) {
            proc_pid[p] = tasks[slot].pid;
            tasks[slot].proc_id = p;
            reset(p);
            if (generated_map[0])
                load_instruction_map(p, generated_map);
            else {
                snprintf(map_file, sizeof(map_file), "%s.map", program_file);
                load_instruction_map(p, map_file);
            }
            set_processor_pid(p, tasks[slot].pid);
            printf("[OS] Loaded PID %d on processor %d\n", tasks[slot].pid, p);
            log_event("[LOADER] PID %d assigned to Processor %d\n", tasks[slot].pid, p);
            log_event("[STATE] PID %d: NEW -> RUNNING (Processor %d)\n", tasks[slot].pid, p);
            if (demo_mode) printf("[DEMO] PID %d -> RUNNING on processor %d\n", tasks[slot].pid, p);
        } else {
            tasks[slot].state = WAITING;
            tasks[slot].proc_id = -1;
            log_event("[STATE] PID %d: NEW -> WAITING (physical memory unavailable)\n", tasks[slot].pid);
            if (demo_mode)
                printf("[DEMO] PID %d -> WAITING queue (waiting for physical frames)\n",
                       tasks[slot].pid);
            else
                printf("[OS] PID %d placed in WAITING queue (physical memory unavailable)\n",
                       tasks[slot].pid);
        }
    } else {
        printf("[OS] All %d processors busy; PID %d placed in WAITING queue\n", NP, tasks[slot].pid);
        log_event("[STATE] PID %d: NEW -> WAITING (all processors busy)\n", tasks[slot].pid);
        if (demo_mode) printf("[DEMO] PID %d -> WAITING queue\n", tasks[slot].pid);
    }
    fflush(stdout);
    return tasks[slot].pid;
}

static void promote_waiting(void)
{
    for (int p = 0; p < NP; ++p) {
        if (proc_pid[p] != 0) continue;
        for (int i = 0; i < MAX_PROCESSES; ++i) {
            if (tasks[i].state != WAITING) continue;
            if (initialize(p, tasks[i].program_file, tasks[i].data_file) != 0)
                continue; 

            proc_pid[p] = tasks[i].pid;
            tasks[i].proc_id = p;
            tasks[i].state = READY;
            log_event("[STATE] PID %d: WAITING -> READY\n", tasks[i].pid);
            log_event("[QUEUE] PID %d removed from WAITING queue and dispatched to Processor %d\n", tasks[i].pid, p);
            if (demo_mode) {
                printf("[DEMO] WAITING -> READY: PID %d\n", tasks[i].pid);
                printf("[DEMO] READY -> RUNNING: PID %d on processor %d\n", tasks[i].pid, p);
            }
            tasks[i].state = RUNNING;
            log_event("[STATE] PID %d: READY -> RUNNING (Processor %d)\n", tasks[i].pid, p);
            reset(p);
            if (tasks[i].generated_map[0])
                load_instruction_map(p, tasks[i].generated_map);
            else {
                char waiting_map[1024];
                snprintf(waiting_map, sizeof(waiting_map), "%s.map", tasks[i].program_file);
                load_instruction_map(p, waiting_map);
            }
            set_processor_pid(p, tasks[i].pid);
            printf("[OS] Dispatched PID %d from WAITING queue to processor %d\n", tasks[i].pid, p);
            fflush(stdout);
            break;
        }
    }
}

static void reap_finished(void)
{
    for (int p = 0; p < NP; ++p) {
        if (!proc_pid[p] || !end_of_simulation[p]) continue;

        Task *t = find_task(proc_pid[p]);
        if (!t) { proc_pid[p] = 0; continue; }

        write_execution_frequency(p, t->frequency_file);
        finalize(p, t->data_file);
        log_event("[FINISH] PID %d completed on Processor %d\n", t->pid, p);
        log_event("[STATE] PID %d: RUNNING -> FINISHED\n", t->pid);
        log_event("[MEMORY] PID %d released its physical-memory frames\n", t->pid);
        printf("[OS] PID %d finished on processor %d\n", t->pid, p);
        if (demo_mode) printf("[DEMO] PID %d -> FINISHED; processor %d becomes FREE\n", t->pid, p);
        fflush(stdout);

        if (t->generated_program[0]) unlink(t->generated_program);
        if (t->generated_map[0]) unlink(t->generated_map);
        t->state = UNUSED;
        t->proc_id = -1;
        proc_pid[p] = 0;
    }
}

static const char *state_name(State s)
{
    switch (s) {
        case READY: return "READY";
        case RUNNING: return "RUNNING";
        case WAITING: return "WAITING";
        default: return "UNUSED";
    }
}


static void log_queue_status(const char *phase)
{
    if (!fd_log) return;
    log_event("[QUEUE] %s\n", phase);
    log_event("        READY : ");
    int any = 0;
    for (int i = 0; i < MAX_PROCESSES; ++i) {
        if (tasks[i].state == READY) {
            log_event("PID%d(P%d) ", tasks[i].pid, tasks[i].proc_id);
            any = 1;
        }
    }
    if (!any) log_event("EMPTY");
    log_event("\n        WAITING: ");
    any = 0;
    for (int i = 0; i < MAX_PROCESSES; ++i) {
        if (tasks[i].state == WAITING) {
            log_event("PID%d ", tasks[i].pid);
            any = 1;
        }
    }
    if (!any) log_event("EMPTY");
    log_event("\n");
    log_event("        CPU    : ");
    for (int p = 0; p < NP; ++p) {
        if (proc_pid[p]) log_event("P%d=PID%d(%s) ", p, proc_pid[p],
                                   state_name(find_task(proc_pid[p])->state));
        else log_event("P%d=IDLE ", p);
    }
    log_event("\n");
}

static void print_scheduler_status(const char *phase)
{
    if (!demo_mode) return;

    printf("\n[DEMO] Scheduler round %lu -- %s\n", scheduler_round, phase);
    printf("[DEMO] Processors: ");
    for (int p = 0; p < NP; ++p) {
        if (proc_pid[p]) {
            Task *t = find_task(proc_pid[p]);
            printf("P%d=PID%d(%s)%s", p, proc_pid[p],
                   t ? state_name(t->state) : "?", p == NP - 1 ? "" : " | ");
        } else {
            printf("P%d=IDLE%s", p, p == NP - 1 ? "" : " | ");
        }
    }
    printf("\n[DEMO] READY queue: ");
    int any_ready = 0;
    for (int i = 0; i < MAX_PROCESSES; ++i) {
        if (tasks[i].state == READY) {
            printf("PID%d(proc%d) ", tasks[i].pid, tasks[i].proc_id);
            any_ready = 1;
        }
    }
    if (!any_ready) printf("empty");
    printf("\n[DEMO] WAITING queue: ");
    int any_waiting = 0;
    for (int i = 0; i < MAX_PROCESSES; ++i) {
        if (tasks[i].state == WAITING) {
            printf("PID%d ", tasks[i].pid);
            any_waiting = 1;
        }
    }
    if (!any_waiting) printf("empty");
    printf("\n");
    fflush(stdout);
}

static void print_time_slice(int p, int pid, int instructions)
{
    if (!demo_mode) return;
    printf("[DEMO] Processor %d: PID %d executes %d instructions (time slice)\n",
           p, pid, instructions);
    fflush(stdout);
}

static void print_prompt(void)
{
    if (!prompt_shown && !shell_exit) {
        printf("$ ");
        fflush(stdout);
        prompt_shown = 1;
    }
}


void shell(void)
{
    fd_set set;
    struct timeval tv = {0, 0};
    char ch;
    int saw_eof = 0;

    if (shell_exit) return;
    print_prompt();

    FD_ZERO(&set);
    FD_SET(STDIN_FILENO, &set);
    if (select(STDIN_FILENO + 1, &set, NULL, NULL, &tv) <= 0) return;

    while (read(STDIN_FILENO, &ch, 1) == 1) {
        if (ch == '\r') continue;

        if (ch == '\n') {
            char local[1024];
            char *save = NULL;
            char *a, *b, *extra;
            int pid;

            shell_line[shell_len] = '\0';
            snprintf(local, sizeof(local), "%s", shell_line);
            shell_len = 0;

            char *s = local;
            while (*s == ' ' || *s == '\t') ++s;
            if (*s == '$') {
                ++s;
                while (*s == ' ' || *s == '\t') ++s;
            }

            if (*s == '\0') {
                prompt_shown = 0;
                print_prompt();
                return;
            }

            if (strcmp(s, "exit") == 0) {
                shell_exit = 1;
                prompt_shown = 0;
                log_event("[SHELL] exit received; no new tasks will be accepted.\n");
                printf("[OS] Shell exiting; existing tasks will continue.\n");
                fflush(stdout);
                return;
            }

            a = strtok_r(s, " \t", &save);
            b = strtok_r(NULL, " \t", &save);
            extra = strtok_r(NULL, " \t", &save);

            if (!a || !b || extra) {
                fprintf(stderr, "[Shell] Invalid command. Use: <program.txt> <data.byte>\n");
                prompt_shown = 0;
                print_prompt();
                return;
            }

            pid = loader(a, b);
            if (pid > 0)
                printf("[OS] PID %d accepted\n", pid);
            fflush(stdout);
            prompt_shown = 0;
            print_prompt();
            return;
        }

        if (shell_len < (int)sizeof(shell_line) - 1)
            shell_line[shell_len++] = ch;
    }

    
    saw_eof = 1;
    if (saw_eof) shell_exit = 1;
}

void scheduler(void)
{
    while (1) {
        int active = 0;
        ++scheduler_round;

        log_event("\n-------------------- SCHEDULER ROUND %lu --------------------\n", scheduler_round);
        promote_waiting();
        log_event("[QUEUE] Scheduler begins round %lu\n", scheduler_round);
        print_scheduler_status("before time slices");
        log_queue_status("Before time slices");

        for (int p = 0; p < NP; ++p) {
            if (proc_pid[p]) {
                active = 1;
                Task *t = find_task(proc_pid[p]);
                if (t) t->state = RUNNING;
                log_event("[RUN] Round %lu: Processor %d -> PID %d\n", scheduler_round, p, proc_pid[p]);
                log_event("[TIME SLICE] PID %d on Processor %d: execute up to %d instructions\n",
                          proc_pid[p], p, TIME_SLICE);
                print_time_slice(p, proc_pid[p], TIME_SLICE);
                process_instructions(p, TIME_SLICE);
                if (!end_of_simulation[p] && t) {
                    t->state = READY;
                    log_event("[TIME SLICE END] PID %d on Processor %d: RUNNING -> READY; CPU state preserved\n",
                              t->pid, p);
                    if (demo_mode)
                        printf("[DEMO] PID %d: RUNNING -> READY (processor %d keeps its saved state)\n",
                               t->pid, p);
                }
            }
        }

        reap_finished();
        promote_waiting();
        print_scheduler_status("after time slices");
        log_queue_status("After time slices");
        shell();

        for (int p = 0; p < NP; ++p)
            if (proc_pid[p]) active = 1;

        if (shell_exit && !active) break;
        msleep(1000); 
    }

    log_event("\n============================================================\n");
    log_event("[OS] Scheduler stopped: all tasks finished and shell exited.\n");
    log_event("============================================================\n");

    if (fd_log) {
        fflush(fd_log);
        fclose(fd_log);
        fd_log = NULL;
    }
}

int os_should_exit(void)
{
    return shell_exit;
}
