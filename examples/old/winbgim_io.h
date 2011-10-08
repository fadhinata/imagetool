#ifndef __WINBGIM_IO_H__

#include <winbgim.h>
#include <bitmap.h>
#include <bytemap.h>
#include <dwordmap.h>

void display_read_red_image(bytemap_t *p, void *image);
void display_read_green_image(bytemap_t *p, void *image);
void display_read_blue_image(bytemap_t *p, void *image);
void display_read_rgb_image(bytemap_t *r, bytemap_t *g, bytemap_t *b, void *image);

// for mono image
void display_write_mono_image(void *image, int x, int y, bitmap_t *p);
void display_write_mono_image_at_red_part(void *image, int x, int y, bitmap_t *p);
void display_write_mono_image_at_green_part(void *image, int x, int y, bitmap_t *p);
void display_write_mono_image_at_blue_part(void *image, int x, int y, bitmap_t *p);
// for gray image
void display_clear_all_parts(void *image);
void display_write_gray_image(void *image, int x, int y, bytemap_t *p);
void display_clear_red_part(void *image);
void display_write_red_image(void *image, int x, int y, bytemap_t *p);
void display_clear_green_part(void *image);
void display_write_green_image(void *image, int x, int y, bytemap_t *p);
void display_clear_blue_part(void *image);
void display_write_blue_image(void *image, int x, int y, bytemap_t *p);

void write_label_image(void *image, dwordmap_t *p);
void write_rgb_image(void *image, dwordmap_t *p);
void write_mask_image(void *image, bitmap_t *p);


#endif /* __WINBGIM_IO_H__ */
