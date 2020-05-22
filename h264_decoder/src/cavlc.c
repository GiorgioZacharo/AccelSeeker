//============================================================================//
//    H.264 High Level Synthesis Benchmark
//    Copyright (c) <2016>
//    <University of Illinois at Urbana-Champaign>
//    All rights reserved.
//
//    Developed by:
//
//    <ES CAD Group>
//        <University of Illinois at Urbana-Champaign>
//        <http://dchen.ece.illinois.edu/>
//
//    <Hardware Research Group>
//        <Advanced Digital Sciences Center>
//        <http://adsc.illinois.edu/>
//============================================================================//

#include "global.h"
#include "vlc.h"
#include <string.h>
const unsigned char lentab[3][4][17] =
{
  {   // 0702
    { 1, 6, 8, 9,10,11,13,13,13,14,14,15,15,16,16,16,16},
    { 0, 2, 6, 8, 9,10,11,13,13,14,14,15,15,15,16,16,16},
    { 0, 0, 3, 7, 8, 9,10,11,13,13,14,14,15,15,16,16,16},
    { 0, 0, 0, 5, 6, 7, 8, 9,10,11,13,14,14,15,15,16,16},
  },
  {
    { 2, 6, 6, 7, 8, 8, 9,11,11,12,12,12,13,13,13,14,14},
    { 0, 2, 5, 6, 6, 7, 8, 9,11,11,12,12,13,13,14,14,14},
    { 0, 0, 3, 6, 6, 7, 8, 9,11,11,12,12,13,13,13,14,14},
    { 0, 0, 0, 4, 4, 5, 6, 6, 7, 9,11,11,12,13,13,13,14},
  },
  {
    { 4, 6, 6, 6, 7, 7, 7, 7, 8, 8, 9, 9, 9,10,10,10,10},
    { 0, 4, 5, 5, 5, 5, 6, 6, 7, 8, 8, 9, 9, 9,10,10,10},
    { 0, 0, 4, 5, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9,10,10,10},
    { 0, 0, 0, 4, 4, 4, 4, 4, 5, 6, 7, 8, 8, 9,10,10,10},
  },

};

const unsigned char codtab[3][4][17] =
{
  {
    { 1, 5, 7, 7, 7, 7,15,11, 8,15,11,15,11,15,11, 7,4},
    { 0, 1, 4, 6, 6, 6, 6,14,10,14,10,14,10, 1,14,10,6},
    { 0, 0, 1, 5, 5, 5, 5, 5,13, 9,13, 9,13, 9,13, 9,5},
    { 0, 0, 0, 3, 3, 4, 4, 4, 4, 4,12,12, 8,12, 8,12,8},
  },
  {
    { 3,11, 7, 7, 7, 4, 7,15,11,15,11, 8,15,11, 7, 9,7},
    { 0, 2, 7,10, 6, 6, 6, 6,14,10,14,10,14,10,11, 8,6},
    { 0, 0, 3, 9, 5, 5, 5, 5,13, 9,13, 9,13, 9, 6,10,5},
    { 0, 0, 0, 5, 4, 6, 8, 4, 4, 4,12, 8,12,12, 8, 1,4},
  },
  {
    {15,15,11, 8,15,11, 9, 8,15,11,15,11, 8,13, 9, 5,1},
    { 0,14,15,12,10, 8,14,10,14,14,10,14,10, 7,12, 8,4},
    { 0, 0,13,14,11, 9,13, 9,13,10,13, 9,13, 9,11, 7,3},
    { 0, 0, 0,12,11,10, 9, 8,13,12,12,12, 8,12,10, 6,2},
  },
};


const unsigned char lentabDC[4][5] =
{
  { 2, 6, 6, 6, 6,},
  { 0, 1, 6, 7, 8,},
  { 0, 0, 3, 7, 8,},
  { 0, 0, 0, 6, 7,},
};

const unsigned char codtabDC[4][5] =
{
  {1,7,4,3,2},
  {0,1,6,3,3},
  {0,0,1,2,2},
  {0,0,0,5,0},
};


