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

#if !defined(XWRAP_AUTO_LINK)
#include <X11/Xlib.h>
#endif // XWRAP_AUTO_LINK

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef XWRAP_AUTO_LINK
#include <dlfcn.h>

/* Typedefs */
typedef struct _XDisplay Display;
typedef struct _XGC* GC;

typedef unsigned long XID;
typedef XID Window;
typedef XID Drawable;
typedef XID Font;
typedef XID Pixmap;
typedef XID Colormap;

typedef unsigned long Time;
typedef char* XPointer;

typedef struct _XExtData XExtData;         // Shortened
typedef struct _XGCValues XGCValues;       // Shortened
typedef struct _ScreenFormat ScreenFormat; // Shortened
typedef struct _Visual Visual;             // Shortened
typedef struct _Depth Depth;               // Shortened
typedef struct _XImage XImage;             // Shortened

typedef struct
{
    XExtData* ext_data;
    struct _XDisplay* display;
    Window root;
    int width, height, mwidth, mheight, ndepths;
    Depth* depths;
    int root_depth;
    Visual* root_visual;
    GC default_gc;
    Colormap cmap;
    unsigned long white_pixel, black_pixel;
    int max_maps, min_maps, backing_store, save_unders;
    long root_input_mask;
} Screen;

typedef struct
{
    XExtData* ext_data;
    struct _XPrivate* private1;
    int fd, private2, proto_major_version, proto_minor_version;
    char* vendor;
    XID private3, private4, private5;
    int private6;
    XID (*resource_alloc)(struct _XDisplay*);
    int byte_order, bitmap_unit, bitmap_pad, bitmap_bit_order, nformats;
    ScreenFormat* pixmap_format;
    int private8, release;
    struct _XPrivate *private9, *private10;
    int qlen;
    unsigned long last_request_read, request;
    XPointer private11, private12, private13, private14;
    unsigned max_request_size;
    struct _XrmHashBucketRec* db;
    int (*private15)(struct _XDisplay*);
    char* display_name;
    int default_screen, nscreens;
    Screen* screens;
    unsigned long motion_buffer, private16;
    int min_keycode, max_keycode;
    XPointer private17, private18;
    int private19;
    char* xdefaults;

}* _XPrivDisplay;

typedef struct
{
    int type;
    unsigned long serial;
    int send_event;
    Display* display;
    Window window, root, subwindow;
    Time time;
    int x, y, x_root, y_root;
    unsigned int state, keycode;
    int same_screen;
} XKeyEvent;

typedef union _XEvent
{
    int type;
    XKeyEvent xkey;
    long pad[24];
} XEvent;

typedef struct
{
    short x, y;
} XPoint;

/* Definitions */
#define ScreenOfDisplay(dpy, scr) (&((_XPrivDisplay)(dpy))->screens[scr])
#define RootWindow(dpy, scr) (ScreenOfDisplay(dpy, scr)->root)
#define DefaultScreen(dpy) (((_XPrivDisplay)(dpy))->default_screen)
#define DefaultVisual(dpy, scr) (ScreenOfDisplay(dpy, scr)->root_visual)
#define WhitePixel(dpy, scr) (ScreenOfDisplay(dpy, scr)->white_pixel)

#define NoEventMask 0L
#define KeyPressMask (1L << 0)
#define KeyReleaseMask (1L << 1)
#define ZPixmap 2
#define LineSolid 0
#define CapButt 1
#define JoinMiter 0
#define Nonconvex 1
#define CoordModeOrigin 0

/* Function declarations */
Display* (*XOpenDisplay)(const char*)                                                   = NULL;
Window (*XCreateSimpleWindow)(Display*, Window, int, int, unsigned int, unsigned int, unsigned int,
                              unsigned long, unsigned long)                             = NULL;
int (*XMapWindow)(Display*, Window)                                                     = NULL;
int (*XSelectInput)(Display*, Window, long)                                             = NULL;
GC (*XCreateGC)(Display*, Drawable, unsigned long, XGCValues*)                          = NULL;
int (*XSetForeground)(Display*, GC, unsigned long)                                      = NULL;
int (*XFillRectangle)(Display*, Drawable, GC, int, int, unsigned int, unsigned int)     = NULL;
int (*XFlush)(Display*)                                                                 = NULL;
int (*XFreeGC)(Display*, GC)                                                            = NULL;
int (*XDestroyWindow)(Display*, Window)                                                 = NULL;
int (*XCloseDisplay)(Display*)                                                          = NULL;
XImage* (*XCreateImage)(Display*, Visual*, unsigned int, int, int, char*, unsigned int,
                        unsigned int, int, int)                                         = NULL;
int (*XPutImage)(Display*, Drawable, GC, XImage*, int, int, int, int, unsigned int,
                 unsigned int)                                                          = NULL;
