/* compile firstly gcc -c -g -o hrtf.o hrtf.c
   then gcc -g -o example example.c hrtf.o -lm

   Under Linux, try it, for example, using
   yes 123456789012345678901234567890 | ./example -30 -90 | aplay -f U8 -c2 -r44100
*/

#include <stdio.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include "hrtf.h"

static const int samplerate = 44100;

int main (int argc, char** argv) {
  if (argc < 3) {
    fprintf(stderr, "Usage: %s elev azim\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  int elev, azim;

  if (sscanf(argv[1], "%d", &elev) < 1) {
    perror("First Argument is not a number.");
    exit(EXIT_FAILURE);
  }
  if (sscanf(argv[2], "%d", &azim) < 1) {
    perror("Second Argument is not a number.");
    exit(EXIT_FAILURE);
  }

  hrtf_load_measurements();

  bool flip;
  struct slow_ir_apply_context cx;
  float* fir = nearest(elev, azim, &flip);

  slow_ir_apply_context_init(&cx, 256, fir, flip);

  /* we're using a getchar()-loop here: it is easier to understand. in
     production code, it should not be done that way. */

  int c;
  while ((c = getchar()) != EOF) {
    /* we read unsigned 8 bit at 44100 samplerate - stupid, but easy */
    float in = ((float) (c - 127)) / 128.f;
    float out[2];
    slow_ir_apply(&cx, in, out);
    int i;
    for (i = 0; i < 2; ++i)
      putchar(127+(INT8_MAX * out[i]));
  }

  slow_ir_apply_context_destroy(&cx);  

  hrtf_free_measurements();
}
