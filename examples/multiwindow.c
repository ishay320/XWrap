/*
This example opens 2 windows and show that:
1. It can draw separately on each window.
2. Get presses from the correct window.
 */
#define XWRAP_IMPLEMENTATION
#define XWRAP_AUTO_LINK
#include "../xwrap.h"

#include <stdint.h>
#include <stdio.h>

#define ESC 9

bool draw(xw_handle* handle, uint32_t color, bool draw_circle)
{
    xw_draw_line(handle, 75, 100, 50, 10, 4, color);
    xw_draw_rectangle(handle, 75, 100, 50, 10, true, 0X00FF00);
    if (draw_circle) {
        xw_draw_circle(handle, 100, 100, 50, 0, 0XFF0000);
    }
    xw_draw_triangle(handle, 100, 100, 90, 150, 130, 140, color);

    xw_draw(handle);
}

bool check_events(xw_handle* handle, const char* name)
{
    while (xw_event_pending(handle)) {
        xw_event event;
        xw_get_next_event(handle, &event);
        switch (event.type) {
            case KeyPress: {
                printf("pressed from window %s: %d\n", name, event.button.key_code);
                switch (event.button.key_code) {
                    case ESC:
                        return true;
                }
            }
            case KeyRelease: {
                printf("released from window %s: %d\n", name, event.button.key_code);
            } break;
            case ButtonPress: {
                if (event.mouse.button == Button1) {
                    printf("Left mouse button clicked at (%d, %d) from window %s\n", event.mouse.x,
                           event.mouse.y, name);
                } else if (event.mouse.button == Button3) {
                    printf("Right mouse button clicked at (%d, %d) from window %s\n", event.mouse.x,
                           event.mouse.y, name);
                }
            } break;

            default:
                printf("event type clicked from window %s: %d\n", name, event.type);
                break;
        }
    }
    return false;
}

int main(int argc, char const* argv[])
{
    const unsigned int width  = 640;
    const unsigned int height = 480;
    // Open 2 windows, dont forget to close them on end
    xw_handle* handle1 = xw_create_window("window1", width, height);
    xw_handle* handle2 = xw_create_window("window2", width, height);

    for (;;) {
        // Check each window for clicks
        if (check_events(handle1, "first window") || check_events(handle2, "second window")) {
            goto shutdown;
        }

        // Draws separately on each window
        draw(handle1, 0X00FFFF, true);
        draw(handle2, 0X0000FF, false);

        xw_sleep_ms(33);
    }

shutdown:
    // Close the windows
    xw_free_window(handle1);
    xw_free_window(handle2);

    return 0;
}
