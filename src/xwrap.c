#include "xwrap.h"

#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum
{
    MODE_IMAGE = 0,
    MODE_GRAPHICS,
    MODE_LEN,
} Mode;

struct _xw_handle
{
    Display* display;
    Window window;
    Mode mode;
    GC gc;
    XImage* image;
    uint16_t width;
    uint16_t height;
};

xw_handle* xw_create_window(int width, int height)
{
    xw_handle* handle = (xw_handle*)malloc(sizeof(xw_handle));
    handle->mode      = MODE_GRAPHICS;
    handle->display   = XOpenDisplay(NULL);
    if (handle->display == NULL)
    {
        printf("Unable to connect X server\n");
        free(handle);
        return NULL;
    }
    handle->window = XCreateSimpleWindow(
        handle->display, RootWindow(handle->display, DefaultScreen(handle->display)), 0, 0, width,
        height, 0, 0x000000, 0xFFFFFF);

    XMapWindow(handle->display, handle->window);
    XSelectInput(handle->display, handle->window, KeyPressMask | KeyReleaseMask);

    handle->gc = XCreateGC(handle->display, handle->window, 0, NULL);
    return handle;
}

void xw_free_window(xw_handle* handle)
{
    XFreeGC(handle->display, handle->gc);
    XDestroyWindow(handle->display, handle->window);
    XCloseDisplay(handle->display);
}

int xw_connect_image(xw_handle* handle, uint32_t* buffer, uint16_t width,
                     uint16_t height)
{
    if (handle->image != NULL)
    {
        fprintf(stderr, "ERROR: cannot reconnect image\n"); // TODO: reconnect image
        return 1;
    }
    handle->image  = XCreateImage(handle->display,
                                  DefaultVisual(handle->display, DefaultScreen(handle->display)), 24,
                                  ZPixmap, 0, (char*)buffer, width, height, 32, 0);

    if (handle->image == NULL)
    {
        fprintf(stderr, "ERROR: could not connect image\n");
        return 1;
    }

    handle->width  = width;
    handle->height = height;
    handle->mode = MODE_IMAGE;
    return 0;
}

int xw_draw(xw_handle* handle, uint32_t* data)
{
    if (handle->mode == MODE_GRAPHICS)
    {
        fprintf(stderr, "ERROR: image mode is not setup, please use 'xw_connect_image' function\n");
        return 1;
    }
    XPutImage(handle->display, handle->window, handle->gc, handle->image, 0, 0, 0, 0, handle->width,
              handle->height);
    XFlush(handle->display);
}

int xw_event_pending(xw_handle* handle)
{
    return XPending(handle->display);
}

int xw_get_next_event(xw_handle* handle, int* type, uint16_t* key_code)
{
    XEvent event;
    int ret   = XNextEvent(handle->display, &event);
    *key_code = event.xkey.keycode;
    *type     = event.type;

    return ret;
}