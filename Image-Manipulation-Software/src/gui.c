#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iup.h>
#include <iupdraw.h>

#include "image.h"
#include "bmp.h"
#include "operations.h"
#include "gui.h"

Ihandle *create_iup_image(Image *image){
    unsigned char *pixels = malloc(image->width * image->height * 3 * sizeof(unsigned char));

    for(int i = 0; i < image->height * image->width; i++){
        pixels[i * 3] = image->data[i].r;
        pixels[i * 3 + 1] = image->data[i].g;
        pixels[i * 3 + 2] = image->data[i].b;
    }

    Ihandle *iup_image = IupImageRGB(
        image->width,
        image->height,
        pixels
    );

    free(pixels);
    return iup_image;
}

int cancel_cb(Ihandle *ih){
    (void)ih;
    return IUP_CLOSE;
}

void update_canvas(GUI *gui){
    Ihandle *iup_image = create_iup_image(gui->image);

    IupSetHandle("CURRENT_IMAGE", iup_image);

    IupUpdate(gui->canvas);
}

int brightness_ok_cb(Ihandle *ih){
    
    GUI *gui = (GUI *)IupGetAttribute(ih, "GUI");
    Ihandle *text = IupGetAttributeHandle(ih, "INPUT");

    const char *value = IupGetAttribute(text, "VALUE");
    if(value == NULL || value[0] == '\0') return IUP_DEFAULT;

    int amount = atoi(value);

    if(amount < -255 || amount > 255) return IUP_DEFAULT;

    if(gui->undo_image != NULL)
        free_image(gui->undo_image);

    gui->undo_image = copy_image(gui->image);

    brightness(gui->image, amount);

    update_canvas(gui);

    return IUP_DEFAULT;
}

int crop_ok_cb(Ihandle *ih){   
    GUI *gui = (GUI *)IupGetAttribute(ih, "GUI");

    Ihandle *text_x = IupGetAttributeHandle(ih, "INPUT_X");
    Ihandle *text_y = IupGetAttributeHandle(ih, "INPUT_Y");
    Ihandle *text_w = IupGetAttributeHandle(ih, "INPUT_W");
    Ihandle *text_h = IupGetAttributeHandle(ih, "INPUT_H");

    int x = atoi(IupGetAttribute(text_x, "VALUE"));
    int y = atoi(IupGetAttribute(text_y, "VALUE"));
    int width = atoi(IupGetAttribute(text_w, "VALUE"));
    int height = atoi(IupGetAttribute(text_h, "VALUE"));

    if(x < 0 || y < 0 || width <= 0 || height <= 0)
    return IUP_DEFAULT;

    if(x + width > gui->image->width)
    return IUP_DEFAULT;

    if(y + height > gui->image->height)
    return IUP_DEFAULT;

    if(gui->undo_image != NULL)
    free_image(gui->undo_image);

    gui->undo_image = copy_image(gui->image);

    Image *new_image = crop(gui->image, x, y, width, height);

    free_image(gui->image);
    gui->image = new_image;

    update_canvas(gui);

    return IUP_CLOSE;
}

int open_image_cb(Ihandle *ih){
    Ihandle *dialog = IupFileDlg();

    IupSetAttribute(dialog, "DIALOGTYPE", "OPEN");
    IupSetAttribute(dialog, "FILTER", "*.bmp");
    IupSetAttribute(dialog, "TITLE", "Open BMP Image");

    GUI *gui = (GUI *)IupGetAttribute(ih, "GUI");

    IupPopup(dialog, IUP_CURRENT, IUP_CURRENT);

    const char *filename = IupGetAttribute(dialog, "VALUE");

    if(filename != NULL){
        gui->image = load_bmp(filename);

        if(gui->image == NULL){
            IupMessage(
                "Unsupported Image",
                "The selected file is not a valid 24-bit uncompressed BMP image."
            );
            IupDestroy(dialog);
            return IUP_DEFAULT;
        }
        gui->undo_image = NULL;

        update_canvas(gui);
    }

    return IUP_DEFAULT;
}

