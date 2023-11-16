// acest fisier prezinta toate includurile, constantele, define urile si structurile custom de care am nevoie
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
#include <sys/wait.h>

#define PATH_MAX 4096
#define BUFF_SIZE 8192

// offseturile pentru jumpuri la offseturile respective in header
const off_t file_size = 2;
const off_t header_offset = 10;
const off_t width_offset = 18;
const off_t bitcount_offset = 28;
const off_t height_offset = 22;
const off_t image_size = 34;
const off_t offsetToPalette = 54;
typedef struct
{
    uint32_t fileSize;
    uint32_t width;
    uint32_t height;
    uint32_t imageSize;
    uint32_t headerOffset;
    uint16_t bitCount;

} bmp_header;
