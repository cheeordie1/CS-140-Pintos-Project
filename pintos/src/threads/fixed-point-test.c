#include "fixed-point.h"
#include <stdint.h>
#include <stdio.h>

<<<<<<< HEAD
#define DECMASK (~(0xffffffff << 14))

int get_int (fp x);
int get_dec (fp x);
=======
#define INTMASK ((2**17) | (0x0000ffff << 14))
#define DECMASK ((2**14) | (2**13) | 0x00000fff)
>>>>>>> 94de177f143cfa51140994a8655c88f99410d90e

int t_1 = 50;
int t_2 = 25;
int t_3 = 3;
int t_4 = -50;
int t_5 = -25;
int t_6 = -6;

int
get_int (fp x)
{
<<<<<<< HEAD
return x >> 14;
=======
  return x < 0 ? -((int)((x & INTMASK) >> 14)) : ((int)((x & INTMASK) >> 14));
>>>>>>> 94de177f143cfa51140994a8655c88f99410d90e
}

int
get_dec (fp x)
{
<<<<<<< HEAD
return x & DECMASK; /* This does not read the decimal back correctly, but everything works */
=======
  return x < 0 ? -((int)(x & DECMASK)) : (int)(x & DECMASK);
>>>>>>> 94de177f143cfa51140994a8655c88f99410d90e
}

int
main ()
{
  fp c_1 = conv_itofp (t_1);
  fp c_2 = conv_itofp (t_2);
  fp c_3 = conv_itofp (t_3);
  fp c_4 = conv_itofp (t_4);
  fp c_5 = conv_itofp (t_5);
  fp c_6 = conv_itofp (t_6);

  fp aff_12 = add_fpfp (c_1, c_2);
  fp aff_14 = add_fpfp (c_1, c_4);
  fp aff_45 = add_fpfp (c_4, c_5);
  
  printf("\n%d.%d + %d.%d = %d.%d\n", get_int(c_1), get_dec(c_1), get_int(c_2), get_dec(c_2), get_int(aff_12), get_dec(aff_12));
  printf("%d.%d + %d.%d = %d.%d\n", get_int(c_1), get_dec(c_1), get_int(c_4), get_dec(c_4), get_int(aff_14), get_dec(aff_14));
  printf("%d.%d + %d.%d = %d.%d\n\n", get_int(c_4), get_dec(c_4), get_int(c_5), get_dec(c_5), get_int(aff_45), get_dec(aff_45));

  fp sff_12 = sub_fpfp (c_1, c_2);
  fp sff_14 = sub_fpfp (c_1, c_4);
  fp sff_45 = sub_fpfp (c_4, c_5);

  printf("%d.%d - %d.%d = %d.%d\n", get_int(c_1), get_dec(c_1), get_int(c_2), get_dec(c_2), get_int(sff_12), get_dec(sff_12));
  printf("%d.%d - %d.%d = %d.%d\n", get_int(c_1), get_dec(c_1), get_int(c_4), get_dec(c_4), get_int(sff_14), get_dec(sff_14));
  printf("%d.%d - %d.%d = %d.%d\n\n", get_int(c_4), get_dec(c_4), get_int(c_5), get_dec(c_5), get_int(sff_45), get_dec(sff_45));

  fp mff_12 = mult_fpfp (c_1, c_2);
  fp mff_14 = mult_fpfp (c_1, c_4);
  fp mff_45 = mult_fpfp (c_4, c_5);

  printf("%d.%d * %d.%d = %d.%d\n", get_int(c_1), get_dec(c_1), get_int(c_2), get_dec(c_2), get_int(mff_12), get_dec(mff_12));
  printf("%d.%d * %d.%d = %d.%d\n", get_int(c_1), get_dec(c_1), get_int(c_4), get_dec(c_4), get_int(mff_14), get_dec(mff_14));
  printf("%d.%d * %d.%d = %d.%d\n\n", get_int(c_4), get_dec(c_4), get_int(c_5), get_dec(c_5), get_int(mff_45), get_dec(mff_45));

  fp dff_12 = div_fpfp (c_1, c_2);
  fp dff_14 = div_fpfp (c_1, c_4);
  fp dff_45 = div_fpfp (c_4, c_5);

  printf("%d.%d / %d.%d = %d.%d\n", get_int(c_1), get_dec(c_1), get_int(c_2), get_dec(c_2), get_int(dff_12), get_dec(dff_12));
  printf("%d.%d / %d.%d = %d.%d\n", get_int(c_1), get_dec(c_1), get_int(c_4), get_dec(c_4), get_int(dff_14), get_dec(dff_14));
  printf("%d.%d / %d.%d = %d.%d\n\n", get_int(c_4), get_dec(c_4), get_int(c_5), get_dec(c_5), get_int(dff_45), get_dec(dff_45));

  fp dff_13 = div_fpfp (c_1, c_3);
  fp dff_16 = div_fpfp (c_1, c_6);
  fp dff_46 = div_fpfp (c_4, c_6);

  printf("%d.%d / %d.%d = %d.%d\n", get_int(c_1), get_dec(c_1), get_int(c_3), get_dec(c_3), get_int(dff_13), get_dec(dff_13));
  printf("%d.%d / %d.%d = %d.%d\n", get_int(c_1), get_dec(c_1), get_int(c_6), get_dec(c_6), get_int(dff_16), get_dec(dff_16));
  printf("%d.%d / %d.%d = %d.%d\n\n", get_int(c_4), get_dec(c_4), get_int(c_6), get_dec(c_6), get_int(dff_46), get_dec(dff_46));

  fp mff_s1 = mult_fpfp (dff_13, c_3);
  fp mff_s2 = mult_fpfp (dff_16, c_6);
  fp mff_s3 = mult_fpfp (dff_46, c_6);

  printf("%d.%d * %d.%d = %d.%d\n", get_int(dff_13), get_dec(dff_13), get_int(c_3), get_dec(c_3), get_int(mff_s1), get_dec(mff_s1));
  printf("%d.%d * %d.%d = %d.%d\n", get_int(dff_16), get_dec(dff_16), get_int(c_6), get_dec(c_6), get_int(mff_s2), get_dec(mff_s2));
  printf("%d.%d * %d.%d = %d.%d\n\n", get_int(dff_46), get_dec(dff_46), get_int(c_6), get_dec(c_6), get_int(mff_s3), get_dec(mff_s3));


  int rnconv_13 = rnconv_fptoi(dff_13);
  int rnconv_16 = rnconv_fptoi(dff_16);
  int rnconv_46 = rnconv_fptoi(dff_46);
  
  int rzconv_13 = rzconv_fptoi(dff_13);
  int rzconv_16 = rzconv_fptoi(dff_16);
  int rzconv_46 = rzconv_fptoi(dff_46);

  printf("%d.%d rounded to the nearest int is %d\n", get_int(dff_13), get_dec(dff_13), rnconv_13);  
  printf("%d.%d rounded to the nearest int is %d\n", get_int(dff_16), get_dec(dff_16), rnconv_16);  
  printf("%d.%d rounded to the nearest int is %d\n", get_int(dff_46), get_dec(dff_46), rnconv_46); 

  printf("%d.%d rounded to zero is %d\n", get_int(dff_13), get_dec(dff_13), rzconv_13);  
  printf("%d.%d rounded to zero is %d\n", get_int(dff_16), get_dec(dff_16), rzconv_16);  
  printf("%d.%d rounded to zero is %d\n\n", get_int(dff_46), get_dec(dff_46), rzconv_46);  

  return 0;
}
