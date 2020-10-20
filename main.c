#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

#define REPEAT 1000000

// CHANGE THIS TO CHANGE TESTED SIZES
size_t LINE_BUFFER_SIZES[] = { 64, 128, 256, 512, 1024, 
                               2048, 4096, 8192, 16384,
                               32768, 65536, 1048576, 2097152}; // in Bytes

// writes the name data_<date-time> to a buffer
size_t format_data_dir_name(char* buf, size_t bufSize)
{
    time_t rawtime;
    struct tm* info;

    time(&rawtime);
    info = localtime(&rawtime);

    return strftime(buf, bufSize, "./data_%y%m%d%H%M%S", info);
}

// makes the dir with hardcoded 0755 permissions
int make_data_dir(const char* dirName)
{
    return mkdir(dirName, (mode_t)0755);
}


// opens a csv output file at the root dir at the data root dir
char fnameBuf[512];
FILE* open_data_file(const char* dataRoot, size_t lineBufSize)
{
    snprintf(fnameBuf, 512, "%s/%luB.csv", dataRoot, lineBufSize);
    return fopen(fnameBuf, "wb");
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


// runs the memory test for lineBufSize writes, 
// outputting generated data to a file,
// which it will put into the directory indicated by dataDirName
void memtest(const char* dataDirName, size_t lineBufSize)
{
    long int rep;
    uint64_t start, end, clock;
    char* lineBuffer = (char*) malloc(lineBufSize);
    char* lineBufferCopy = (char*) malloc(lineBufSize);

    char dataLineBuf[64];
    size_t bytesToWrite;
    FILE* dataFp = open_data_file(dataDirName, lineBufSize);

    // error check allocs
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

    clock = 0;
    for (rep = 0; rep < REPEAT; rep++) {

        // clock the memcpy call
        start = rdtsc();
        memcpy(lineBufferCopy, lineBuffer, lineBufSize);
        end = rdtsc();

        clflush(lineBuffer);
        clflush(lineBufferCopy);
        clock = clock + (end - start);

        // write data to output file & progress
        if(rep % 100000 == 0)
        {
            fprintf(stdout, "rep %lu: took %lu ticks to copy %luB\n", rep, (end-start), lineBufSize);
        }
        bytesToWrite = snprintf(dataLineBuf, 64, "%lu,", (end-start));
        if(rep == REPEAT - 1) 
            bytesToWrite -= 1; // account for trailing comma
        fwrite(&dataLineBuf, sizeof(char), bytesToWrite, dataFp);
    }
    fputc('\r', dataFp);
    fputc('\n', dataFp);

    fprintf(stdout, "buffer size %lu: took %lu ticks total.\n", lineBufSize, clock);

    // release resources
    fclose(dataFp);
    free(lineBufferCopy);
    free(lineBuffer);
}

// main
int main(int ac, char **av)
{
    int i;
    char dataDirName[256];

    format_data_dir_name(dataDirName, 256);
    make_data_dir(dataDirName);

    for(i = 0; i < sizeof(LINE_BUFFER_SIZES) / sizeof(size_t); i++)
    {
        fprintf(stdout, "------------------------------\n");
        memtest(dataDirName, LINE_BUFFER_SIZES[i]);
    }
    return 0;
}
