#include "xwrap.h"

#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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
        height, 0, 0x000000, WhitePixel(handle->display, 0));

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

int xw_connect_image(xw_handle* handle, uint32_t* buffer, uint16_t width, uint16_t height)
{
    if (handle->image != NULL)
    {
        fprintf(stderr, "ERROR: cannot reconnect image\n"); // TODO: reconnect image
        return 1;
    }
    handle->image = XCreateImage(handle->display,
                                 DefaultVisual(handle->display, DefaultScreen(handle->display)), 24,
                                 ZPixmap, 0, (char*)buffer, width, height, 32, 0);

    if (handle->image == NULL)
    {
        fprintf(stderr, "ERROR: could not connect image\n");
        return 0;
    }

    handle->width  = width;
    handle->height = height;
    handle->mode   = MODE_IMAGE;
    return 1;
}

int xw_draw(xw_handle* handle)
{
    if (handle->mode == MODE_IMAGE)
    {
        XPutImage(handle->display, handle->window, handle->gc, handle->image, 0, 0, 0, 0,
                  handle->width, handle->height);
    }
    XFlush(handle->display);
}

int xw_draw_rectangle(xw_handle* handle, int x, int y, unsigned int width, unsigned int height,
                      uint32_t color, bool filled)
{

    if (handle->mode == MODE_GRAPHICS)
    {
        XSetForeground(handle->display, handle->gc, color);
        int ret;
        if (filled)
        {
            ret = XFillRectangle(handle->display, handle->window, handle->gc, x, y, width, height);
        }
        else
        {
            ret = XDrawRectangle(handle->display, handle->window, handle->gc, x, y, width, height);
        }
        return ret;
    }

    fprintf(stderr,
            "ERROR: trying to draw while the mode is not graphic mode - disconnect image\n");
    return 0;
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

static void sleep_us(unsigned long microseconds)
{
    struct timespec ts;
    ts.tv_sec  = microseconds / 1000000ul;
    ts.tv_nsec = (microseconds % 1000000ul) * 1000;
    nanosleep(&ts, NULL);
}

void xw_wait_for_esc(xw_handle* handle, uint64_t ms_sleep)
{
    for (;;)
    {
        while (xw_event_pending(handle))
        {
            int type;
            uint16_t keycode;
            xw_get_next_event(handle, &type, &keycode);
            if (type == KeyPress && keycode == 9)
            {
                return;
            }
        }
        sleep_us(ms_sleep);
    }
}