#ifndef __HRTF_H
#define __HRTF_H

/* requires -lm */

#include <stdbool.h>

struct slow_ir_apply_context {
  int nfir;
  float* fir;
  bool flip;
  int backlog_index;
  float* backlog;
};
/* This structure is the context ("continuation", sort of) for
   slow_ir_apply_context */

void hrtf_load_measurements ();
/* call this function before doing anything else. */

void hrtf_free_measurements ();
/* after the database is not needed anymore, call this function */

float* nearest (int el, int azim, bool* flip);
/* get the nearest approximation of "elevation" and "azimuth". The
   boolean "flip" tells whether the left and right channel must be
   flipped. Do not modify the returned value. It is a pointer to the
   structures created by hrtf_load_measurements. If you want to modify
   it, copy it first. */

void slow_ir_apply_context_init (struct slow_ir_apply_context* ctx, int nfir, float* fir, bool flip);
/* initialize the slow_ir_apply_context with <nfir> samples of the
   finite impulse response <fir>. <flip> indicates whether the
   channels should be switched. <nfir> is usually 256. */

void slow_ir_apply_context_destroy (struct slow_ir_apply_context* ctx);

inline static void slow_ir_apply (struct slow_ir_apply_context* c, float sample_in, float* samples_out) {
  /* slow, but doesn't use magic */
  samples_out[0] = samples_out[1] = 0;

  c->backlog[c->backlog_index] = sample_in;
  int j;

  if (c->flip) {
    for (j = 0; j < c->nfir; j+=2) {
      int b = (c->backlog_index + c->nfir - j) % c->nfir;
      samples_out[0] += c->backlog[b] * c->fir[j + 1];
      samples_out[1] += c->backlog[b] * c->fir[j];
    }
  } else {
    for (j = 0; j < c->nfir; j+=2) {
      int b = (c->backlog_index + c->nfir - j) % c->nfir;
      samples_out[0] += c->backlog[b] * c->fir[j];
      samples_out[1] += c->backlog[b] * c->fir[j + 1];
    }
  }

  c->backlog_index = (c->backlog_index + 1) % c->nfir;
}

#endif
