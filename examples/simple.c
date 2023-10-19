#define XWRAP_IMPLEMENTATION
#define XWRAP_AUTO_LINK
#include "../xwrap.h"

#include <stdint.h>
#include <stdio.h>

#define ESC 9

int main(int argc, char const* argv[])
{
    const unsigned int width  = 640;
    const unsigned int height = 480;
    xw_handle* handle         = xw_create_window(width, height);

    uint32_t* image_buffer = (uint32_t*)malloc(sizeof(uint32_t) * height * width);
    for (size_t i = 0; i < height * width; ++i) {
        image_buffer[i] = 0x000000;
    }

    // if (!xw_image_connect(handle, image_buffer, width, height))
    // {
    //     return 1;
    // }

    // int point = 0;
    for (;;) {
        while (xw_event_pending(handle)) {
            xw_event event;
            xw_get_next_event(handle, &event);
            switch (event.type) {
                case KeyPress: {
                    printf("pressed: %d\n", event.button.key_code);
                    switch (event.button.key_code) {
                        case ESC:
                            goto shutdown;
                    }
                }
                case KeyRelease: {
                    printf("released: %d\n", event.button.key_code);
                } break;
                case ButtonPress: {
                    if (event.mouse.button == Button1) {
                        printf("Left mouse button clicked at (%d, %d)\n", event.mouse.x,
                               event.mouse.y);
                    } else if (event.mouse.button == Button3) {
                        printf("Right mouse button clicked at (%d, %d)\n", event.mouse.x,
                               event.mouse.y);
                    }
                } break;

                default:
                    printf("event type clicked: %d\n", event.type);
                    break;
            }
        }

        /* image manipulation */
        // image_buffer[point++] = 0xFF0000;
        xw_draw_line(handle, 75, 100, 50, 10, 4, 0X00FFFF);
        xw_draw_rectangle(handle, 75, 100, 50, 10, true, 0X00FF00);
        xw_draw_circle(handle, 100, 100, 50, 0, 0XFF0000);
        xw_draw_triangle(handle, 100, 100, 90, 150, 130, 140, 0x0000FF);

        xw_draw(handle);

        xw_sleep_us(33 * 1000);
    }

shutdown:
    xw_free_window(handle);
    free(image_buffer);

    return 0;
}
