#include <stdio.h>
#include <stdlib.h>
#include "image.h"

Image *create_image(int width, int height){
    Image *image = malloc(sizeof(Image));

    if(!image) return NULL;

    image->width = width;
    image->height = height;

    image->data = malloc(width * height * sizeof(Pixel));
    
    if(!(image->data)){
        free(image);
        return NULL;
    }

    return image;
}

Image *copy_image(const Image *original){
    Image *copy = create_image(original->width, original->height);

    int total_pixels = original->width * original->height;

    for(int i = 0; i < total_pixels; i++)
        copy->data[i] = original->data[i];

    return copy;
}

void free_image(Image *image){

    if(!image) return;

    free(image->data);
    free(image);
}