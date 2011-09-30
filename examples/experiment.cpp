#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <assert.h>

#define NR_SAMPLE 11
#define NR_IMAGE 24
#define REALLOC_LEN 50

#define NORMAL1 0
#define NORMAL2 1
#define TOP1    2
#define TOP2    3
#define TOP3    4
#define BOTTOM1 5
#define BOTTOM2 6
#define SCAB1   7
#define SCAB2   8
#define BRUISE1 9
#define BRUISE2 10

static const char *sample_names[NR_SAMPLE] = {
	"Sound Without Glaring",
	"Sound Under Glaring Region",
	"Top With Cross Section",
	"Top With Branch",
	"Top Under Funnel Region",
	"Bottom With Strong Darkness",
	"Bottom Neiborhood",
	"Scab Brown",
	"Scab Black",
	"Bruise Strong",
	"Bruise Doubtfull"
};

static const int wavelength[NR_IMAGE] = {
	530, 550, 570, 590, 610, 630, 650, 700, 720, 740,
	760, 780, 800, 820, 840, 860, 880, 900, 920, 940,
	960, 980, 1000, 1050
};

typedef struct {
	int count;
	int length;
	int **spectrum;
} spectrums_t;

int Experiment(int argc, char *argv[])
{
	FILE *fd;
	int i, j, k, m, n, u;
	int xpos, ypos, values[NR_IMAGE], value;
	int step1_cutup, step1_cutdown;
	int step2_cutup, step2_cutdown;
	int total_count;
	int false_rejection, false_acceptance;
	int true_rejection, true_acceptance;
	char filename[256], applename[256];
	spectrums_t spectrums[NR_SAMPLE];
	int **array;
	
	double r;
	
	double ratio;
	int i_ratio, j_ratio;
	int step1_cutup_ratio, step1_cutdown_ratio;
	int step2_cutup_ratio, step2_cutdown_ratio;
	
	double max_ratio;
	int i_max_ratio, j_max_ratio;
	int step1_cutup_max_ratio, step1_cutdown_max_ratio;
	int step2_cutup_max_ratio, step2_cutdown_max_ratio;
	
	for (m = 0; m < NR_SAMPLE; m++) {
		//spectrums[m];
		sprintf(filename, "%s.txt", sample_names[m]);
		printf("read filename(%s) : ", filename);

		spectrums[m].count = 0;
		spectrums[m].length = REALLOC_LEN;
		spectrums[m].spectrum = (int **)malloc((spectrums[m].length)*sizeof(int *));
		assert(spectrums[m].spectrum);
		for (i = 0; i < spectrums[m].length; i++) {
			*(spectrums[m].spectrum+i) = (int *)malloc(NR_IMAGE*sizeof(int));
			assert(*(spectrums[m].spectrum+i));
		}
		fd = fopen(filename, "rb");
		while (0 < fscanf(fd, "%s %d %d ", applename, &xpos, &ypos)) {
			for (u = 0; u < NR_IMAGE; u++) {
				fscanf(fd, "%d ", (*(spectrums[m].spectrum+spectrums[m].count)+u));
			}
			spectrums[m].count++;
			if (spectrums[m].count >= spectrums[m].length) {
				//spectrums[m].length += REALLOC_LEN;
				array = (int **)malloc((spectrums[m].length+REALLOC_LEN)*sizeof(int *));
				assert(array);
				for (i = 0; i < (spectrums[m].length+REALLOC_LEN); i++) {
					*(array+i) = (int *)malloc(NR_IMAGE*sizeof(int));
					assert(*(array+i));
				}
				for (i = 0; i < spectrums[m].length; i++) {
					memcpy(*(array+i), *(spectrums[m].spectrum+i), NR_IMAGE*sizeof(int));
					free(*(spectrums[m].spectrum+i));
				}
				free(spectrums[m].spectrum);
				spectrums[m].spectrum = array;
				spectrums[m].length += REALLOC_LEN;
			}
		}
		fclose(fd);
		printf("count(%d)\n", spectrums[m].count);
	}
	
	// for the scab
	// image subtraction
	max_ratio = 0.0;
	for (i = 0; i < NR_IMAGE-1; i++) {
		for (j = i+1; j < NR_IMAGE; j++) {
			ratio = 0.0;
			printf("i(%d)-j(%d)\n", i, j);
			for (step1_cutup = -255; step1_cutup <= 256; step1_cutup++) {
				for (step1_cutdown = -256; step1_cutdown <= 255; step1_cutdown++) {
					if (step1_cutup == step1_cutdown) continue;
					total_count = 0;
					false_rejection = false_acceptance = 0;
					true_rejection = true_acceptance = 0;
					for (m = 0; m < NR_SAMPLE; m++) {
						for (n = 0; n < spectrums[m].count; n++) {
							total_count++;
							value = (*(*(spectrums[m].spectrum+n)+i))-(*(*(spectrums[m].spectrum+n)+j));
							/* image subtraction and threshold for scab */
							if ((m == SCAB1) || (m == SCAB2)) {
								if (((step1_cutdown > step1_cutup) && (value <= step1_cutdown) && (value >= step1_cutup)) ||
									((step1_cutdown < step1_cutup) && ((value <= step1_cutdown) || (value >= step1_cutup)))) {
									true_acceptance++;
								} else {
									true_rejection++;
								}
							} else {
								if (((step1_cutdown > step1_cutup) && (value <= step1_cutdown) && (value >= step1_cutup)) ||
									((step1_cutdown < step1_cutup) && ((value <= step1_cutdown) || (value >= step1_cutup)))) {
									false_acceptance++;
								} else {
									false_rejection++;
								}
							}
						}
					}
					r = (double)(true_acceptance+false_rejection)/(double)total_count;
					if (r > ratio) {
						i_ratio = i;
						j_ratio = j;
						step1_cutup_ratio = step1_cutup;
						step1_cutdown_ratio = step1_cutdown;
						ratio = r;
					}
				}
			}
			fd = fopen("ratio_for_scab.txt", "a");
			fprintf(fd, "%dnm-%dnm : %lf cutup(%d) cutdown(%d)\n",
				wavelength[i_ratio], wavelength[j_ratio],
				ratio, step1_cutup_ratio, step1_cutdown_ratio);
			fclose(fd);
			
			if (ratio > max_ratio) {
				i_max_ratio = i_ratio;
				j_max_ratio = j_ratio;
				step1_cutup_max_ratio = step1_cutup_ratio;
				step1_cutdown_max_ratio = step1_cutdown_ratio;
				max_ratio = ratio;
			}
		}
	}
	fd = fopen("ratio_for_scab.txt", "a");
	fprintf(fd, "\n%dnm-%dnm : %lf cutup(%d) cutdown(%d)\n",
		wavelength[i_max_ratio], wavelength[j_max_ratio],
		max_ratio, step1_cutup_max_ratio, step1_cutdown_max_ratio);
	fclose(fd);

	// for the bruise
	// image subtraction
	max_ratio = 0.0;
	for (i = 0; i < NR_IMAGE-1; i++) {
		for (j = i+1; j < NR_IMAGE; j++) {
			ratio = 0.0;
			printf("i(%d)-j(%d)\n", i, j);
			for (step1_cutup = -255; step1_cutup <= 256; step1_cutup++) {
				for (step1_cutdown = -256; step1_cutdown <= 255; step1_cutdown++) {
					if (step1_cutup == step1_cutdown) continue;
					total_count = 0;
					false_rejection = false_acceptance = 0;
					true_rejection = true_acceptance = 0;
					for (m = 0; m < NR_SAMPLE; m++) {
						for (n = 0; n < spectrums[m].count; n++) {
							total_count++;
							value = (*(*(spectrums[m].spectrum+n)+i))-(*(*(spectrums[m].spectrum+n)+j));
							/* image subtraction and threshold for bruise */
							if ((m == BRUISE1) || (m == BRUISE2)) {
								if (((step1_cutdown > step1_cutup) && (value <= step1_cutdown) && (value >= step1_cutup)) ||
									((step1_cutdown < step1_cutup) && ((value <= step1_cutdown) || (value >= step1_cutup)))) {
									true_acceptance++;
								} else {
									true_rejection++;
								}
							} else {
								if (((step1_cutdown > step1_cutup) && (value <= step1_cutdown) && (value >= step1_cutup)) ||
									((step1_cutdown < step1_cutup) && ((value <= step1_cutdown) || (value >= step1_cutup)))) {
									false_acceptance++;
								} else {
									false_rejection++;
								}
							}
						}
					}
					r = (double)(true_acceptance+false_rejection)/(double)total_count;
					if (r > ratio) {
						i_ratio = i;
						j_ratio = j;
						step1_cutup_ratio = step1_cutup;
						step1_cutdown_ratio = step1_cutdown;
						ratio = r;
					}
				}
			}
			fd = fopen("ratio_for_bruise.txt", "a");
			fprintf(fd, "%dnm-%dnm : %lf cutup(%d) cutdown(%d)\n",
				wavelength[i_ratio], wavelength[j_ratio],
				ratio, step1_cutup_ratio, step1_cutdown_ratio);
			fclose(fd);
			
			if (ratio > max_ratio) {
				i_max_ratio = i_ratio;
				j_max_ratio = j_ratio;
				step1_cutup_max_ratio = step1_cutup_ratio;
				step1_cutdown_max_ratio = step1_cutdown_ratio;
				max_ratio = ratio;
			}
		}
	}
	fd = fopen("ratio_for_bruise.txt", "a");
	fprintf(fd, "\n%dnm-%dnm : %lf cutup(%d) cutdown(%d)\n",
		wavelength[i_max_ratio], wavelength[j_max_ratio],
		max_ratio, step1_cutup_max_ratio, step1_cutdown_max_ratio);
	fclose(fd);
	
	return 0;
}
