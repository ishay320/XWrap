/*
This example shows the how to use sliders
 */
#define XWRAP_IMPLEMENTATION
#define XWRAP_AUTO_LINK
#include "../xwrap.h"

#include <stdio.h>
#define ESC 9

typedef struct {
    int x;
    int y;
} xw_point;

typedef struct {
    xw_point tl;
    xw_point br;
} xw_box;

typedef struct {
    xw_handle* handle;
    float last_value;
    char* slider_name;
    xw_box position;
    uint32_t color_circle;
    uint32_t color_line;
    uint32_t color_text;
} xw_slider;

xw_slider xw_create_slider(xw_handle* handle, xw_box position, float start_value, char* slider_name,
                           uint32_t color_circle, uint32_t color_line, uint32_t color_text)
{
    // clamp 0...1
    start_value = start_value < 0 ? 0 : 1 < start_value ? 1 : start_value;

    return (xw_slider){
        .handle       = handle,
        .last_value   = start_value,
        .slider_name  = slider_name,
        .position     = position,
        .color_circle = color_circle,
        .color_line   = color_line,
        .color_text   = color_text,
    };
}

bool xw_draw_slider(xw_slider* slider, float* output)
{
    // TODO: return false on fail
    const xw_point tl = slider->position.tl;
    const xw_point br = slider->position.br;

    const int event_pending = xw_event_pending(slider->handle);
    xw_event events[event_pending];
    size_t pos = 0;
    for (size_t i = 0; i < event_pending; i++) {
        XW_RETURN_ON_FAIL(xw_get_next_event(slider->handle, &events[pos]));
        // Check for mouse
        if (events[pos].type == ButtonPress && events[pos].mouse.button == Button1 &&
            tl.x < events[pos].mouse.x && events[pos].mouse.x < br.x &&
            tl.y < events[pos].mouse.y && events[pos].mouse.y < br.y) {
            slider->last_value = 1 - (float)(events[pos].mouse.y - tl.y) / (br.y - tl.y);
        } else {
            pos++;
        }
    }
    // put back on the event stack - (in the doc they said queue but...)
    for (int i = pos - 1; 0 <= i; i--) {
        xw_push_back_event(slider->handle, events[i]);
    }

    // draw slider
    const int x_middle = (br.x - tl.x) / 2 + tl.x;
    xw_draw_rectangle(slider->handle, tl.x, tl.y, br.x - tl.x, br.y - tl.y, false,
                      0xff0000); // debug
    xw_draw_line(slider->handle, x_middle, tl.y, x_middle, br.y, 1, slider->color_line);
    xw_draw_circle(slider->handle, x_middle, (br.y - tl.y) * (1 - slider->last_value) + tl.y,
                   (br.x - tl.x) / 6, true, slider->color_circle);

    // draw texts
    char value_s[32];
    sprintf(value_s, "%.3f", slider->last_value);
    xw_draw_text(slider->handle, tl.x + 2, br.y - 2, value_s, slider->color_text);
    const uint8_t text_size = 9;
    xw_draw_text(slider->handle, tl.x + 2, tl.y + text_size + 2, slider->slider_name,
                 slider->color_text);
    xw_draw(slider->handle);

    *output = slider->last_value;
    return true;
}

int main(int argc, char const* argv[])
{
    int width = 210, height = 600;
    xw_handle* handle = xw_create_window("example", width, height);

    // Settings
    char* slider_name     = "test drive";
    uint32_t color_circle = 0xff00ffff;
    uint32_t color_text   = 0x00000000;
    uint32_t color_line   = 0x00000000;

    xw_slider slider1 = xw_create_slider(handle, (xw_box){{0, 0}, {100, 400}}, 0.5, slider_name,
                                         color_circle, color_line, color_text);
    xw_slider slider2 = xw_create_slider(handle, (xw_box){{110, 0}, {210, 400}}, 1, slider_name,
                                         0xff00ff, color_line, color_text);
    // main loop
    for (;;) {
        // Clear screen
        xw_draw_background(handle, 0xffffffff);
        float value1;
        if (!xw_draw_slider(&slider1, &value1))
            break;
        float value2;
        if (!xw_draw_slider(&slider2, &value2))
            break;

        if (xw_wait_for_esc(handle, 1000 * 33)) {
            break;
        }
        printf("value1 is %f and value2 is %f\n", value1, value2);
    }

    xw_free_window(handle);
    return 0;
}
// TODO: add function flushing of events with background flushing - but somehow not the important
// ones
