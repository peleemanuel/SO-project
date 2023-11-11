#define _GNU_SOURCE

#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdint.h>

#define PATH_MAX 4096
#define BUFF_SIZE 8192

typedef struct
{
    uint16_t signature;
    uint32_t fileSize;
    uint32_t reserved;
    uint32_t dataOffset;
    uint32_t size;
    uint32_t width;
    uint32_t height;
    uint16_t planes;
    uint16_t bitCount;
    uint32_t compression;
    uint32_t imageSize;
    uint32_t xPixelsPerM;
    uint32_t yPixelsPerM;
    uint32_t colorsUsed;
    uint32_t colorsImportant;
} bmp_header;