int canvas_image_cb(Ihandle *ih){
    GUI *gui = (GUI *)IupGetAttribute(ih, "GUI");

    if(gui->image == NULL)
        return IUP_DEFAULT;

    int canvas_width;
    int canvas_height;

    IupGetIntInt(ih, "RASTERSIZE", &canvas_width, &canvas_height);

    int image_width = gui->image->width;
    int image_height = gui->image->height;

    double scale_x = (double)canvas_width / image_width;
    double scale_y = (double)canvas_height / image_height;

    double scale = scale_x < scale_y ? scale_x : scale_y;

    if(scale > 1.0)
        scale = 1.0;

    int draw_width = (int)(image_width * scale);
    int draw_height = (int)(image_height * scale);

    int x = (canvas_width - draw_width) / 2;
    int y = (canvas_height - draw_height) / 2;

    IupDrawBegin(ih);

    IupDrawRectangle(
        ih,
        0,
        0,
        canvas_width,
        canvas_height
    );

    IupDrawImage(
        ih,
        "CURRENT_IMAGE",
        x,
        y,
        draw_width,
        draw_height
    );

    IupDrawEnd(ih);

    return IUP_DEFAULT;
}

int grayscale_cb(Ihandle *ih){
    GUI *gui = (GUI *)IupGetAttribute(ih, "GUI");

    if(gui->undo_image != NULL)
    free_image(gui->undo_image);

    gui->undo_image = copy_image(gui->image);

    grayscale(gui->image);

    update_canvas(gui);

    return IUP_DEFAULT;
}

int brightness_cb(Ihandle *ih){
    GUI *gui = (GUI *)IupGetAttribute(ih, "GUI");

    Ihandle *amount = IupLabel("Amount: ");
    Ihandle *text = IupText(NULL);
    IupSetAttribute(text, "RASTERSIZE", "100x25");
    IupSetAttribute(text, "MASK", "[-+]?[0-9]+");

    Ihandle *ok = IupButton("OK", NULL);
    IupSetAttribute(ok, "GUI", (char *)gui);
    IupSetAttributeHandle(ok, "INPUT", text);
    IupSetCallback(ok, "ACTION", (Icallback)brightness_ok_cb);

    Ihandle *cancel = IupButton("Cancel", NULL);
    IupSetCallback(cancel, "ACTION", (Icallback)cancel_cb);

    IupSetAttribute(ok, "RASTERSIZE", "70x30");
    IupSetAttribute(cancel, "RASTERSIZE", "70x30");

    Ihandle *input = IupHbox(amount, text, NULL);
    IupSetAttribute(input, "GAP", "10");

    Ihandle *buttons = IupHbox(ok, cancel, NULL);
    IupSetAttribute(buttons, "GAP", "10");

    Ihandle *layout = IupVbox(input, buttons, NULL);
    IupSetAttribute(layout, "GAP", "10");
    IupSetAttribute(layout, "MARGIN", "15x15");

    Ihandle *dialog = IupDialog(layout);
    IupSetAttribute(dialog, "TITLE", "Brightness");
    IupSetAttribute(dialog, "RASTERSIZE", "300x180");

    IupPopup(dialog, IUP_CURRENT, IUP_CURRENT);

    return IUP_DEFAULT;
}

int inversion_cb(Ihandle *ih){
    GUI *gui = (GUI *)IupGetAttribute(ih, "GUI");

    if(gui->undo_image != NULL)
    free_image(gui->undo_image);

    gui->undo_image = copy_image(gui->image);

    inversion(gui->image);

    update_canvas(gui);

    return IUP_DEFAULT;
}

int h_flip_cb(Ihandle *ih){
    GUI *gui = (GUI *)IupGetAttribute(ih, "GUI");

    if(gui->undo_image != NULL)
    free_image(gui->undo_image);

    gui->undo_image = copy_image(gui->image);

    h_flip(gui->image);

    update_canvas(gui);

    return IUP_DEFAULT;
}

