#ifndef GUI_H
#define GUI_H

#include <iup.h>
#include "image.h"
#include "operations.h"

typedef struct{
    Image *image;
    Image *undo_image;
    Ihandle *canvas;
} GUI;

void gui_init(GUI *gui);

#endif