#include <X11/Xlib.h>
#include <stdio.h>

int main(int argc, char const* argv[])
{
    Display* display = XOpenDisplay(NULL);
    if (display == NULL)
    {
        printf("Unable to connect X server\n");
        return 1;
    }

    Window window = XCreateSimpleWindow(display, RootWindow(display, DefaultScreen(display)), 200,
                                        0, 100, 100, 100, 0x000000, 0xFFFFFF);

    XMapWindow(display, window);
    XSync(display, False);

    getchar();
    return 0;
}