int v_flip_cb(Ihandle *ih){
    GUI *gui = (GUI *)IupGetAttribute(ih, "GUI");

    if(gui->undo_image != NULL)
    free_image(gui->undo_image);

    gui->undo_image = copy_image(gui->image);

    v_flip(gui->image);

    update_canvas(gui);

    return IUP_DEFAULT;
}

int rotate_cb(Ihandle *ih){
    GUI *gui = (GUI *)IupGetAttribute(ih, "GUI");

    if(gui->undo_image != NULL)
    free_image(gui->undo_image);

    gui->undo_image = copy_image(gui->image);

    Image *new_image = rotate(gui->image);

    free_image(gui->image);
    gui->image = new_image;

    update_canvas(gui);

    return IUP_DEFAULT;
}

int crop_cb(Ihandle *ih){
    GUI *gui = (GUI *)IupGetAttribute(ih, "GUI");

    Ihandle *X = IupLabel("X: ");
    Ihandle *text_x = IupText(NULL);
    IupSetAttribute(text_x, "MASK", "[0-9]+");

    Ihandle *Y = IupLabel("Y: ");
    Ihandle *text_y = IupText(NULL);
    IupSetAttribute(text_y, "MASK", "[0-9]+");

    Ihandle *width = IupLabel("Width: ");
    Ihandle *text_w = IupText(NULL);
    IupSetAttribute(text_w, "MASK", "[0-9]+");

    Ihandle *height = IupLabel("Height: ");
    Ihandle *text_h = IupText(NULL);
    IupSetAttribute(text_h, "MASK", "[0-9]+");

    IupSetAttribute(text_x, "RASTERSIZE", "100x25");
    IupSetAttribute(text_y, "RASTERSIZE", "100x25");
    IupSetAttribute(text_w, "RASTERSIZE", "100x25");
    IupSetAttribute(text_h, "RASTERSIZE", "100x25");

    Ihandle *ok = IupButton("OK", NULL);
    IupSetAttribute(ok, "GUI", (char *)gui);
    IupSetAttributeHandle(ok, "INPUT_X", text_x);
    IupSetAttributeHandle(ok, "INPUT_Y", text_y);
    IupSetAttributeHandle(ok, "INPUT_W", text_w);
    IupSetAttributeHandle(ok, "INPUT_H", text_h);
    IupSetCallback(ok, "ACTION", (Icallback)crop_ok_cb);

    Ihandle *cancel = IupButton("Cancel", NULL);
    IupSetCallback(cancel, "ACTION", (Icallback)cancel_cb);

    Ihandle *xbox = IupHbox(X, text_x, NULL);
    Ihandle *ybox = IupHbox(Y, text_y, NULL);
    Ihandle *wbox = IupHbox(width, text_w, NULL);
    Ihandle *hbox = IupHbox(height, text_h, NULL);
    Ihandle *buttons = IupHbox(ok, cancel, NULL);
    IupSetAttribute(xbox, "GAP", "10");
    IupSetAttribute(ybox, "GAP", "10");
    IupSetAttribute(wbox, "GAP", "10");
    IupSetAttribute(hbox, "GAP", "10");
    IupSetAttribute(buttons, "GAP", "10");

    Ihandle *layout = IupVbox(xbox, ybox, wbox, hbox, buttons, NULL);
    IupSetAttribute(layout, "GAP", "8");
    IupSetAttribute(layout, "MARGIN", "15x15");

    Ihandle *dialog = IupDialog(layout);
    IupSetAttribute(dialog, "TITLE", "Crop");
    IupSetAttribute(dialog, "RASTERSIZE", "300x360");

    IupPopup(dialog, IUP_CURRENT, IUP_CURRENT);

    return IUP_DEFAULT;
}

