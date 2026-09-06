#include "image.h"
#include <stdio.h>

void grayscale(Image *image){
    for(int i = 0; i < image->width * image->height; i++){
        int gray = 0.299 * image->data[i].r + 0.587 * image->data[i].g + 0.114 * image->data[i].b;
        image->data[i].r = gray;
        image->data[i].g = gray;
        image->data[i].b = gray;
    }
}

void brightness(Image *image, int input){
    // int input;
    // printf("Enter a value: ");
    // scanf("%d", &input);

    for(int i = 0; i < image->width * image->height; i++){

        if((input + image->data[i].r) > 255) image->data[i].r = 255;
        else if((input + image->data[i].r) < 0) image->data[i].r = 0;
        else image->data[i].r = input + image->data[i].r;

        if((input + image->data[i].g) > 255) image->data[i].g = 255;
        else if((input + image->data[i].g) < 0) image->data[i].g = 0;
        else image->data[i].g = input + image->data[i].g;

        if((input + image->data[i].b) > 255) image->data[i].b = 255;
        else if((input + image->data[i].b) < 0) image->data[i].b = 0;
        else image->data[i].b = input + image->data[i].b;
    }
}

void inversion(Image *image){
    for(int i = 0; i < image->width * image->height; i++){
        image->data[i].r = 255 - image->data[i].r;
        image->data[i].g = 255 - image->data[i].g;
        image->data[i].b = 255 - image->data[i].b;
    }
}

void h_flip(Image *image){
    for(int r = 0; r < image->height; r++){
        for(int c = 0; c < (image->width / 2); c++){
            Pixel temp;
            int i = r * image->width + c;
            int j = r * image->width + image->width - 1 - c;

            temp = image->data[i];
            image->data[i] = image->data[j];
            image->data[j] = temp;
        }
    }
}

void v_flip(Image *image){
    for(int r = 0; r < (image->height / 2); r++){
        for(int c = 0; c < image->width; c++){
            Pixel temp;
            int i = r * image->width + c;
            int j = (image->height - 1 - r) * image->width + c;

            temp = image->data[i];
            image->data[i] = image->data[j];
            image->data[j] = temp;
        }
    }
}

Image *rotate(Image *original){
    Image *rotated = create_image(original->height, original->width);
    int i, r, c;

    for(i = rotated->width - 1, r = 0; r < original->height && i >= 0 ; i--, r++){
        for(c = 0; c < original->width; c++){
            rotated->data[c * rotated->width + i] = original->data[r * original->width + c];
        }
    }

    return rotated;
}

Image *crop(Image *original, int x, int y, int width, int height){

    if(original->width < y + width || original->height < x + height) return NULL;

    Image *cropped = create_image(width, height);

    for(int i = 0 ; i < height; i++){
        for(int j = 0 ; j < width; j++){
            cropped->data[i * cropped->width + j] = original->data[(x + i) * original->width + y + j];
        }
    }

    return cropped;
}

Image *blur(Image *original){
    Image *blurred = create_image(original->width, original->height);


    for(int r = 0; r < original->height; r++){
        for(int c = 0; c < original->width; c++){
            
            int r_sum = 0, g_sum = 0, b_sum = 0, count = 0;

            for(int pr = -1; pr <= 1; pr++){
                for(int pc = -1; pc <= 1; pc++){
                    if(((r + pr) >= 0 && (r + pr) < original->height) && ((c + pc) >= 0 && (c + pc) < original->width)){
                        r_sum = r_sum + original->data[(r + pr) * original->width + c + pc].r;
                        g_sum = g_sum + original->data[(r + pr) * original->width + c + pc].g;
                        b_sum = b_sum + original->data[(r + pr) * original->width + c + pc].b;
                        count++;
                    }  
                }
            }

            blurred->data[r * blurred->width + c].r = r_sum / count;      
            blurred->data[r * blurred->width + c].g = g_sum / count;     
            blurred->data[r * blurred->width + c].b = b_sum / count;
        }
    }

    return blurred;
}

Image *sharpen(Image *original){
    Image *sharpened = create_image(original->width, original->height);


    for(int r = 0; r < original->height; r++){
        for(int c = 0; c < original->width; c++){
            
            int r_sum = 0, g_sum = 0, b_sum = 0;

            for(int pr = -1; pr <= 1; pr++){
                for(int pc = -1; pc <= 1; pc++){
                    int weight = 0;
                    if((pr == -1 && pc == 0) || (pr == 0 && pc == -1) || (pr == 0 && pc == 1) || (pr == 1 && pc == 0)) weight = -1;
                    else if(pr == 0 && pc == 0) weight = 5;

                    if(((r + pr) >= 0 && (r + pr) < original->height) && ((c + pc) >= 0 && (c + pc) < original->width)){
                        r_sum = r_sum + original->data[(r + pr) * original->width + c + pc].r * weight;
                        g_sum = g_sum + original->data[(r + pr) * original->width + c + pc].g * weight;
                        b_sum = b_sum + original->data[(r + pr) * original->width + c + pc].b * weight;
                    }  
                }
            }
            if(r_sum < 0) r_sum = 0;
            else if(r_sum > 255) r_sum = 255;
            sharpened->data[r * sharpened->width + c].r = r_sum;

            if(g_sum < 0) g_sum = 0;
            else if(g_sum > 255) g_sum = 255;
            sharpened->data[r * sharpened->width + c].g = g_sum;

            if(b_sum < 0) b_sum = 0;
            else if(b_sum > 255) b_sum = 255;     
            sharpened->data[r * sharpened->width + c].b = b_sum;
        }
    }

    return sharpened;
}

void undo(Image **original, Image **undo_image){
    free_image(*original);
    *original = *undo_image;
    *undo_image = NULL;
}