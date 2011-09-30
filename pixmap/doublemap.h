#ifndef __DOUBLEMAP_H__
#define __DOUBLEMAP_H__

#include <maphdr.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    maphdr_t header;
    double *buffer;
} doublemap_t;

doublemap_t *doublemap_create(int w, int h);
void doublemap_delete(doublemap_t *m);
doublemap_t *doublemap_clone(void *p);

#ifdef __cplusplus
}
#endif

#endif /* __DOUBLEMAP_H__ */
