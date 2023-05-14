#include "xwrap.h"

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

    const unsigned int width  = 640;
    const unsigned int height = 480;
    xw_handle* handle         = xw_create_window(width, height);

    uint32_t* image_buffer = (uint32_t*)malloc(sizeof(uint32_t) * height * width);
    for (size_t i = 0; i < height * width; ++i)
    {
        image_buffer[i] = 0x000000;
    }

    // if (xw_connect_image(handle, image_buffer, width, height))
    // {
    //     fprintf(stderr, "ERROR: could not connect image\n");
    //     return 1;
    // }

    long long last = time_in_milliseconds();
    int point      = 0;
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

        image_buffer[point++] = 0xFF0000;
        xw_draw_line(handle, 75, 100, 50, 10, 4, 0X00FFFF);
        xw_draw_rectangle(handle, 75, 100, 50, 10, true, 0X00FF00);
        xw_draw_circle(handle, 100, 100, 50, 0, 0XFF0000);
        xw_draw_triangle(handle, 100, 100, 90, 150, 130, 140, 0x0000FF);

        xw_draw(handle);

        long long now         = time_in_milliseconds();
        const long long delta = now - last;
        last                  = now;
        // printf("loop %lld %lld\n", delta, (33 * 2) - delta);
        sleep_us(33 * 1000);
    }

shutdown:
    xw_free_window(handle);
    free(image_buffer);

    return 0;
}
