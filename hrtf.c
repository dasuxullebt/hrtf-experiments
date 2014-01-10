/* compile with gcc -c -g -o hrtf.o hrtf.c */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <assert.h>
#include <fcntl.h>

#include "hrtf.h"

/* load and use MIT HRTF data. this is highly specific and must
   probably be adapted for other hrtf databases. */
static const float hrtf_incs[14] = {6.43, 6, 5, 5, 5, 5, 5, 6, 6.43, 8, 10, 15, 30, 0};
static const int hrtf_nums[14] = {29, 31, 37, 37, 37, 37, 37, 31, 29, 23, 19, 13, 7, 1};
static float** measurements[14];

#define HRTF_PATHS "compact/elev%d/H%de%03da.dat"

float* nearest (int el, int azim, bool* flip) {

  assert(azim <= 180);
  assert(azim >= -180);

  /* maybe flip? */
  if (azim < 0) {
    azim = -azim;
    *flip = true;
  } else {
    *flip = false;
  }

  assert(el >= -40);
  assert(el <= 90);

  /* nearest elevation */
  int n_el = (el / 10) + 4;

  int n_azim;

  /* nearest azimuth */
  if (n_el < 13) {
    n_azim = -1;
    double dist = 9999;
    int i;
    for (i = 0; i < hrtf_nums[n_el]; ++i) {
      double ndist = abs((i * hrtf_incs[n_el]) - azim);
      if (dist > ndist) {
	n_azim = i;
	dist = ndist;
      }
    }
    if (n_azim < 0) {
      fprintf(stderr, "n_azim\n");
      exit(EXIT_FAILURE);
    }
  } else {
    n_azim = 0;
  }

  return measurements[n_el][n_azim];
}

void hrtf_free_measurements () {
  int i, j;
  for (i = 0; i < 14; ++i) {
    for (j = 0; j < hrtf_nums[i]; ++j) {
      free(measurements[i][j]);
    }
    free(measurements[i]);
  }
}

void hrtf_load_measurements () {
  int i, j, k;
  for (i = 0; i < 14; ++i) {
    measurements[i] = malloc(hrtf_nums[i]*sizeof(float*));
    if (! measurements[i]) {
      fprintf(stderr, "Error allocating measurements[%d]\n", i);
      exit(EXIT_FAILURE);
    }
    for (j = 0; j < hrtf_nums[i]; ++j) {
      measurements[i][j] = malloc(256*sizeof(float));
      if (!measurements[i][j]) {
	fprintf(stderr, "Error allocating measurements[%d][%d]\n", i, j);
	exit(EXIT_FAILURE);
      }

      char fname[1024];
      uint16_t input[256];
      int elev = (i - 4) * 10;
      int az = round(j * hrtf_incs[i]);
      int pf = snprintf(fname, sizeof(fname), HRTF_PATHS, elev, elev, az);
      if (pf >= sizeof(fname)) {
	fprintf(stderr,
		"Bug in hrtf_load_measurements: fname too small when trying to open"
		"elev=%d az=%d, snprintf returned %d!\n", elev, az, pf);
	exit(EXIT_FAILURE);
      } else if (pf < 0) {
	fprintf(stderr,
		"In hrtf_load_measurements: snprintf=%d for elev=%d az=%d\n",
		pf, elev, az);
	exit(EXIT_FAILURE);
      }

      int fd = open(fname, O_RDONLY);
      if (fd < 0) { perror(fname); exit(EXIT_FAILURE); }
      int rd = read(fd, input, sizeof(input));
      if (rd < 0) { perror(fname); exit(EXIT_FAILURE); }
      else if (rd < sizeof(input)) {
	/* too lazy to do this right, it will probably work anyway */
	fprintf(stderr, "Unsuccessful read for measurements[%d][%d]\n", i, j);
	exit(EXIT_FAILURE);
      }
      fd = close(fd);
      if (fd < 0) {
	perror(fname);
	exit(EXIT_FAILURE);
      }
      for (k = 0; k < 256; ++k) {
	measurements[i][j][k] = ((float) ((int16_t) be16toh(input[k]))) / ((float) INT16_MAX);
      }
    }
  }
}

void slow_ir_apply_context_init (struct slow_ir_apply_context* ctx, int nfir, float* fir, bool flip) {
  ctx->nfir = nfir;
  ctx->fir = fir;
  ctx->flip = flip;
  ctx->backlog_index = 0;
  ctx->backlog = calloc(nfir,sizeof(float));
}

void slow_ir_apply_context_destroy (struct slow_ir_apply_context* ctx) {
  free(ctx->backlog);
}
