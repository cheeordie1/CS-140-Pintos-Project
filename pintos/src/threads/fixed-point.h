#ifndef FIXED_POINT_H
#define FIXED_POINT_H

/* Library for making fixed-point calculations */
#define f 2**14
#define SIGNEDBITMASK 0x80000000

typedef int32_t fp;

static inline fp conv_itofp (int n) { 
  return n > 0 ? (~SIGNEDBITMASK) & (n * f) : SIGNEDBITMASK | (n * f);
}
static inline int rncont_fptoi (fp x) {
  return x > 0 ? (int)(x + (f / 2)) / f : -((int)((((~SIGNEDBITMASK) & x) + (f / 2)) / f));
}
static inline int rzcont_fptoi (fp x) {
  return x > 0 ? x / f : -(((~SIGNEDBITMASK) & x) / f);
}

static inline fp add_fpfp (fp x, fp y) { return x + y; }
static inline fp add_fpn (fp x, int n) { return add_fpfp (x, conv_itofp (n)); }
static inline fp sub_fpfp (fp x, fp y) { return x - y; } 
static inline fp sub_fpn (fp x, int n) { return sub_fpfp (x, conv_itofp (n)); }

static inline fp mult_fpfp (fp x, fp y) { return ((int64_t)x) * y / f; }
static inline fp mult_fpn (fp x, int n) { return mult_fpfp (x, conv_itofp (n)); }
static inline div_fpfp (fp x, fp y) { return ((int64_t)x) * f / y }
static inline div_fpn (fp x, int n) { return div_fpfp (x, conv_itofp (n)); }

#endif /* threads/fixed-point.h */
