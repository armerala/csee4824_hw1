#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define REPEAT 1000000

// opens a csv output file
char timeBuf[128];
char fnameBuf[255];
FILE* open_data_file(const char* fnameStem, size_t lineBufSize)
{
    time_t rawtime;
    struct tm* info;

    time(&rawtime);
    info = localtime(&rawtime);

    strftime(timeBuf, 128, "%y%m%d%H%M%S", info);
    snprintf(fnameBuf, 256, "%s_%luB_%s.csv", fnameStem, lineBufSize, timeBuf);
    return fopen(fnameBuf, "w");
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

size_t LINE_BUFFER_SIZES[] = { 64, 128 }; // in Bytes
long int rep;

void memtest(size_t lineBufSize)
{
    uint64_t start, end, clock;
    char* lineBuffer = (char*) malloc(lineBufSize);
    char* lineBufferCopy = (char*) malloc(lineBufSize);
    FILE* dataFp = open_data_file("data", lineBufSize);

    // some error checking on allocs
    if(lineBuffer == NULL || lineBufferCopy == NULL)
    {
        perror("bad buffer alloc!");
        exit(-1);
    }
    else if(dataFp == NULL) {
        perror("couldnt open output data file!");
        exit(-1);
    }

    // populate line buf
    for (int i = 0; i < lineBufSize; i++) {
        lineBuffer[i] = '1';
    }

    // clock memcpy for line buf REPEAT times
    clock = 0;
    for (rep = 0; rep < REPEAT; rep++) {

        start = rdtsc();
        memcpy(lineBufferCopy, lineBuffer, lineBufSize);
        end = rdtsc();

        clflush(lineBuffer);
        clflush(lineBufferCopy);
        clock = clock + (end - start);

        // print progress & write to output file
        if(rep % 100000 == 0)
        {
            printf("rep %lu: took %lu ticks to copy %luB\n", rep, (end-start), lineBufSize);
        }
        fprintf(dataFp, "%lu,\n", (end-start));
    }

    printf("buffer size %lu: took %lu ticks total.\n", lineBufSize, clock);

    //close output data file
    fclose(dataFp);
}

// main
int main(int ac, char **av)
{
    int i;

    for(i = 0; i < sizeof(LINE_BUFFER_SIZES) / sizeof(size_t); i++)
    {
        printf("------------------------------\n");
        memtest(LINE_BUFFER_SIZES[i]);
    }
    return 0;
}
