#include <malloc.h>
#include <string.h>
#include <assert.h>
#include <floatmap.h>

floatmap_t *floatmap_new(int w, int h)
{
  int pitch = WIDTHBYTES(w*sizeof(float)*8);
  floatmap_t *m;

  m = (floatmap_t *)malloc(sizeof(*m)+h*pitch);
  assert(m);
  if (m == NULL) goto __error_return__;
  m->header.width = w;
  m->header.height = h;
  m->header.pitch = pitch;
  m->buffer = (float *)((unsigned char *)m+sizeof(*m));
  memset(m->buffer, 0, h*pitch);
  return m;
 __error_return__:;
  return NULL;
}

void floatmap_destroy(floatmap_t *m)
{
  free(m);
}

floatmap_t *floatmap_clone(void *p)
{
  return floatmap_new(((maphdr_t *)p)->width, ((maphdr_t *)p)->height);
}
