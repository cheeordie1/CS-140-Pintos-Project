#ifndef FIXED_POINT_H
#define FIXED_POINT_H

#include <stdint.h>

/* Library for making fixed-point calculations */
#define F_CONV (1 << 14)

typedef int32_t fp;

static inline fp conv_itofp (int n) { return n * F_CONV; }
static inline int rnconv_fptoi (fp x) {
  return x >= 0 ? (x + F_CONV / 2) / F_CONV : (x - F_CONV / 2) / F_CONV;
}
static inline int rzconv_fptoi (fp x) { return (x / F_CONV); }

static inline fp add_fpfp (fp x, fp y) { return x + y; }
static inline fp add_fpn (fp x, int n) { return add_fpfp (x, conv_itofp (n)); }
static inline fp sub_fpfp (fp x, fp y) { return x - y; } 
static inline fp sub_fpn (fp x, int n) { return sub_fpfp (x, conv_itofp (n)); }

static inline fp mult_fpfp (fp x, fp y) { return ((int64_t)x) * y / F_CONV; }
static inline fp mult_fpn (fp x, int n) { return mult_fpfp (x, conv_itofp (n)); }
static inline fp div_fpfp (fp x, fp y) { return ((int64_t)x) * F_CONV / y; }
static inline fp div_fpn (fp x, int n) { return div_fpfp (x, conv_itofp (n)); }

#endif /* threads/fixed-point.h */