int blur_cb(Ihandle *ih){
    GUI *gui = (GUI *)IupGetAttribute(ih, "GUI");

    if(gui->undo_image != NULL)
    free_image(gui->undo_image);

    gui->undo_image = copy_image(gui->image);

    Image *new_image = blur(gui->image);

    free_image(gui->image);
    gui->image = new_image;

    update_canvas(gui);

    return IUP_DEFAULT;
}

int sharpen_cb(Ihandle *ih){
    GUI *gui = (GUI *)IupGetAttribute(ih, "GUI");

    if(gui->undo_image != NULL)
    free_image(gui->undo_image);

    gui->undo_image = copy_image(gui->image);

    Image *new_image = sharpen(gui->image);

    free_image(gui->image);
    gui->image = new_image;

    update_canvas(gui);

    return IUP_DEFAULT;
}

int undo_cb(Ihandle *ih){
    GUI *gui = (GUI *)IupGetAttribute(ih, "GUI");

    if(gui->undo_image == NULL)
        return IUP_DEFAULT;

    undo(&gui->image, &gui->undo_image);

    update_canvas(gui);

    return IUP_DEFAULT;
}

int save_cb(Ihandle *ih){
    GUI *gui = (GUI *)IupGetAttribute(ih, "GUI");

    if(gui->image == NULL)
        return IUP_DEFAULT;

    Ihandle *dialog = IupFileDlg();

    IupSetAttribute(dialog, "DIALOGTYPE", "SAVE");
    IupSetAttribute(dialog, "FILTER", "*.bmp");
    IupSetAttribute(dialog, "TITLE", "Save BMP Image");

    IupPopup(dialog, IUP_CURRENT, IUP_CURRENT);

    char *filename = IupGetAttribute(dialog, "VALUE");
    char type[] = ".bmp";

    if(filename != NULL){
        strcat(filename, type);

        int len = strlen(filename);

        if(len < 4 || strcmp(filename + len - 4, ".bmp") != 0){
            strcat(filename, ".bmp");
        }

        save_bmp(gui->image, filename);
    }

    IupDestroy(dialog);

    return IUP_DEFAULT;
}

