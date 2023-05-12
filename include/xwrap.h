#ifndef XWRAP_H
#define XWRAP_H
#include <stdint.h>

#define KeyPress 2
#define KeyRelease 3

typedef struct _xw_handle xw_handle;

xw_handle* xw_create_window(int width, int height);
int xw_connect_image(xw_handle* handle, uint32_t* buffer, uint16_t image_width,
                     uint16_t image_height);

void xw_draw(xw_handle* handle, uint32_t* data);
void xw_free_window(xw_handle* handle);

int xw_event_pending(xw_handle* handle);
int xw_get_next_event(xw_handle* handle, int* type, uint16_t* key_code);

#endif // XWRAP_H