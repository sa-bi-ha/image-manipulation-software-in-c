#include "bmp.h"
#include "image.h"
#include <stdio.h>

Image *load_bmp(const char *filename){

    BMPFileHeader file_header;
    BMPInfoHeader info_header;

    FILE *fp = fopen(filename, "rb");

    if(!fp) return NULL;

    if((fread(&file_header, sizeof(BMPFileHeader), 1, fp)) != 1){
        fclose(fp);
        return NULL;
    }
    if((fread(&info_header, sizeof(BMPInfoHeader), 1, fp)) != 1){
        fclose(fp);
        return NULL;
    }

    if(file_header.signature != 0x4D42 ||
        info_header.color_plane != 1 ||
        info_header.BPP != 24 ||
        info_header.compression != 0){
        fclose(fp);
        return NULL;
    }

    fseek(fp, file_header.pixel_offset, SEEK_SET);

    if(info_header.width <= 0 || info_header.height <= 0){
        fclose(fp);
        return NULL;
    }

    Image *image = create_image(info_header.width, info_header.height);

    if(image == NULL){
        fclose(fp);
        return NULL;
    }
    
    unsigned char kill_padding;
    int exact_width = 3 * image->width;
    int padding = 4 - exact_width % 4;
    if(padding == 4) padding = 0;

    for(int r = image->height - 1; r >= 0 ; r--){
        for(int c = 0; c < image->width; c++){
            int i = r * image->width + c; 
            unsigned char pixel[3];
            fread(pixel, sizeof(pixel), 1, fp);
            image->data[i].b = pixel[0];
            image->data[i].g = pixel[1];
            image->data[i].r = pixel[2];
        }
        for(int i = 0; i < padding; i++) fread(&kill_padding, sizeof(char), 1, fp);
    }

    fclose(fp);
    return image;
}

int save_bmp(const Image *image, const char *filename){

    BMPFileHeader file_header;
    BMPInfoHeader info_header;

    int exact_width = 3 * image->width;
    int padding = 4 - exact_width % 4;
    if(padding == 4) padding = 0;
    int pds = (exact_width + padding) * image->height;

    file_header.signature = 0x4D42;
    file_header.file_size = 14 + 40 + pds;
    file_header.reserved = 0;
    file_header.pixel_offset = 14 + 40;

    info_header.header_size = 40;
    info_header.width = image->width;
    info_header.height = image->height;
    info_header.color_plane = 1;
    info_header.BPP = 24;
    info_header.compression = 0;
    info_header.pixel_data_size = pds;
    info_header.horizontal_resolution = 0;
    info_header.vertical_resolution = 0;
    info_header.colors_used = 0;
    info_header.important_colors = 0;

    FILE *fp = fopen(filename, "wb");

    if(!fp) return 0;

    fwrite(&file_header, sizeof(BMPFileHeader), 1, fp);
    fwrite(&info_header, sizeof(BMPInfoHeader), 1, fp);

    unsigned char spadding[3] = {0};
    for(int r = image->height - 1; r >= 0 ; r--){
        for(int c = 0; c < image->width; c++){
            int i = r * image->width + c; 
            unsigned char pixel[3];
            pixel[0] = image->data[i].b;
            pixel[1] = image->data[i].g;
            pixel[2] = image->data[i].r;
            fwrite(pixel, sizeof(pixel), 1, fp);
        }
        for(int i = 0; i < padding; i++) fwrite(&spadding[i], sizeof(char), 1, fp);
    }

    fclose(fp);
    return 1;

}