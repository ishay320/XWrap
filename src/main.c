#include <X11/Xlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#define ESC 9

long long time_in_milliseconds(void)
{
    struct timeval tv;

    gettimeofday(&tv, NULL);
    return (((long long)tv.tv_sec) * 1000) + (tv.tv_usec / 1000);
}

void sleep_us(unsigned long microseconds)
{
    struct timespec ts;
    ts.tv_sec  = microseconds / 1000000ul;
    ts.tv_nsec = (microseconds % 1000000ul) * 1000;
    nanosleep(&ts, NULL);
}

int main(int argc, char const* argv[])
{
    Display* display = XOpenDisplay(NULL);
    if (display == NULL)
    {
        printf("Unable to connect X server\n");
        return 1;
    }

    unsigned int x = 640;
    unsigned int y = 480;
    Window window  = XCreateSimpleWindow(display, RootWindow(display, DefaultScreen(display)), 0, 0,
                                         x, y, 0, 0x000000, 0xFFFFFF);

    XMapWindow(display, window);
    XSelectInput(display, window, KeyPressMask | KeyReleaseMask);
    GC gc = XCreateGC(display, window, 0, NULL);

    const int image_width  = x;
    const int image_height = y;
    uint32_t* buffer       = (uint32_t*)malloc(sizeof(uint32_t) * image_height * image_width);
    for (size_t i = 0; i < image_height * image_width; ++i)
    {
        buffer[i] = 0x000000;
    }

    XImage* ximage = XCreateImage(display, DefaultVisual(display, DefaultScreen(display)), 24,
                                  ZPixmap, 0, (char*)buffer, image_width, image_height, 32, 0);

    long long last = time_in_milliseconds();
    int point      = 0;
    for (;;)
    {
        XEvent event;
        while (XPending(display))
        {
            XNextEvent(display, &event);
            if (event.type == KeyPress)
            {
                printf("pressed: %d\n", event.xkey.keycode);
                switch (event.xkey.keycode)
                {
                    case ESC:
                        goto shutdown;
                }
            }
        }
        buffer[point++] = 0xFF0000;

        /* draw */
        // XDrawLine(display, window, gc, 0, 0, 500, 300);
        XPutImage(display, window, gc, ximage, 0, 0, 0, 0, image_width, image_height);

        long long now         = time_in_milliseconds();
        const long long delta = now - last;
        last                  = now;
        // printf("loop %lld %lld\n", delta, (33 * 2) - delta);
        sleep_us(33 * 1000);
    }

shutdown:
    XFreeGC(display, gc);
    XDestroyWindow(display, window);
    XCloseDisplay(display);
    free(buffer);

    return 0;
}
