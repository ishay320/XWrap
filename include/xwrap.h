#ifndef XWRAP_H
#define XWRAP_H
#include <stdint.h>

#define KeyPress 2
#define KeyRelease 3

typedef struct _xw_handle xw_handle;

xw_handle* xw_create_window(int width, int height);
void xw_free_window(xw_handle* handle);

int xw_connect_image(xw_handle* handle, uint32_t* buffer, uint16_t width, uint16_t height);
int xw_draw(xw_handle* handle);

int xw_event_pending(xw_handle* handle);
int xw_get_next_event(xw_handle* handle, int* type, uint16_t* key_code);

void xw_wait_for_esc(xw_handle* handle, uint64_t ms_sleep);

// TODO: add graphics functions: rectangle circle line arch...
#endif // XWRAP_H