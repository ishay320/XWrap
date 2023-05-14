#ifndef XWRAP_H
#define XWRAP_H
#include <stdbool.h>
#include <stdint.h>

#define KeyPress 2
#define KeyRelease 3

typedef struct _xw_handle xw_handle;

xw_handle* xw_create_window(int width, int height);
void xw_free_window(xw_handle* handle);

int xw_connect_image(xw_handle* handle, uint32_t* buffer, uint16_t width, uint16_t height);
int xw_draw(xw_handle* handle);

int xw_draw_rectangle(xw_handle* handle, int x, int y, unsigned int width, unsigned int height,
                      bool fill, uint32_t color);
int xw_draw_line(xw_handle* handle, int x0, int y0, int x1, int y1, uint16_t width, uint32_t color);
int xw_draw_circle(xw_handle* handle, int x, int y, int r, bool fill, uint32_t color);
int xw_draw_pixel(xw_handle* handle, int x, int y, uint32_t color);
int xw_draw_triangle(xw_handle* handle, int x0, int y0, int x1, int y1, int x2, int y2,
                     uint32_t color);

int xw_event_pending(xw_handle* handle);
int xw_get_next_event(xw_handle* handle, int* type, uint16_t* key_code);

void xw_wait_for_esc(xw_handle* handle, uint64_t ms_sleep);

// TODO: add graphics functions: rectangle circle line arch...
#endif // XWRAP_H