int (*XDrawRectangle)(Display*, Drawable, GC, int, int, unsigned int, unsigned int)     = NULL;
int (*XSetLineAttributes)(Display*, GC, unsigned int, int, int, int)                    = NULL;
int (*XDrawLine)(Display*, Drawable, GC, int, int, int, int)                            = NULL;
int (*XFillArc)(Display*, Drawable, GC, int, int, unsigned int, unsigned int, int, int) = NULL;
int (*XDrawArc)(Display*, Drawable, GC, int, int, unsigned int, unsigned int, int, int) = NULL;
int (*XDrawPoint)(Display*, Drawable, GC, int, int)                                     = NULL;
int (*XFillPolygon)(Display*, Drawable, GC, XPoint*, int, int, int)                     = NULL;
int (*XPending)(Display*)                                                               = NULL;
int (*XNextEvent)(Display*, XEvent*)                                                    = NULL;

/* Linker */
void* dl_handle         = NULL;
const char* name_libx11 = "libX11.so";
const struct
{
    const char* name;
    void** fun;
} dl_fun[] = {
    {"XOpenDisplay", (void**)&XOpenDisplay},
    {"XCreateSimpleWindow", (void**)&XCreateSimpleWindow},
    {"XMapWindow", (void**)&XMapWindow},
    {"XSelectInput", (void**)&XSelectInput},
    {"XCreateGC", (void**)&XCreateGC},
    {"XSetForeground", (void**)&XSetForeground},
    {"XFillRectangle", (void**)&XFillRectangle},
    {"XFlush", (void**)&XFlush},
    {"XFreeGC", (void**)&XFreeGC},
    {"XDestroyWindow", (void**)&XDestroyWindow},
    {"XCloseDisplay", (void**)&XCloseDisplay},
    {"XCreateImage", (void**)&XCreateImage},
    {"XPutImage", (void**)&XPutImage},
    {"XDrawRectangle", (void**)&XDrawRectangle},
    {"XSetLineAttributes", (void**)&XSetLineAttributes},
    {"XDrawLine", (void**)&XDrawLine},
    {"XFillArc", (void**)&XFillArc},
    {"XDrawArc", (void**)&XDrawArc},
    {"XDrawPoint", (void**)&XDrawPoint},
    {"XFillPolygon", (void**)&XFillPolygon},
    {"XPending", (void**)&XPending},
    {"XNextEvent", (void**)&XNextEvent},
};

const size_t dl_fun_len = sizeof(dl_fun) / sizeof(*dl_fun);

void _xw_d_unlink(void* handle)
{
    dlclose(handle);
}

bool _xw_d_link(void** handle)
{
    if (*handle != NULL)
    {
        fprintf(stderr, "WARNING: double link\n");
        return true;
    }

    *handle = dlopen(name_libx11, RTLD_LAZY | RTLD_GLOBAL);
    for (size_t i = 0; i < dl_fun_len; i++)
    {
        *dl_fun[i].fun = dlsym(*handle, dl_fun[i].name);
        if (*dl_fun[i].fun == NULL)
        {
            return false;
        }
    }
    return true;
}
#endif // XWRAP_AUTO_LINK

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
#ifdef XWRAP_AUTO_LINK
    if (!_xw_d_link(&dl_handle))
    {
        fprintf(stderr, "ERROR: could not link with x11: %s\n", dlerror());
        exit(1);
    }
#endif // XWRAP_AUTO_LINK

    xw_handle* handle = (xw_handle*)malloc(sizeof(xw_handle));
    handle->mode      = MODE_GRAPHICS;
    handle->display   = XOpenDisplay(NULL);
    if (handle->display == NULL)
    {
        fprintf(stderr, "ERROR: Unable to connect X server\n");
        free(handle);
        return NULL;
    }
    handle->window = XCreateSimpleWindow(
        handle->display, RootWindow(handle->display, DefaultScreen(handle->display)), 0, 0, width,
        height, 0, 0x000000, WhitePixel(handle->display, 0));

    XMapWindow(handle->display, handle->window);
    XSelectInput(handle->display, handle->window, KeyPressMask | KeyReleaseMask);

    handle->gc    = XCreateGC(handle->display, handle->window, 0, NULL);
    handle->image = NULL;
    return handle;
}

XW_DEF void xw_free_window(xw_handle* handle)
{
    XFreeGC(handle->display, handle->gc);
    XDestroyWindow(handle->display, handle->window);
    XCloseDisplay(handle->display);
#ifdef XWRAP_AUTO_LINK
    // TODO: what with multi screens?
    _xw_d_unlink(dl_handle);
    dl_handle = NULL;
#endif // XWRAP_AUTO_LINK
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

static void _xw_sleep_us(unsigned long microseconds)
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
        _xw_sleep_us(ms_sleep);
    }
}
#endif // XWRAP_IMPLEMENTATION