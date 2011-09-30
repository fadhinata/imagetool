#ifndef __FLOATMAP_H__
#define __FLOATMAP_H__

#include <maphdr.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct {
    maphdr_t header;
    float *buffer;
  } floatmap_t;

  floatmap_t *floatmap_new(int w, int h);
  void floatmap_destroy(floatmap_t *m);
  floatmap_t *floatmap_clone(void *p);

#define floatmap_get_width(m) ((m)->header.width)
#define floatmap_get_height(m) ((m)->header.height)
#define floatmap_get_pitch(m) ((m)->header.pitch)
#define floatmap_get_buffer(m) ((m)->buffer)
#define floatmap_get_value(x, y, m) *((float *)((m)->buffer + (y) * (m)->header.pitch / sizeof(float) + (x)))
#define floatmap_put_value(v, x, y, m) (*((float *)((m)->buffer + (y) * (m)->header.pitch / sizeof(float) + (x))) = (v))

#ifdef __cplusplus
}
#endif

#endif /* __FLOATMAP_H__ */
