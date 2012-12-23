#ifndef __EXAMPLES_H__
#define __EXAMPLES_H__

void test_distance_transform_and_watershed(int w, int h);
int carrot_process(int argc, char *argv[]);
int tricam_process(int argc, char *argv[]);
int tricam_diff(int argc, char *argv[]);
int tricam_diff_div(int argc, char *argv[]);
int tricam_morphology(int argc, char *argv[]);
int tricam_labeling(int argc, char *argv[]);
int tricam_display(int argc, char *argv[]);
int tricam_citrus(int argc, char *argv[]);
int tricam_apple_sub_div(int argc, char *argv[]);
int tricam_apple_sub(int argc, char *argv[]);
int image_downsize(int argc, char *argv[]);
void matrix_test(void);

int FLIR_PM595_viewer(int argc, char *argv[]);

#endif /* __EXAMPLES_H__ */