void gui_init(GUI *gui){
    IupOpen(NULL, NULL);

    Ihandle *file_label = IupLabel("FILE");
    Ihandle *adjust_label = IupLabel("ADJUSTMENTS");
    Ihandle *transform_label = IupLabel("TRANSFORM");

    gui->canvas = IupCanvas(NULL);
    IupSetAttribute(gui->canvas, "RASTERSIZE", "600x500");
    IupSetAttribute(gui->canvas, "DRAWCOLOR", "200 200 200");
    IupSetAttribute(gui->canvas, "DRAWSTYLE", "FILL");
    IupSetAttribute(gui->canvas, "GUI", (char *)gui);
    

    Ihandle *open_button = IupButton("Open", NULL);
    IupSetAttribute(open_button, "GUI", (char *)gui);
    IupSetAttribute(open_button, "RASTERSIZE", "120x30");
    IupSetCallback(open_button, "ACTION", (Icallback)open_image_cb);
    
    IupSetCallback(gui->canvas, "ACTION", (Icallback)canvas_image_cb);

    Ihandle *grayscale_button = IupButton("Grayscale", NULL);
    IupSetAttribute(grayscale_button, "GUI", (char *)gui);
    IupSetAttribute(grayscale_button, "RASTERSIZE", "120x30");
    IupSetCallback(grayscale_button, "ACTION", (Icallback)grayscale_cb);

    Ihandle *brightness_button = IupButton("Brightness", NULL);
    IupSetAttribute(brightness_button, "GUI", (char *)gui);
    IupSetAttribute(brightness_button, "RASTERSIZE", "120x30");
    IupSetCallback(brightness_button, "ACTION", (Icallback)brightness_cb);

    Ihandle *invert_button = IupButton("Invert", NULL);
    IupSetAttribute(invert_button, "GUI", (char *)gui);
    IupSetAttribute(invert_button, "RASTERSIZE", "120x30");
    IupSetCallback(invert_button, "ACTION", (Icallback)inversion_cb);

    Ihandle *h_flip_button = IupButton("Horizontal Flip", NULL);
    IupSetAttribute(h_flip_button, "GUI", (char *)gui);
    IupSetAttribute(h_flip_button, "RASTERSIZE", "120x30");
    IupSetCallback(h_flip_button, "ACTION", (Icallback)h_flip_cb);

    Ihandle *v_flip_button = IupButton("Vertical Flip", NULL);
    IupSetAttribute(v_flip_button, "GUI", (char *)gui);
    IupSetAttribute(v_flip_button, "RASTERSIZE", "120x30");
    IupSetCallback(v_flip_button, "ACTION", (Icallback)v_flip_cb);

    Ihandle *rotate_button = IupButton("Rotate", NULL);
    IupSetAttribute(rotate_button, "GUI", (char *)gui);
    IupSetAttribute(rotate_button, "RASTERSIZE", "120x30");
    IupSetCallback(rotate_button, "ACTION", (Icallback)rotate_cb);

    Ihandle *crop_button = IupButton("Crop", NULL);
    IupSetAttribute(crop_button, "GUI", (char *)gui);
    IupSetAttribute(crop_button, "RASTERSIZE", "120x30");
    IupSetCallback(crop_button, "ACTION", (Icallback)crop_cb);

    Ihandle *blur_button = IupButton("Blur", NULL);
    IupSetAttribute(blur_button, "GUI", (char *)gui);
    IupSetAttribute(blur_button, "RASTERSIZE", "120x30");
    IupSetCallback(blur_button, "ACTION", (Icallback)blur_cb);

    Ihandle *sharpen_button = IupButton("Sharpen", NULL);
    IupSetAttribute(sharpen_button, "GUI", (char *)gui);
    IupSetAttribute(sharpen_button, "RASTERSIZE", "120x30");
    IupSetCallback(sharpen_button, "ACTION", (Icallback)sharpen_cb);

    Ihandle *undo_button = IupButton("Undo", NULL);
    IupSetAttribute(undo_button, "GUI", (char *)gui);
    IupSetAttribute(undo_button, "RASTERSIZE", "120x30");
    IupSetCallback(undo_button, "ACTION", (Icallback)undo_cb);

    Ihandle *save_button = IupButton("Save", NULL);
    IupSetAttribute(save_button, "GUI", (char *)gui);
    IupSetAttribute(save_button, "RASTERSIZE", "120x30");
    IupSetCallback(save_button, "ACTION", (Icallback)save_cb);

    Ihandle *file_section = IupVbox(
        file_label,
        open_button,
        save_button,
        NULL
    );

    Ihandle *adjust_section = IupVbox(
        adjust_label,
        grayscale_button,
        brightness_button,
        invert_button,
        blur_button,
        sharpen_button,
        NULL
    );

    Ihandle *transform_section = IupVbox(
        transform_label,
        h_flip_button,
        v_flip_button,
        rotate_button,
        crop_button,
        undo_button,
        NULL
    );

    IupSetAttribute(file_section, "MARGIN", "0x10");
    IupSetAttribute(adjust_section, "MARGIN", "0x10");
    IupSetAttribute(transform_section, "MARGIN", "0x10");

    Ihandle *buttons = IupVbox(
        file_section,
        adjust_section,
        transform_section,
        NULL
    );

    IupSetAttribute(buttons, "GAP", "5");
    IupSetAttribute(buttons, "MARGIN", "10x10");

    Ihandle *layout = IupHbox(
        buttons,
        gui->canvas,
        NULL
    );

    IupSetAttribute(layout, "GAP", "10");
    

    Ihandle *dialog = IupDialog(layout);
    IupSetAttribute(dialog, "TITLE", "Image Manipulation Software");
    IupSetAttribute(dialog, "RASTERSIZE", "800x600");
    
    IupShow(dialog);

    IupMainLoop();
    IupClose();
}