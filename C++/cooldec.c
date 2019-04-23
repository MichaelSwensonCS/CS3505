/* Author: Andrey & Mike
 *  Feb 2019
 * Cool image format decoder (derived from bmp.c).  Decodes frames using a pal8 pixel format the decoded file
 * is approximately 1 byte per pixel.  pal8 is a 8 bit pixel depth format giving the output 
 * 256 colors. Note: Encoded data packets contain a vertically inverted image and this decoder
 * vertically flips the image.
 *
 *
 *  _______              ______________
 * |       |            |              |
 * | input |  demuxer   | encoded data |   decoder
 * | file  | ---------> | packets      | -----+
 * |_______|            |______________|      |
 *                                     _______v_
 *                                    |         |
 *                                    | decoded |
 *                                    | frames  |
 *                                    |_________|
 *

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

 ***********************/

#include <inttypes.h>

#include "avcodec.h"
#include "bytestream.h"
#include "bmp.h"
#include "internal.h"
#include "msrledec.h"

static int cool_decode_frame(AVCodecContext *avctx,
			     void *data, int *got_frame,
			     AVPacket *avpkt)
{
  const uint8_t *buf = avpkt->data;
  int buf_size       = avpkt->size;
  AVFrame *p         = data;
  unsigned int hsize;
  int width, height;
  unsigned int depth;
  unsigned int ihsize;
  int i, line_size_in_file, linesize, ret;
  uint8_t *image_info_ptr;
  const uint8_t *buffer_initial_pos = buf;
  
  int HEADER_SIZE = 12;

  //NOTE: Comments that are just numbers are mapped to numbered comments in the coolenc.c 
  //1-4
  if (bytestream_get_byte(&buf) != 'c' ||
      bytestream_get_byte(&buf) != 'o' ||
      bytestream_get_byte(&buf) != 'o' ||
      bytestream_get_byte(&buf) != 'l') {
    av_log(avctx, AV_LOG_ERROR, "Bad Identifier, First 4 Bytes are not 'c' 'o' 'o' 'l'\n");
    return AVERROR_INVALIDDATA;
  }
    
  //5-6
  hsize  = bytestream_get_le32(&buf); /* header size */
  ihsize = 8; /* more header size */
    
  //7-8
  width  = bytestream_get_le32(&buf);
  height = bytestream_get_le32(&buf);
        
  //Bit_Count 8 // 256 colors 1 byte per pixel
  depth = 8;

  int colors = 1 << depth; //Colors = 256

  //FFMPEG Error Checking and setting image width/height
  ret = ff_set_dimensions(avctx, width, height);
  if (ret < 0) {
    return AVERROR_INVALIDDATA;
  }

  avctx->pix_fmt = AV_PIX_FMT_PAL8;

  //FFMPEG Error Checking
  if ((ret = ff_get_buffer(avctx, p, 0)) < 0)
    return ret;

  p->pict_type = AV_PICTURE_TYPE_I;
  p->key_frame = 1;
  
  //Move Buffer Past Header Info because we are done with INFOHEADER
  buf = buffer_initial_pos + hsize;

  /* Line size in file multiple of 4 from Bmp.c */
  //Assume width is 10 = 1010 * 8 = 80 + 31 = 111 / 8 = 13 = 1011 & !3 = 1011 & 0100 = 1000 = 8 
  line_size_in_file = ((avctx->width * depth + 31) / 8) & ~3;

  //Flip the Image
  image_info_ptr = p->data[0];
  linesize       = p->linesize[0];
    
  memset(p->data[1], 0, 1024);
	        
  //palette location
  buf = buffer_initial_pos + HEADER_SIZE + ihsize; 

  //From BMP.c reading palette data into buffer
  for (i = 0; i < colors; i++)
    ((uint32_t*)p->data[1])[i] = 0xFFU << 24 | bytestream_get_le32(&buf);
  buf = buffer_initial_pos + hsize;
   
  //depth should always be 8 but error checking just in case
  //Also derivative of Bmp.c
  if(depth == 8) {
    for (i = 0; i < avctx->height; i++) {
      //Copy from the pointer to the buffer for the entire line
      memcpy(image_info_ptr, buf, line_size_in_file);
      buf += line_size_in_file;
      //Advance to the next line
      image_info_ptr += linesize;
    }
  }else{
    av_log(avctx, AV_LOG_ERROR, "COOL decoder is broken\n");
    return AVERROR_INVALIDDATA;
  }
    
  *got_frame = 1;

  return buf_size;
}

AVCodec ff_cool_decoder = {
  .name           = "cool",
  .long_name      = NULL_IF_CONFIG_SMALL("COOL image (CS 3505 Spring 2019)"),
  .type           = AVMEDIA_TYPE_VIDEO,
  .id             = AV_CODEC_ID_COOL,
  .decode         = cool_decode_frame,
  .capabilities   = AV_CODEC_CAP_DR1,
};
