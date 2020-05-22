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

#include <string.h>

#include "global.h"
#include "nalu.h"
#include "decode.h"

#ifdef __SDSCC__
#include "sds_lib.h"
#endif

#if _N_HLS_
extern FILE *prediction_test;
extern FILE* construction_test;
extern FILE *trace_bit;
extern FILE* debug_test;
#endif

FILE *bitstr;
FILE *p_out;

#ifdef __SDSCC__
StorablePicture *Pic;
unsigned char *Pic_Sluma_ptr;     // This pointer has to be cast to a 3D array.
unsigned char *Pic_SChroma_0_ptr; // This pointer has to be cast to a 3D array.
unsigned char *Pic_SChroma_1_ptr; // This pointer has to be cast to a 3D array.
StorablePictureInfo *Pic_info;
NALU_t *PINGPONGbuffer;
unsigned char *PINGPONGbuffer_buf;
unsigned long int PINGPONGbuffer_bit_offset;
#else
StorablePicture Pic[MAX_REFERENCE_PICTURES];
unsigned char Pic_Sluma[MAX_REFERENCE_PICTURES][PicWidthInSamplesL][FrameHeightInSampleL];
unsigned char Pic_SChroma_0[MAX_REFERENCE_PICTURES][PicWidthInSamplesC][FrameHeightInSampleC];
unsigned char Pic_SChroma_1[MAX_REFERENCE_PICTURES][PicWidthInSamplesC][FrameHeightInSampleC];
StorablePictureInfo Pic_info[MAX_REFERENCE_PICTURES];


NALU_t PINGPONGbuffer;
unsigned char PINGPONGbuffer_buf[MAXNALBUFFERSIZE];
unsigned long int PINGPONGbuffer_bit_offset;
#endif

void write_out_pic(StorablePicture *pic, unsigned pic_index, unsigned char Pic_Sluma[MAX_REFERENCE_PICTURES][PicWidthInSamplesL][FrameHeightInSampleL], unsigned char Pic_SChroma_0[MAX_REFERENCE_PICTURES][PicWidthInSamplesC][FrameHeightInSampleC], unsigned char Pic_SChroma_1[MAX_REFERENCE_PICTURES][PicWidthInSamplesC][FrameHeightInSampleC], FILE * p_out)
{
  int i,j;

  for(i=0;i<FrameHeightInSampleL;i++)
    for(j=0; j<PicWidthInSamplesL; j++)
    {
      fputc(Pic_Sluma[pic_index][j][i],p_out);
    }
  for(i=0;i<FrameHeightInSampleC;i++)
    for(j=0; j<PicWidthInSamplesC; j++)
    {
      fputc(Pic_SChroma_0[pic_index][j][i],p_out);
    }
  for(i=0;i<FrameHeightInSampleC;i++)
    for(j=0 ;j <PicWidthInSamplesC ;  j++)
    {
      fputc(Pic_SChroma_1[pic_index][j][i],p_out);
    }

}


