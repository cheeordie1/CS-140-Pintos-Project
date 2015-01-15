#include "fixed-point.h"

#define INTMASK ((2**17) | (0x0000ffff << 14))
#define DECMASK ((2**14) | (2**13) | 0x00000fff)

int t_1 = 50;
int t_2 = 25;
int t_3 = 3;

int
get_int (fp x)
{
  return x < 0 ? -((int)((x & INTMASK) >> 14)) : ((int)((x & INTMASK) >> 14));
}

int
get_dec (fp x)
{
  return x < 0 ? -((int)(x & DECMASK)) : (int)(x & DECMASK);
}

int 
main (int argc, char **argv)
{
  
}
