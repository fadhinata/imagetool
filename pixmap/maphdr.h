#ifndef __MAPHDR_H__
#define __MAPHDR_H__

#define WIDTHBYTES(bits) (((bits) + 31) / 32 * 4)

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  int width;
  int height;
  int pitch;
} maphdr_t;

#ifdef __cplusplus
}
#endif

#endif /* __MAPHDR_H__ */
