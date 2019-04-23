/*
 * Author: Andrey & Mike 
 * Feb 2019
 * Cool image format encoder, (which is derived from bmpenc.c).  Encodes frames using a pal8 pixel format the decoded file
 * is approximately 1 byte per pixel.  pal8 is a 8 bit pixel depth format giving the output 
 * 256 colors. Note: Encoded data packets store a vertically inverted version of the image.
 *              
 *  ________             ______________        
 * |        |           |              |      
 * | output | <-------- | encoded data | <----+--------Decoded Frames
 * | file   |   muxer   | packets      |   encoder
 * |________|           |______________|

 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */


#include "libavutil/imgutils.h"
#include "libavutil/avassert.h"
#include "avcodec.h"
#include "bytestream.h"
#include "bmp.h"
#include "internal.h"


static av_cold int cool_encode_init(AVCodecContext *avctx){
  avctx->bits_per_coded_sample = 8;
  return 0;
}

static int cool_encode_frame(AVCodecContext *avctx, AVPacket *av_pkt,
			     const AVFrame *pict, int *got_packet)
{
  const AVFrame * const avFrame_pict = pict;
  int n_bytes_image, n_bytes_per_row, n_bytes, i, hsize, ret;
  const uint32_t *pal = NULL;
  int pad_bytes_per_row, pal_entries = 0;
  int bit_count = avctx->bits_per_coded_sample;
  uint8_t *image_info_uint8_ptr, *buf;

#if FF_API_CODED_FRAME
  FF_DISABLE_DEPRECATION_WARNINGS
  avctx->coded_frame->pict_type = AV_PICTURE_TYPE_I;
  avctx->coded_frame->key_frame = 1;
  FF_ENABLE_DEPRECATION_WARNINGS
#endif
  
  //PAL8 assumed  
  pal = (uint32_t *)avFrame_pict->data[1];       

  // 1 << 8 = 256
  if (pal && !pal_entries) pal_entries = 1 << bit_count; 
    
  //7LL >> 3LL Padding the bit count then shifting to get bytes from the bits (LL = LONG LONG for int64)
  //Math taken from Bmpenc.c
  n_bytes_per_row = ((int64_t)avctx->width * (int64_t)bit_count + 7LL) >> 3LL;

  //This is used to create line sizes in multiples of 4, for bytes
  pad_bytes_per_row = (4 - n_bytes_per_row) & 3; 
    
  //Height Times Width
  n_bytes_image = avctx->height * (n_bytes_per_row + pad_bytes_per_row);

#define SIZE_COOLFILEHEADER 8 //4 chars + 32bit int(4)
#define SIZE_COOLINFOHEADER 12
  
  hsize = SIZE_COOLFILEHEADER + SIZE_COOLINFOHEADER + (pal_entries << 2);
  av_log(NULL, AV_LOG_INFO, "hsize %d:", hsize);
  n_bytes = n_bytes_image + hsize;

  if ((ret = ff_alloc_packet2(avctx, av_pkt, n_bytes, 0)) < 0)
    return ret;

  //Get data from the packet so that we can start adding our Header to it
  buf = av_pkt->data;

  bytestream_put_byte(&buf, 'c');                   // First Four Bytes Identifier //1
  bytestream_put_byte(&buf, 'o');                   // ...                         //2
  bytestream_put_byte(&buf, 'o');                   // ...                         //3
  bytestream_put_byte(&buf, 'l');                   // ...                         //4
  bytestream_put_le32(&buf, hsize);                 // COOLFILEHEADER.bfOffBits    //5
  //END COOLFILEHEADER COUNT HERE
    
  bytestream_put_le32(&buf, avctx->width);          // COOLHEADER.biWidth          //7
  bytestream_put_le32(&buf, avctx->height);         // COOLHEADER.biHeight         //8
  bytestream_put_le32(&buf, n_bytes_image);        // COOLHEADER.biSizeImage       
  //END COOLINFOHEADER COUNT HERE
    
  //Store Colors
  for (i = 0; i < pal_entries; i++)
    bytestream_put_le32(&buf, pal[i] & 0xFFFFFF); //10 Mask most significant bits

  // Cool files are stored upside down.
  //Start From the top
  image_info_uint8_ptr = avFrame_pict->data[0];
    
  //Move over in memory past the header info
  //Inspired by Bmpenc.c
  buf = av_pkt->data + hsize;
  for(i = 0; i < avctx->height; i++) {
    //Copy from the buffer into the ptr row by row
    memcpy(buf, image_info_uint8_ptr, n_bytes_per_row);

    //Advance the buffer
    buf += n_bytes_per_row;

    //Set memory to 0
    memset(buf, 0, pad_bytes_per_row);

    //Advance
    buf += pad_bytes_per_row;
    image_info_uint8_ptr += avFrame_pict->linesize[0]; // ... and go back
  }
    
  //This is a bit wise or and is the equivalent of pk->flags = pkt->flags | AV_PKT_FLAG_KEY;
  av_pkt->flags |= AV_PKT_FLAG_KEY;
  *got_packet = 1;
  return 0;
}
AVCodec ff_cool_encoder = {
  .name           = "cool",
  .long_name      = NULL_IF_CONFIG_SMALL("COOL image (CS 3505 Spring 2019)"),
  .type           = AVMEDIA_TYPE_VIDEO,
  .id             = AV_CODEC_ID_COOL,
  .init           = cool_encode_init,
  .encode2        = cool_encode_frame,
  .pix_fmts       = (const enum AVPixelFormat[]){
    AV_PIX_FMT_PAL8,
    AV_PIX_FMT_NONE
  },
};
