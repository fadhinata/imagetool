#include <malloc.h>
#include <string.h>
#undef NDEBUG
#include <assert.h>
#include <doublemap.h>

doublemap_t *doublemap_create(int w, int h)
{
    int pitch = WIDTHBYTES(w*sizeof(double)*8);
    doublemap_t *m;

    m = (doublemap_t *)malloc(sizeof(*m)+h*pitch);
    assert(m);
    if (m == NULL) goto __error_return__;
    m->header.width = w;
    m->header.height = h;
    m->header.pitch = pitch;
    m->buffer = (double *)((unsigned char *)m+sizeof(*m));
    memset(m->buffer, 0, h*pitch);
    return m;
__error_return__:;
    return NULL;
}

void doublemap_delete(doublemap_t *m)
{
    free(m);
}

doublemap_t *doublemap_clone(void *p)
{
    return doublemap_create(((maphdr_t *)p)->width, ((maphdr_t *)p)->height);
}
