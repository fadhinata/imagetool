
#include <complex2.h>
#include <vector.h>
#include <point_list.h>
#include <dct.h>

// Fourier descriptor
int test_fourier_descriptor(vector_t *fd, point_list_t *plist)
{
  int i, n;
  vector_t *time, *fourier, *fd;
  complex_t comp;

  assert(fd);
  assert(plist);
  assert(vecter_get_length(fd) == point_list_get_count(plist));

  time = cvector_new_and_copy_point_list(plist);
  fourier = vector_new(vector_get_length(time), true);
  //fd = vector_new(vector_get_length(time), true);

  dft(fourier, time);
  n = vector_get_length(time);

  // filtering
  /*
  comp.real = comp.imag = 0.0;
  for (i = n / 2 - 16; i < n / 2 + 16; i++) {
    cvector_put_value(comp, i, fourier);
  }
  */

  ///////////////////////  Normalization ////////////////////////////
  vector_copy(fd, fourier);
  // For translation invariance, 0 frequency must have a zero value
  cvector_put_value(comp, 0, fd);

  // For scale invariance,
  // fourier descriptor must be divied by the absolute of 1 frequency component.
  cvector_read_value(&comp, 1, fd);
  val = complex_get_abs(&comp);
  vector_divide_scalar(fd, val);
  ivector_divide_scalar(fd, val);

  // For rotating and changes in starting point
  // remove all phase information and
  // consider only absolute values of the descriptor element
  //f = fopen("fourier_descriptor.txt", "a+");
  //fprintf(f, "%s", fn);
  for (i = 0; i < vector_get_length(fd); i++) {
    cvector_read_value(&comp, i, fd);
    val = complex_get_abs(&comp);
    vector_put_value(val, i, fd);
    ivector_put_value(0, i, fd);
    //fprintf(f, ", %lf", val);
  }
  //fprintf(f, "\n");
  //fclose(f);
  ///////////////////////////////////////////////////////////////////

  //  idft(time, fourier);
  return n;
}
