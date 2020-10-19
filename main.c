#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define REPEAT 1000000

// opens a csv output file
char time_buf[128];
char fname_buf[255];
FILE* open_data_file(const char* fname_stem)
{
    time_t rawtime;
    struct tm* info;

    time(&rawtime);
    info = localtime(&rawtime);

    strftime(time_buf, 128, "%y%m%d-%H%M%S", info);
    snprintf(fname_buf, 256, "%s_%s.csv", fname_stem, time_buf);
    return fopen(fname_buf, "w");
}

// flush cache line
__attribute__((always_inline)) inline void clflush(volatile void *p) 
{
    asm volatile ("clflush (%0)" :: "r"(p));
}

// fetch read-time stamp counter
__attribute__((always_inline)) inline uint64_t rdtsc()
{
    unsigned long a, d;
    asm volatile ("rdtsc" : "=a" (a), "=d" (d));
    return a | ((uint64_t)d << 32);
}

char data_line_buf[255];
char lineBuffer[64];
long int rep;

void memtest()
{
    FILE* data_fp;
    uint64_t start, end, clock;
    char* lineBuffer = (char*) malloc(64);
    char* lineBufferCopy = (char*) malloc(64);

    // open output data file
    data_fp = open_data_file("data");
    if(data_fp == NULL) {
        perror("couldnt open output data file!");
        exit(-1);
    }

    // populate line buf
    for (int i = 0; i < 64; i++) {
        lineBuffer[i] = '1';
    }

    // clock memcpy for line buf REPEAT times
    clock = 0;
    for (rep = 0; rep < REPEAT; rep++) {

        start = rdtsc();
        memcpy(lineBufferCopy, lineBuffer, 64);
        end = rdtsc();

        clflush(lineBuffer);
        clflush(lineBufferCopy);
        clock = clock + (end - start);

        printf("%lu ticks to copy 64B\n", (end-start));

        // write bu
        snprintf(data_line_buf, 256, "%lu,\n", (end-start));
        fputs(data_line_buf, data_fp);
    }

    printf("took %lu ticks total\n", clock);

    //close output data file
    fclose(data_fp);
}

// main
int main(int ac, char **av)
{
    printf("------------------------------\n");
    memtest();
    return 0;
}
