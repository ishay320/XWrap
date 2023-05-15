#ifndef XWRAP_INCLUDE_H
#define XWRAP_INCLUDE_H
#include <stdbool.h>
#include <stdint.h>

#ifndef XW_DEF
#ifdef XW_STATIC
#define XW_DEF static
#else
#define XW_DEF extern
#endif
#endif

#define KeyPress 2
#define KeyRelease 3

typedef struct _xw_handle xw_handle;

XW_DEF xw_handle* xw_create_window(int width, int height);
XW_DEF void xw_free_window(xw_handle* handle);

XW_DEF int xw_connect_image(xw_handle* handle, uint32_t* buffer, uint16_t width, uint16_t height);
XW_DEF int xw_draw(xw_handle* handle);

XW_DEF int xw_draw_rectangle(xw_handle* handle, int x, int y, unsigned int width,
                             unsigned int height, bool fill, uint32_t color);
XW_DEF int xw_draw_line(xw_handle* handle, int x0, int y0, int x1, int y1, uint16_t width,
                        uint32_t color);
XW_DEF int xw_draw_circle(xw_handle* handle, int x, int y, int r, bool fill, uint32_t color);
XW_DEF int xw_draw_pixel(xw_handle* handle, int x, int y, uint32_t color);
XW_DEF int xw_draw_triangle(xw_handle* handle, int x0, int y0, int x1, int y1, int x2, int y2,
                            uint32_t color);

XW_DEF int xw_event_pending(xw_handle* handle);
XW_DEF int xw_get_next_event(xw_handle* handle, int* type, uint16_t* key_code);

XW_DEF void xw_wait_for_esc(xw_handle* handle, uint64_t ms_sleep);

#endif // XWRAP_INCLUDE_H

#ifdef XWRAP_IMPLEMENTATION
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

XW_DEF xw_handle* xw_create_window(int width, int height)
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

XW_DEF void xw_free_window(xw_handle* handle)
{
    XFreeGC(handle->display, handle->gc);
    XDestroyWindow(handle->display, handle->window);
    XCloseDisplay(handle->display);
}

XW_DEF int xw_connect_image(xw_handle* handle, uint32_t* buffer, uint16_t width, uint16_t height)
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

XW_DEF int xw_draw(xw_handle* handle)
{
    if (handle->mode == MODE_IMAGE)
    {
        XPutImage(handle->display, handle->window, handle->gc, handle->image, 0, 0, 0, 0,
                  handle->width, handle->height);
    }
    XFlush(handle->display);
}

XW_DEF int xw_draw_rectangle(xw_handle* handle, int x, int y, unsigned int width,
                             unsigned int height, bool fill, uint32_t color)
{
    if (handle->mode == MODE_GRAPHICS)
    {
        XSetForeground(handle->display, handle->gc, color);
        if (fill)
        {
            return XFillRectangle(handle->display, handle->window, handle->gc, x, y, width, height);
        }
        return XDrawRectangle(handle->display, handle->window, handle->gc, x, y, width, height);
    }

    fprintf(stderr,
            "ERROR: trying to draw while the mode is not graphic mode - disconnect image\n");
    return 0;
}

XW_DEF int xw_draw_line(xw_handle* handle, int x0, int y0, int x1, int y1, uint16_t width,
                        uint32_t color)
{
    if (handle->mode == MODE_GRAPHICS)
    {
        XSetLineAttributes(handle->display, handle->gc, width, LineSolid, CapButt, JoinMiter);
        XSetForeground(handle->display, handle->gc, color);

        return XDrawLine(handle->display, handle->window, handle->gc, x0, y0, x1, y1);
    }

    fprintf(stderr,
            "ERROR: trying to draw while the mode is not graphic mode - disconnect image\n");
    return 0;
}

XW_DEF int xw_draw_circle(xw_handle* handle, int x, int y, int r, bool fill, uint32_t color)
{
    if (handle->mode == MODE_GRAPHICS)
    {
        XSetForeground(handle->display, handle->gc, color);
        if (fill)
        {
            return XFillArc(handle->display, handle->window, handle->gc, x - r, y - r, 2 * r, 2 * r,
                            0, 360 * 64);
        }
        return XDrawArc(handle->display, handle->window, handle->gc, x - r, y - r, 2 * r, 2 * r, 0,
                        360 * 64);
    }

    fprintf(stderr,
            "ERROR: trying to draw while the mode is not graphic mode - disconnect image\n");
    return 0;
}

XW_DEF int xw_draw_pixel(xw_handle* handle, int x, int y, uint32_t color)
{
    if (handle->mode == MODE_GRAPHICS)
    {
        XSetForeground(handle->display, handle->gc, color);
        return XDrawPoint(handle->display, handle->window, handle->gc, x, y);
    }

    fprintf(stderr,
            "ERROR: trying to draw while the mode is not graphic mode - disconnect image\n");
    return 0;
}

XW_DEF int xw_draw_triangle(xw_handle* handle, int x0, int y0, int x1, int y1, int x2, int y2,
                            uint32_t color)
{
    if (handle->mode == MODE_GRAPHICS)
    {
        XSetForeground(handle->display, handle->gc, color);
        XPoint points[3] = {
            [0] = {.x = x0, .y = y0},
            [1] = {.x = x1, .y = y1},
            [2] = {.x = x2, .y = y2},
        };
        int npoints = 3;
        int shape   = Nonconvex;
        int mode    = CoordModeOrigin;

        return XFillPolygon(handle->display, handle->window, handle->gc, points, npoints, shape,
                            mode);
    }

    fprintf(stderr,
            "ERROR: trying to draw while the mode is not graphic mode - disconnect image\n");
    return 0;
}

XW_DEF int xw_event_pending(xw_handle* handle)
{
    return XPending(handle->display);
}

XW_DEF int xw_get_next_event(xw_handle* handle, int* type, uint16_t* key_code)
{
    XEvent event;
    int ret   = XNextEvent(handle->display, &event);
    *key_code = event.xkey.keycode;
    *type     = event.type;

    return ret;
}

static void xw_sleep_us(unsigned long microseconds)
{
    struct timespec ts;
    ts.tv_sec  = microseconds / 1000000ul;
    ts.tv_nsec = (microseconds % 1000000ul) * 1000;
    nanosleep(&ts, NULL);
}

XW_DEF void xw_wait_for_esc(xw_handle* handle, uint64_t ms_sleep)
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
        xw_sleep_us(ms_sleep);
    }
}
#endif // XWRAP_IMPLEMENTATION