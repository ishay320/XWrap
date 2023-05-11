#include <X11/Xlib.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

#define ESC 9

long long time_in_milliseconds(void)
{
    struct timeval tv;

    gettimeofday(&tv, NULL);
    return (((long long)tv.tv_sec) * 1000) + (tv.tv_usec / 1000);
}

int main(int argc, char const* argv[])
{
    Display* display = XOpenDisplay(NULL);
    if (display == NULL)
    {
        printf("Unable to connect X server\n");
        return 1;
    }

    unsigned int x = 300;
    unsigned int y = 250;
    Window window  = XCreateSimpleWindow(display, RootWindow(display, DefaultScreen(display)), 0, 0,
                                         x, y, 0, 0x000000, 0xFFFFFF);

    XMapWindow(display, window);
    XSelectInput(display, window, KeyPressMask | KeyReleaseMask);

    long long last = time_in_milliseconds();
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

        /* draw */

        long long now         = time_in_milliseconds();
        const long long delta = now - last;
        last                  = now;
        // printf("loop %lld %lld\n", delta, (33 * 2) - delta);
        usleep(33 * 1000);
    }

shutdown:
    XDestroyWindow(display, window);
    XCloseDisplay(display);

    return 0;
}
