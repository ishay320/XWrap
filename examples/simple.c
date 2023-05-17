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
    for (size_t i = 0; i < height * width; ++i)
    {
        image_buffer[i] = 0x000000;
    }

    // if (!xw_image_connect(handle, image_buffer, width, height))
    // {
    //     return 1;
    // }

    // int point = 0;
    for (;;)
    {
        while (xw_event_pending(handle))
        {
            int type;
            uint16_t keycode;
            xw_get_next_event(handle, &type, &keycode);
            if (type == KeyPress)
            {
                printf("pressed: %d\n", keycode);
                switch (keycode)
                {
                    case ESC:
                        goto shutdown;
                }
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
