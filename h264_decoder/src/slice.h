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


#ifndef _SLICE_H_
#define _SLICE_H_

void ProcessSlice
(
 NALU_t* nalu,
 unsigned char *nalu_buf, unsigned long int *nalu_bit_offset,
 StorablePicture PIC[MAX_REFERENCE_PICTURES],
 unsigned char pic_Sluma[MAX_REFERENCE_PICTURES][PicWidthInSamplesL][FrameHeightInSampleL],
 unsigned char pic_SChroma_0[MAX_REFERENCE_PICTURES][PicWidthInSamplesC][FrameHeightInSampleC],
 unsigned char pic_SChroma_1[MAX_REFERENCE_PICTURES][PicWidthInSamplesC][FrameHeightInSampleC],
 StorablePictureInfo  PICINFO[MAX_REFERENCE_PICTURES],
 unsigned char Imode[PicWidthInMBs][FrameHeightInMbs],
 char IntraPredMode[PicWidthInMBs*4][FrameHeightInMbs*4],
 unsigned char NzLuma[PicWidthInMBs*4][FrameHeightInMbs*4],
 unsigned char NzChroma[2][PicWidthInMBs*2][FrameHeightInMbs*2],
 unsigned char constraint_intra_flag,
 slice_header_rbsp_t *SH,
 ImageParameters* img
 );
#endif // _SLICE_H_
