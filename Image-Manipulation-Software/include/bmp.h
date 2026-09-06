#ifndef BMP_H
#define BMP_H

#include <stdint.h>
#include "image.h"

#pragma pack(push, 1)

typedef struct{
    uint16_t signature;
    uint32_t file_size;
    uint32_t reserved;
    uint32_t pixel_offset;
} BMPFileHeader;

typedef struct{
    uint32_t header_size;
    int32_t width;
    int32_t height;
    uint16_t color_plane;
    uint16_t BPP;
    uint32_t compression;
    uint32_t pixel_data_size;
    uint32_t horizontal_resolution;
    uint32_t vertical_resolution;
    uint32_t colors_used;
    uint32_t important_colors;
} BMPInfoHeader;

#pragma pack(pop)

Image *load_bmp(const char *filename);
int save_bmp(const Image *image, const char *filename);

#endif