int main(int argc, char **argv)
{
  char AnnexbFileName[100];

#ifdef __SDSCC__
  Pic = (StorablePicture *) sds_alloc(MAX_REFERENCE_PICTURES * sizeof(StorablePicture));

  // This pointer has to be cast to a 3D array.
  Pic_Sluma_ptr = (unsigned char *) sds_alloc(MAX_REFERENCE_PICTURES * PicWidthInSamplesL * FrameHeightInSampleL * sizeof(unsigned char));
  Pic_SChroma_0_ptr = (unsigned char *) sds_alloc(MAX_REFERENCE_PICTURES * PicWidthInSamplesC * FrameHeightInSampleC * sizeof(unsigned char));
  Pic_SChroma_1_ptr = (unsigned char *) sds_alloc(MAX_REFERENCE_PICTURES * PicWidthInSamplesC * FrameHeightInSampleC * sizeof(unsigned char));

  nz_coeff_chroma_ptr = (unsigned char*) sds_alloc(2*PicWidthInMBs*2*FrameHeightInMbs*2 * sizeof(unsigned char));

  // Pointer to 3D-array cast.
  unsigned char (*Pic_Sluma)[PicWidthInSamplesL][FrameHeightInSampleL] = (unsigned char (*)[PicWidthInSamplesL][FrameHeightInSampleL]) Pic_Sluma_ptr;
  unsigned char (*Pic_SChroma_0)[PicWidthInSamplesC][FrameHeightInSampleC] = (unsigned char (*)[PicWidthInSamplesC][FrameHeightInSampleC]) Pic_SChroma_0_ptr;
  unsigned char (*Pic_SChroma_1)[PicWidthInSamplesC][FrameHeightInSampleC] = (unsigned char (*)[PicWidthInSamplesC][FrameHeightInSampleC]) Pic_SChroma_1_ptr;

  nz_coeff_chroma = (unsigned char (*)[PicWidthInMBs*2][FrameHeightInMbs*2]) nz_coeff_chroma_ptr;

  Pic_info = (StorablePictureInfo *) sds_alloc(MAX_REFERENCE_PICTURES * sizeof(StorablePictureInfo));
  PINGPONGbuffer = (NALU_t *) sds_alloc(sizeof(PINGPONGbuffer));
  PINGPONGbuffer_buf = sds_alloc(MAXNALBUFFERSIZE * sizeof(unsigned char));

  img_inst = (ImageParameters *) sds_alloc(sizeof(ImageParameters));
#else
  img_inst = (ImageParameters *) malloc(sizeof(ImageParameters));
#endif

#ifdef VALIDATION
  printf("Size: %d x %d\n", PicWidthInMBs*16, FrameHeightInMbs*16);
#endif

  if(argc != 3)
  {
    printf("ERROR: Usage %s <input_file.264> <golden_file.yuv>", argv[0]);
    exit(-1);
  }


  if(argc == 3)
  {
    sprintf(AnnexbFileName, "%s", argv[1]);
  }
  else
    strcpy(AnnexbFileName,"input/test.264");


  bitstr=fopen(AnnexbFileName,"rb");
  if(bitstr==NULL)
  {
    puts("cannot find the corresponding file.");
    exit(-1);
  }

#if _N_HLS_
  prediction_test=fopen("pred_test.txt","w");
  construction_test=fopen("construc_test.txt","w");
  trace_bit=fopen("trace.txt","w");
  debug_test=fopen("debug_test.txt","w");
#endif


  p_out=fopen("testresult.yuv","wb");

#ifdef __SDSCC__
  PINGPONGbuffer->nal_unit_type=0;
#else
  PINGPONGbuffer.nal_unit_type=0;
#endif
  memset(Pic, 0, MAX_REFERENCE_PICTURES * sizeof(StorablePicture));
  memset(Pic_Sluma, 0, MAX_REFERENCE_PICTURES * PicWidthInSamplesL * FrameHeightInSampleL * sizeof(unsigned char));
  memset(Pic_SChroma_0, 0, MAX_REFERENCE_PICTURES * PicWidthInSamplesC * FrameHeightInSampleC * sizeof(unsigned char));
  memset(Pic_SChroma_1, 0, MAX_REFERENCE_PICTURES * PicWidthInSamplesC * FrameHeightInSampleC * sizeof(unsigned char));
  memset(Pic_info, 0, MAX_REFERENCE_PICTURES*sizeof(StorablePictureInfo));
  memset(nz_coeff_chroma, 0, 2*PicWidthInMBs*2*FrameHeightInMbs*2*sizeof(unsigned char));
  memset(img_inst, 0, sizeof(ImageParameters));

  int i=0;
  int j;
  int poc=0;


  while(1)
  {
#ifdef __SDSCC__
    if(GetAnnexbNALU (PINGPONGbuffer, PINGPONGbuffer_buf, &PINGPONGbuffer_bit_offset, bitstr)==0)
      break;

    PINGPONGbuffer->len = EBSPtoRBSP (PINGPONGbuffer_buf, PINGPONGbuffer->len, 1);
    RBSPtoSODB(PINGPONGbuffer, PINGPONGbuffer_buf, &PINGPONGbuffer_bit_offset, PINGPONGbuffer->len-1);


    decode_main(PINGPONGbuffer, PINGPONGbuffer_buf, &PINGPONGbuffer_bit_offset, Pic, Pic_Sluma, Pic_SChroma_0, Pic_SChroma_1, Pic_info);


    if(PINGPONGbuffer->nal_unit_type==5 || PINGPONGbuffer->nal_unit_type==1 )
#else
    if(GetAnnexbNALU (&PINGPONGbuffer, PINGPONGbuffer_buf, &PINGPONGbuffer_bit_offset, bitstr)==0)
      break;

    PINGPONGbuffer.len = EBSPtoRBSP (PINGPONGbuffer_buf, PINGPONGbuffer.len, 1);
    RBSPtoSODB(&PINGPONGbuffer, PINGPONGbuffer_buf, &PINGPONGbuffer_bit_offset, PINGPONGbuffer.len-1);


    decode_main(&PINGPONGbuffer, PINGPONGbuffer_buf, &PINGPONGbuffer_bit_offset, Pic, Pic_Sluma, Pic_SChroma_0, Pic_SChroma_1, Pic_info);


    if(PINGPONGbuffer.nal_unit_type==5 || PINGPONGbuffer.nal_unit_type==1 )
#endif
      for(j=0;j<MAX_REFERENCE_PICTURES;j++)
        for(i=0;i<MAX_REFERENCE_PICTURES;i++)
        {
          if(Pic[i].memoccupied && Pic[i].Picorder_num==poc)
          {
            write_out_pic(&Pic[i], i, Pic_Sluma, Pic_SChroma_0, Pic_SChroma_1, p_out);
#ifdef VALIDATION
            printf("writing %d\n",poc );
#endif
            poc++;
          }
        }
  }




#if _N_HLS_
  fclose(prediction_test);
  fclose(construction_test);
  fclose(trace_bit);
  fclose(debug_test);
#endif
  fclose(bitstr);
  fclose(p_out);

#ifdef VALIDATION
  sprintf(AnnexbFileName,"diff -q testresult.yuv %s",argv[2]);

  if (!system(AnnexbFileName))
    printf("PASSED\n");
  else
    printf("FAILED\n");
#endif

#if 0
#ifdef __SDSCC__
  free(Pic);

  free(Pic_Sluma_ptr);
  free(Pic_SChroma_0_ptr);
  free(Pic_SChroma_1_ptr);

  free(Pic_info);
  free(PINGPONGbuffer);
  free(PINGPONGbuffer_buf);

  free(img_inst);
#else
  free(img_inst);
#endif
#endif


  return 0;
}
