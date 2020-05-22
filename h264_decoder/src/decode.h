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


#ifndef _DECODE_H
#define _DECODE_H
#include "global.h"
void decode_main(NALU_t* nalu, unsigned char *nalu_buf, unsigned long int *nalu_bit_offset, StorablePicture pic[MAX_REFERENCE_PICTURES], unsigned char pic_Sluma[MAX_REFERENCE_PICTURES][PicWidthInSamplesL][FrameHeightInSampleL], unsigned char pic_SChroma_0[MAX_REFERENCE_PICTURES][PicWidthInSamplesC][FrameHeightInSampleC], unsigned char pic_SChroma_1[MAX_REFERENCE_PICTURES][PicWidthInSamplesC][FrameHeightInSampleC], StorablePictureInfo pic_info[MAX_REFERENCE_PICTURES]);

seq_parameter_set_rbsp_t SPS_GLOBAL;
pic_parameter_set_rbsp_t PPS_GLOBAL;

ImageParameters *img_inst;

slice_header_rbsp_t sliceHeader_inst;

#endif