const unsigned char tzlentab[15][16] =
{
  { 1,3,3,4,4,5,5,6,6,7,7,8,8,9,9,9},
  { 3,3,3,3,3,4,4,4,4,5,5,6,6,6,6,0},
  { 4,3,3,3,4,4,3,3,4,5,5,6,5,6,0,0},
  { 5,3,4,4,3,3,3,4,3,4,5,5,5,0,0,0},
  { 4,4,4,3,3,3,3,3,4,5,4,5,0,0,0,0},
  { 6,5,3,3,3,3,3,3,4,3,6,0,0,0,0,0},
  { 6,5,3,3,3,2,3,4,3,6,0,0,0,0,0,0},
  { 6,4,5,3,2,2,3,3,6,0,0,0,0,0,0,0},
  { 6,6,4,2,2,3,2,5,0,0,0,0,0,0,0,0},
  { 5,5,3,2,2,2,4,0,0,0,0,0,0,0,0,0},
  { 4,4,3,3,1,3,0,0,0,0,0,0,0,0,0,0},
  { 4,4,2,1,3,0,0,0,0,0,0,0,0,0,0,0},
  { 3,3,1,2,0,0,0,0,0,0,0,0,0,0,0,0},
  { 2,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
  { 1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
};

const unsigned char tzcodtab[15][16] =
{
  {1,3,2,3,2,3,2,3,2,3,2,3,2,3,2,1},
  {7,6,5,4,3,5,4,3,2,3,2,3,2,1,0,0},
  {5,7,6,5,4,3,4,3,2,3,2,1,1,0,0,0},
  {3,7,5,4,6,5,4,3,3,2,2,1,0,0,0,0},
  {5,4,3,7,6,5,4,3,2,1,1,0,0,0,0,0},
  {1,1,7,6,5,4,3,2,1,1,0,0,0,0,0,0},
  {1,1,5,4,3,3,2,1,1,0,0,0,0,0,0,0},
  {1,1,1,3,3,2,2,1,0,0,0,0,0,0,0,0},
  {1,0,1,3,2,1,1,1,0,0,0,0,0,0,0,0},
  {1,0,1,3,2,1,1,0,0,0,0,0,0,0,0,0},
  {0,1,1,2,1,3,0,0,0,0,0,0,0,0,0,0},
  {0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0},
  {0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
};

const unsigned char tzlentabDC[3][4] =
{
  { 1, 2, 3, 3,},
  { 1, 2, 2, 0,},
  { 1, 1, 0, 0,},
};

const unsigned char tzcodtabDC[3][4] =
{
  { 1, 1, 1, 0,},
  { 1, 1, 0, 0,},
  { 1, 0, 0, 0,},
};
const unsigned char rblentab[7][16] =
{
  {1,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {1,2,2,0,0,0,0,0,0,0,0,0,0,0,0},
  {2,2,2,2,0,0,0,0,0,0,0,0,0,0,0},
  {2,2,2,3,3,0,0,0,0,0,0,0,0,0,0},
  {2,2,3,3,3,3,0,0,0,0,0,0,0,0,0},
  {2,3,3,3,3,3,3,0,0,0,0,0,0,0,0},
  {3,3,3,3,3,3,3,4,5,6,7,8,9,10,11},
};

const unsigned char rbcodtab[7][16] =
{
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {1,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {3,2,1,0,0,0,0,0,0,0,0,0,0,0,0},
  {3,2,1,1,0,0,0,0,0,0,0,0,0,0,0},
  {3,2,3,2,1,0,0,0,0,0,0,0,0,0,0},
  {3,0,1,3,2,5,4,0,0,0,0,0,0,0,0},
  {7,6,5,4,3,2,1,1,1,1,1,1,1,1,1},
};

#pragma SDS data mem_attribute (nalu_buf:PHYSICAL_CONTIGUOUS)
#pragma SDS data access_pattern(nalu_buf:RANDOM)
#pragma SDS data zero_copy(nalu_buf[0:MAXNALBUFFERSIZE])
void TrailingOnes_TotalCoeff(NALU_t *nalu, unsigned char *nalu_buf, unsigned long int *nalu_bit_offset,  unsigned char *TotalCoeff, unsigned char *TrailingZeros, unsigned char nC_range)
{
//#pragma HLS PIPELINE

  int i = 0;
  int j = 0;
  int len = 0;
  int cod = 0;
  if(nC_range>3)
  {
    puts("nc_range too large!");
    exit(-1);
  }
  if(nC_range==3)
  {
    cod=u_n(6,nalu, nalu_buf, nalu_bit_offset);
    *TrailingZeros=cod & 3;
    *TotalCoeff= (cod >>2)+1;

    if(*TrailingZeros>*TotalCoeff)
    {
      *TrailingZeros=0;
      *TotalCoeff=0;
    }
    return;
  }

  int a, b, c;
  a = 0;
  b = 0;
  c = 0;
  // load (*nalu_bit_offset) and nalu-buf first, because
  // showbits() does not update them
  int offset = (*nalu_bit_offset);
#if 0
  // Loading 4 consecutive bytes to temp by casting
  unsigned int *temp = (unsigned int*)&nalu_buf[offset / 8];
  unsigned int temp0 = bytes_reverse_32(*temp);
#else
  // Casting does not work well with HLS, this is a workaround
  unsigned char temp_1 = nalu_buf[(offset/8)];
  unsigned char temp_2 = nalu_buf[(offset/8)+1];
  unsigned char temp_3 = nalu_buf[(offset/8)+2];
  unsigned char temp_4 = nalu_buf[(offset/8)+3];

  unsigned int temp = temp_4;
  temp = (temp <<8) | temp_3;
  temp = (temp <<8) | temp_2;
  temp = (temp <<8) | temp_1;
  unsigned int temp0 = bytes_reverse_32(temp);
#endif

  for(j=0;j<4;j++)
  {
    #pragma HLS PIPELINE
    for(i=0;i<17;i++)
    {
      len=lentab[nC_range][j][i];
      cod=codtab[nC_range][j][i];
      unsigned char test = (showbits(len,temp0,offset)==cod);
      a += j * test;
      b += i * test;
      c += len * test;
    }
  }

  *TrailingZeros=a;
  *TotalCoeff=b;
#if _N_HLS_
  fprintf(trace_bit,"%s %*d\t%d\t%d\n","TrailingZeros & TotalCoeff",50-strlen("TrailingZeros & TotalCoeff"),b,a,nC_range);
#endif // _N_HLS_
  (*nalu_bit_offset)+=c;
}

void TrailingOnes_TotalCoeff_ChromaDc(NALU_t *nalu, unsigned char *nalu_buf, unsigned long int *nalu_bit_offset,  unsigned char *TotalCoeff, unsigned char *TrailingZeros)
{
//#pragma HLS PIPELINE

  int i = 0, j = 0;
  int len = 0, cod = 0;
  int a = 0, b = 0, c = 0;
  a = 0;
  b = 0;
  c = 0;
  // load (*nalu_bit_offset) and nalu-buf first, because
  // showbits() does not update them
  int offset = (*nalu_bit_offset);

#if 0
  // Loading 4 consecutive bytes to temp by casting
  unsigned int *temp = (unsigned int*)&nalu_buf[offset / 8];
  unsigned int temp0 = bytes_reverse_32(*temp);
#else
  // Casting does not work well with HLS, this is a workaround
  unsigned char temp_1 = nalu_buf[(offset/8)];
  unsigned char temp_2 = nalu_buf[(offset/8)+1];
  unsigned char temp_3 = nalu_buf[(offset/8)+2];
  unsigned char temp_4 = nalu_buf[(offset/8)+3];

  unsigned int temp = temp_4;
  temp = (temp <<8) | temp_3;
  temp = (temp <<8) | temp_2;
  temp = (temp <<8) | temp_1;
  unsigned int temp0 = bytes_reverse_32(temp);
#endif

  for(i=0;i<4;i++)
  {
    #pragma HLS PIPELINE
    for (j=0;j<5;j++)
    {
      len=lentabDC[i][j];
      cod=codtabDC[i][j];
      unsigned char test = (showbits(len,temp0,offset)==cod);
      a += j * test;
      b += i * test;
      c += len * test;

    }
  }

  *TrailingZeros=b;
  *TotalCoeff=a;

#if _N_HLS_
  fprintf(trace_bit,"%s %*d\t%d\n","TrailingZeros & TotalCoeff",50-strlen("TrailingZeros & TotalCoeff"),b,a);
#endif // _N_HLS_
  (*nalu_bit_offset)+=c;
}

unsigned char total_zeros(NALU_t *nalu, unsigned char *nalu_buf, unsigned long int *nalu_bit_offset,  unsigned char tzVLC)
{
#pragma HLS PIPELINE
  int len = 0, cod = 0;
  int i = 0;
  int a = 0, b = 0;
  a = 0;
  b = 0;
  // load (*nalu_bit_offset) and nalu-buf first, because
  // showbits() does not update them
  int offset = (*nalu_bit_offset);
#if 0
  // Loading 4 consecutive bytes to temp by casting
  unsigned int *temp = (unsigned int*)&nalu_buf[offset / 8];
  unsigned int temp0 = bytes_reverse_32(*temp);
#else
  // Casting does not work well with HLS, this is a workaround
  unsigned char temp_1 = nalu_buf[(offset/8)];
  unsigned char temp_2 = nalu_buf[(offset/8)+1];
  unsigned char temp_3 = nalu_buf[(offset/8)+2];
  unsigned char temp_4 = nalu_buf[(offset/8)+3];

  unsigned int temp = temp_4;
  temp = (temp <<8) | temp_3;
  temp = (temp <<8) | temp_2;
  temp = (temp <<8) | temp_1;
  unsigned int temp0 = bytes_reverse_32(temp);
#endif

  for(i=0;i<15;i++)
  {
    //#pragma HLS PIPELINE
    len = tzlentab[tzVLC-1][i];
    cod = tzcodtab[tzVLC-1][i];
    unsigned char test = (showbits(len,temp0,offset) == cod);
    a += len * test;
    b += i * test;
  }

  (*nalu_bit_offset) += a;
#if _N_HLS_
  fprintf(trace_bit,"%s %*d\n","total_zeros",50-strlen("total_zeros"),b);
#endif // _N_HLS_
  return b;
}

unsigned char total_zeros_DC(NALU_t *nalu, unsigned char *nalu_buf, unsigned long int *nalu_bit_offset,  unsigned char tzVLC)
{
#pragma HLS PIPELINE
  int len = 0, cod = 0;
  int i = 0;
  int a = 0, b = 0;
  a = 0;
  b = 0;
  // load (*nalu_bit_offset) and nalu-buf first, because
  // showbits() does not update them
  int offset = (*nalu_bit_offset);
#if 0
  // Loading 4 consecutive bytes to temp by casting
  unsigned int *temp = (unsigned int*)&nalu_buf[offset / 8];
  unsigned int temp0 = bytes_reverse_32(*temp);
#else
  // Casting does not work well with HLS, this is a workaround
  unsigned char temp_1 = nalu_buf[(offset/8)];
  unsigned char temp_2 = nalu_buf[(offset/8)+1];
  unsigned char temp_3 = nalu_buf[(offset/8)+2];
  unsigned char temp_4 = nalu_buf[(offset/8)+3];

  unsigned int temp = temp_4;
  temp = (temp <<8) | temp_3;
  temp = (temp <<8) | temp_2;
  temp = (temp <<8) | temp_1;
  unsigned int temp0 = bytes_reverse_32(temp);
#endif

  for(i=0;i<4;i++)
  {
    //#pragma HLS PIPELINE
    len = tzlentabDC[tzVLC-1][i];
    cod = tzcodtabDC[tzVLC-1][i];
    unsigned char test = (showbits(len,temp0,offset) == cod);
    a += len * test;
    b += i * test;

  }
  (*nalu_bit_offset) += a;
#if _N_HLS_
  fprintf(trace_bit,"%s %*d\n","total_zeros_DC",50-strlen("total_zeros_DC"),b);
#endif // _N_HLS_
  return b;
}

unsigned char run_before(NALU_t *nalu, unsigned char *nalu_buf, unsigned long int *nalu_bit_offset,  unsigned char tzVLC)
{
#pragma HLS PIPELINE
  int len = 0, cod = 0;
  int i = 0;
  unsigned char tmp = 0;
  if(tzVLC>7)
    tmp=6;
  else
    tmp=tzVLC-1;
  int a, b;
  a = 0;
  b = 0;
  // load (*nalu_bit_offset) and nalu-buf first, because
  // showbits() does not update them
  int offset = (*nalu_bit_offset);
#if 0
  // Loading 4 consecutive bytes to temp by casting
  unsigned int *temp = (unsigned int*)&nalu_buf[offset / 8];
  unsigned int temp0 = bytes_reverse_32(*temp);
#else
  // Casting does not work well with HLS, this is a workaround
  unsigned char temp_1 = nalu_buf[(offset/8)];
  unsigned char temp_2 = nalu_buf[(offset/8)+1];
  unsigned char temp_3 = nalu_buf[(offset/8)+2];
  unsigned char temp_4 = nalu_buf[(offset/8)+3];

  unsigned int temp = temp_4;
  temp = (temp <<8) | temp_3;
  temp = (temp <<8) | temp_2;
  temp = (temp <<8) | temp_1;
  unsigned int temp0 = bytes_reverse_32(temp);
#endif
  for(i=0;i<15;i++)
  {
    //#pragma HLS PIPELINE
    len = rblentab[tmp][i];
    cod = rbcodtab[tmp][i];
    unsigned char test = (showbits(len,temp0,offset) == cod);
    a += len * test;
    b += i * test;

  }
  (*nalu_bit_offset) += a;
#if _N_HLS_
  fprintf(trace_bit,"%s %*d\n","run_before",50-strlen("run_before"),b);
#endif // _N_HLS_
  return b;
}

unsigned char unary_code(NALU_t *nalu, unsigned char *nalu_buf, unsigned long int *nalu_bit_offset)
{
  unsigned char i;
  i=0;
  while(u_1(nalu, nalu_buf, nalu_bit_offset)==0)
  {
    i++;
  }
  return i;
}

#pragma SDS data mem_attribute (nalu_buf:PHYSICAL_CONTIGUOUS)
#pragma SDS data access_pattern(nalu_buf:RANDOM)
#pragma SDS data zero_copy(nalu_buf[0:MAXNALBUFFERSIZE])
unsigned char residual_block_cavlc_16(int coeffLevel[4][4], NALU_t *nalu, unsigned char *nalu_buf, unsigned long int *nalu_bit_offset, int startIdx, int endIdx,int nC)
{
// Disable HLS directives on function parameters when the function is a top-module for SDSoC/HLS.
//#pragma HLS ARRAY_PARTITION variable=coeffLevel complete dim=1
//#pragma HLS ARRAY_PARTITION variable=coeffLevel complete dim=2

  const unsigned char framescan[16][2]=
  {
    {0,0},{1,0},{0,1},{0,2},{1,1},{2,0},{3,0},{2,1},{1,2},{0,3},{1,3},{2,2},{3,1},{3,2},{2,3},{3,3}
  };

  int i = 0;
  unsigned char trailingOnes = 0;
  unsigned char totalcoeff = 0;

  unsigned char nC_range = { 0 };
  unsigned char suffixLength = { 0 };
  unsigned char trailing_ones_sign_flag = { 0 };
  unsigned char level_prefix,level_suffix = { 0 };
  unsigned char SizesuffixLength = { 0 };
  unsigned char zeroLeft = { 0 };
  int levelCode = 0;
  //int level_temp1;
  //int level_temp2;

  int levelVal[16] = { 0 };
#pragma HLS ARRAY_PARTITION variable=levelVal complete dim=1
  unsigned char runVal[16] = { 0 };
#pragma HLS ARRAY_PARTITION variable=runVal complete dim=1
  nC_range=nC/2;
  if(nC_range >3)
    nC_range=3;
  else if (nC_range==3)
    nC_range=2;
  for(i=0;i<16;i++)
  {
    #pragma HLS UNROLL
    coeffLevel[i/4][i%4]=0;
  }

  TrailingOnes_TotalCoeff(nalu, nalu_buf, nalu_bit_offset, &totalcoeff, &trailingOnes, nC_range);


  if(totalcoeff>0)
  {
    suffixLength= (totalcoeff > 10 && trailingOnes <3) ? 1:0;
    //LOOP_OUTER:
	for(i=0;i<totalcoeff;i++)
    {
      if(i<trailingOnes)
      {
        trailing_ones_sign_flag=u_1(nalu, nalu_buf, nalu_bit_offset);
#if _N_HLS_
        fprintf(trace_bit,"%s %*d\n","trailing_ones_sign_flag",50-strlen("trailing_ones_sign_flag"),trailing_ones_sign_flag);
#endif // _N_HLS_
        levelVal[i]=1-2*trailing_ones_sign_flag;
      }
      else
      {

        level_prefix=unary_code(nalu, nalu_buf, nalu_bit_offset);
#if _N_HLS_
        fprintf(trace_bit,"%s %*d\n","level_prefix",50-strlen("level_prefix"),level_prefix);
#endif // _N_HLS_

        levelCode=(level_prefix)<<suffixLength;

         if(suffixLength>0 || level_prefix>=14)
         {
           if(level_prefix==14 && suffixLength == 0)
           {
             SizesuffixLength=4;
           }
           else if(level_prefix>=15)
           {
             SizesuffixLength=level_prefix-3;
           }
           else
           {
             SizesuffixLength=suffixLength;
           }
           level_suffix=u_n(SizesuffixLength,nalu,nalu_buf,nalu_bit_offset);
#if _N_HLS_
           fprintf(trace_bit,"%s %*d\n","level_suffix",50-strlen("level_suffix"),level_suffix);
#endif // _N_HLS_
           levelCode+=level_suffix;
         }

         if(level_prefix>=15 && suffixLength ==0 )
         {
           levelCode+=15;
         }
         if(level_prefix>=16)
         {
           levelCode+=(1<<(level_prefix-3))-4096;
         }
         if(i== trailingOnes && trailingOnes <3)
         {
           levelCode+=2;
         }
         if(levelCode%2==0)
         {
           levelVal[i]=(levelCode+2)>>1;
         }
         else
         {
           levelVal[i]=(-levelCode-1)>>1;
         }

         if(suffixLength==0)
         {
           suffixLength=1;
         }
         if(ABSS(levelVal[i])> (3<<(suffixLength-1)) && suffixLength<6)
         {
           suffixLength++;
         }
       }
     }
     if(totalcoeff<endIdx- startIdx +1)
     {
       zeroLeft=total_zeros(nalu, nalu_buf, nalu_bit_offset, totalcoeff);
     }
     else
       zeroLeft=0;

    //LOOP_INNER1:
	for(i=0;i<totalcoeff-1;i++)
    {
      #pragma HLS PIPELINE
      if(zeroLeft > 0 )
      {
        runVal[i]=run_before(nalu, nalu_buf, nalu_bit_offset, zeroLeft);
      }
      else
      {
        runVal[i]=0;
      }
      zeroLeft=zeroLeft- runVal[i];

    }
    runVal[totalcoeff-1]=zeroLeft;
    int coeffNum=-1;
    //LOOPINNER2:
	for(i=totalcoeff-1;i>=0;i--)
    {
      #pragma HLS PIPELINE
      coeffNum+=runVal[i]+1;
      coeffLevel[framescan[startIdx+coeffNum][0]][framescan[startIdx+coeffNum][1]]=levelVal[i];
    }
  }
  return totalcoeff;
}

unsigned char residual_block_cavlc_4(int coeffLevel[2][2], NALU_t *nalu, unsigned char *nalu_buf, unsigned long int *nalu_bit_offset, int startIdx, int endIdx)
{
#pragma HLS ARRAY_PARTITION variable=coeffLevel complete dim=1
#pragma HLS ARRAY_PARTITION variable=coeffLevel complete dim=2
  int i = 0;
  unsigned char trailingOnes = 0;
  unsigned char totalcoeff = 0;

  unsigned char suffixLength = 0;
  unsigned char trailing_ones_sign_flag = 0;
  unsigned char level_prefix = 0, level_suffix = 0;
  unsigned char SizesuffixLength = 0;
  unsigned char zeroLeft = 0;
  int levelCode = 0;

  int levelVal[4] = { 0 };
  unsigned char runVal[4] = { 0 };
#pragma HLS ARRAY_PARTITION variable=levelVal complete dim=1
#pragma HLS ARRAY_PARTITION variable=runVal complete dim=1

  for(i=0;i<4;i++)
  {
    #pragma HLS UNROLL
    coeffLevel[i/2][i%2]=0;
  }

  TrailingOnes_TotalCoeff_ChromaDc(nalu, nalu_buf, nalu_bit_offset, &totalcoeff, &trailingOnes);
  if(totalcoeff>0)
  {
    suffixLength= (totalcoeff > 10 && trailingOnes <3) ? 1:0;
    for(i=0;i<totalcoeff;i++)
    {
      if(i<trailingOnes)
      {
        trailing_ones_sign_flag=u_1(nalu, nalu_buf, nalu_bit_offset);
#if _N_HLS_
        fprintf(trace_bit,"%s %*d\n","trailing_ones_sign_flag",50-strlen("trailing_ones_sign_flag"),trailing_ones_sign_flag);
#endif // _N_HLS_
        levelVal[i]=1-2*trailing_ones_sign_flag;
      }
      else
      {
        level_prefix=unary_code(nalu, nalu_buf, nalu_bit_offset);
#if _N_HLS_
        fprintf(trace_bit,"%s %*d\n","level_prefix",50-strlen("level_prefix"),level_prefix);
#endif // _N_HLS_

        levelCode=(level_prefix)<<suffixLength;

        if(suffixLength>0 || level_prefix>=14)
        {
          if(level_prefix==14 && suffixLength == 0)
          {
            SizesuffixLength=4;
          }
          else if(level_prefix>=15)
          {
            SizesuffixLength=level_prefix-3;
          }
          else
          {
            SizesuffixLength=suffixLength;
          }
          level_suffix=u_n(SizesuffixLength,nalu, nalu_buf, nalu_bit_offset);
#if _N_HLS_
          fprintf(trace_bit,"%s %*d\n","level_suffix",50-strlen("level_suffix"),level_suffix);
#endif // _N_HLS_
          levelCode+=level_suffix;
        }

        if(level_prefix>=15 && suffixLength ==0 )
        {
          levelCode+=15;
        }
        if(level_prefix>=16)
        {
          levelCode+=(1<<(level_prefix-3))-4096;
        }
        if(i== trailingOnes && trailingOnes <3)
        {
          levelCode+=2;
        }
        if(levelCode%2==0)
        {
          levelVal[i]=(levelCode+2)>>1;
        }
        else
        {
          levelVal[i]=(-levelCode-1)>>1;
        }

        if(suffixLength==0)
        {
          suffixLength=1;
        }
        if(ABSS(levelVal[i])> (3<<(suffixLength-1)) && suffixLength<6)
        {
          suffixLength++;
        }
      }
    }
    if(totalcoeff<endIdx- startIdx +1)
    {
      zeroLeft=total_zeros_DC(nalu, nalu_buf, nalu_bit_offset, totalcoeff);
    }
    else
      zeroLeft=0;

    for(i=0;i<totalcoeff-1;i++)
    {
      if(zeroLeft > 0)
      {
        runVal[i]=run_before(nalu, nalu_buf, nalu_bit_offset, zeroLeft);
      }
      else
      {
        runVal[i]=0;
      }
      zeroLeft=zeroLeft- runVal[i];
    }
    runVal[totalcoeff-1]=zeroLeft;
    int coeffNum=-1;
    for(i=totalcoeff-1;i>=0;i--)
    {
      coeffNum+=runVal[i]+1;
      coeffLevel[ (startIdx+coeffNum)%2][(startIdx+coeffNum)/2]=levelVal[i];
    }
  }
  return totalcoeff;
}



unsigned char nc_Luma
(
 unsigned char residualmode[PicWidthInMBs][FrameHeightInMbs],
 unsigned char nz[PicWidthInMBs*4][FrameHeightInMbs*4],
 int xoff,
 int yoff
 )
{
  unsigned char nA=0;
  unsigned char nB=0;
  unsigned char nC=0;

  if (xoff>0)
  {
    nA=(residualmode[(xoff-1)/4][yoff/4]==IPCM16x16)*16+(residualmode[(xoff-1)/4][yoff/4]!=INTERSKIP)*nz[xoff-1][yoff];
  }
  if (yoff>0)
  {
    nB=(residualmode[xoff/4][(yoff-1)/4]==IPCM16x16)*16+(residualmode[xoff/4][(yoff-1)/4]!=INTERSKIP)*nz[xoff][yoff-1];
  }

  nC=(nA+nB+(yoff>0)*(xoff>0))>>((yoff>0)*(xoff>0));

  return nC;
}


unsigned char nc_Chroma
(
 unsigned char residualmode[PicWidthInMBs][FrameHeightInMbs],
 unsigned char nzC[PicWidthInMBs*2][FrameHeightInMbs*2],
 int xoff,
 int yoff
 )
{
  unsigned char nA=0;
  unsigned char nB=0;
  unsigned char nC=0;

  if (xoff>0)
  {
    nA=(residualmode[(xoff-1)/2][yoff/2]==IPCM16x16)*16+(residualmode[(xoff-1)/2][yoff/2]!=INTERSKIP)*nzC[xoff-1][yoff];
  }
  if (yoff>0)
  {
    nB=(residualmode[xoff/2][(yoff-1)/2]==IPCM16x16)*16+(residualmode[xoff/2][(yoff-1)/2]!=INTERSKIP)*nzC[xoff][yoff-1];
  }

  nC=(nA+nB+(yoff>0)*(xoff>0))>>((yoff>0)*(xoff>0));
  return nC;
}
