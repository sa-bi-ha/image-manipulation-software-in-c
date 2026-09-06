#ifndef OPERATIONS_H
#define OPERATIONS_H

#include "image.h"

void grayscale(Image *image);
void brightness(Image *image, int input);
void inversion(Image *image);
void h_flip(Image *image);
void v_flip(Image *image);
Image *rotate(Image *original);
Image *crop(Image *original, int x, int y, int width, int height);
Image *blur(Image *original);
Image *sharpen(Image *original);
void undo(Image **original, Image **undo_image);
#endif