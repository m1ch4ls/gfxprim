/*****************************************************************************
 * This file is part of gfxprim library.                                     *
 *                                                                           *
 * Gfxprim is free software; you can redistribute it and/or                  *
 * modify it under the terms of the GNU Lesser General Public                *
 * License as published by the Free Software Foundation; either              *
 * version 2.1 of the License, or (at your option) any later version.        *
 *                                                                           *
 * Gfxprim is distributed in the hope that it will be useful,                *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU         *
 * Lesser General Public License for more details.                           *
 *                                                                           *
 * You should have received a copy of the GNU Lesser General Public          *
 * License along with gfxprim; if not, write to the Free Software            *
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,                        *
 * Boston, MA  02110-1301  USA                                               *
 *                                                                           *
 * Copyright (C) 2011      Cyril Hrubis <metan@ucw.cz>                       *
 * Copyright (C) 2011      Tomas Gavenciak <gavento@ucw.cz>                  *
 *                                                                           *
 *****************************************************************************/

%% extends "base.h.t"

%% block descr
Access pixel bytes, Get and PutPixel
Do not include directly, use GP_Pixel.h
%% endblock

%% block body


 /*

   Note about byte aligment
   ~~~~~~~~~~~~~~~~~~~~~~~~

   Unaligned access happens when instruction that works with multiple byte
   value gets an address that is not divideable by the size of the value. Eg.
   if 32 bit integer instruction gets an address that is not a multiple of 4.
   On intel cpus this type of access works and is supported however the C
   standard defines this as undefined behavior. This fails to work ARM and most
   of the non intel cpus. So some more trickery must be done in order to write
   unaligned multibyte values. First of all we must compute offset and number
   of bytes to be accessed (which is cruicial for speed as we are going to read
   the pixel value byte by byte).

   The offsets (starting with the first one eg. pixel_size mod 8) forms subgroup
   in the mod 8 cyclic group. The maximal count of bits, from the start of the
   byte, then will be max from this subgroup + pixel_size. If this number is
   less or equal to 8 * N, we could write such pixel by writing N bytes.

   For example the offsets of 16 BPP forms subgroup only with {0} so we only
   need 2 bytes to write it. As a matter of fact the 16 and 32 BPP are special
   cases that are always aligned together with the 8 BPP (which is aligned
   trivially). These three are coded as special cases which yields to faster
   operations in case of 16 and 32 BPP. The 24 BPP is not aligned as there are
   no instruction to operate 3 byte long numbers.

   For second example take offsets of 20 BPP that forms subgroup {4, 0}
   so the max + pixel_size = 24 and indeed we fit into 3 bytes.

   If pixel_size is coprime to 8, the offsets generates whole group and so the
   max + pixel_size = 7 + pixel_size. The 17 BPP fits into 24 bits and so 3
   bytes are needed. The 19 BPP fits into 26 bits and because of that 4 bytes
   are needed. 

   Once we figure maximal number of bytes and the offset all that is to be done
   is to fetch first and last byte to combine it together with given pixel value
   and write the result back to the bitmap.

 */

#include "GP_GetSetBits.h"
#include "GP_Context.h"

%% for ps in pixelsizes
/* 
 * macro to get address of pixel in a {{ ps.suffix }} context
 */
#define GP_PIXEL_ADDR_{{ ps.suffix }}(context, x, y) \
	((GP_Pixel*)(((void*)((context)->pixels)) + (context)->bytes_per_row * (y) + ({{ ps.size }} * (x)) / 8))

/*
 * macro to get bit-offset of pixel in {{ ps.suffix }} context
 */
#define GP_PIXEL_ADDR_OFFSET_{{ ps.suffix }}(x) \
%% if not ps.needs_bit_endian()
	(0)
%% else
%% if ps.bit_endian == LE
%% if ps.size < 8
	(((x) % {{ 8 // ps.size }}) * {{ ps.size }})
%% else
	(({{ ps.size }} * (x)) % 8)
%% endif
%% else
%% if ps.size < 8
	({{ 8 - ps.size }} - ((x) % {{ 8 // ps.size }}) * {{ ps.size }})
%% else
	{{ error('Insanity check: big bit-endian with >8 bpp. Are you sure?') }}
%% endif
%% endif
%% endif

/*
 * GP_GetPixel for {{ ps.suffix }} 
 */
static inline GP_Pixel GP_GetPixel_Raw_{{ ps.suffix }}(const GP_Context *c, int x, int y)
{
%% if ps.size == 32
	/*
	 * 32 BPP is expected to have aligned pixels
	 */
	return *((uint32_t*)GP_PIXEL_ADDR_{{ ps.suffix }}(c, x, y));
%% elif ps.size == 16
	/*
	 * 16 BPP is expected to have aligned pixels
	 */
	return *((uint16_t*)GP_PIXEL_ADDR_{{ ps.suffix }}(c, x, y));
%% elif ps.size == 8
	/*
	 * 8 BPP is byte aligned
	 */
	return *((uint8_t*)GP_PIXEL_ADDR_{{ ps.suffix }}(c, x, y));
%% elif ps.size == 1 or ps.size == 2 or ps.size == 4 or ps.size == 8
	/*
	 * Whole pixel is stored only and only in one byte
	 *
	 * The full list = {1, 2, 4, 8}
	 */
	return GP_GET_BITS1_ALIGNED(GP_PIXEL_ADDR_OFFSET_{{ ps.suffix }}(x), {{ ps.size }},
		*(GP_PIXEL_ADDR_{{ ps.suffix }}(c, x, y)));
%% elif ps.size <= 10 or ps.size == 12 or ps.size == 16
	/*
	 * The pixel is stored in one or two bytes
	 *
	 * The max from subgroup (of mod 8 factor group) generated by
	 * pixel_size mod 8 + pixel_size <= 16
	 *
	 * The full list = {3, 5, 6, 7, 9, 10, 12, 16}
	 *
	 * Hint: If the pixel size is coprime to 8 the group is generated by
	 *       pixel_size mod 8 and maximal size thus is pixel_size + 7
	 */
	return GP_GET_BITS2_ALIGNED(GP_PIXEL_ADDR_OFFSET_{{ ps.suffix }}(x), {{ ps.size }},
		*(GP_PIXEL_ADDR_{{ ps.suffix }}(c, x, y)));
%% elif ps.size <= 18 or ps.size == 20 or ps.size == 24
	/*
	 * The pixel is stored in two or three bytes
	 *
	 * The max from subgroup (of mod 8 factor group) generated by
	 * pixel_size mod 8 + pixel_size <= 24
	 *
	 * The full list = {11, 13, 14, 15, 17, 18, 20, 24}
	 *
	 * Hint: If the pixel size is coprime to 8 the group is generated by
	 *       pixel_size mod 8 and maximal size thus is pixel_size + 7
	 */
	return GP_GET_BITS3_ALIGNED(GP_PIXEL_ADDR_OFFSET_{{ ps.suffix }}(x), {{ ps.size }},
		*(GP_PIXEL_ADDR_{{ ps.suffix }}(c, x, y)));
%% elif ps.size <= 23 or ps.size == 25 or ps.size == 26 or ps.size == 28 or ps.size == 32
	/*
	 * The pixel is stored in three or four bytes
	 *
	 * The max from subgroup (of mod 8 factor group) generated by
	 * pixel_size mod 8 + pixel_size <= 32
	 *
	 * The full list = {19, 21, 22, 23, 25, 26, 28, 32}
	 *
	 * Hint: If the pixel size is coprime to 8 the group is generated by
	 *       pixel_size mod 8 and maximal size thus is pixel_size + 7
	 */
	return GP_GET_BITS4_ALIGNED(GP_PIXEL_ADDR_OFFSET_{{ ps.suffix }}(x), {{ ps.size }},
		*(GP_PIXEL_ADDR_{{ ps.suffix }}(c, x, y)));
%% else
	#error not implemented
%% endif
}

/*
 * GP_PutPixel for {{ ps.suffix }} 
 */
static inline void GP_PutPixel_Raw_{{ ps.suffix }}(GP_Context *c, int x, int y, GP_Pixel p)
{
%% if ps.size == 32
	/*
	 * 32 BPP is expected to have aligned pixels
	 */
	*((uint32_t*)GP_PIXEL_ADDR_{{ ps.suffix }}(c, x, y)) = p;
%% elif ps.size == 16
	/*
	 * 16 BPP is expected to have aligned pixels
	 */
	*((uint16_t*)GP_PIXEL_ADDR_{{ ps.suffix }}(c, x, y)) = p;
%% elif ps.size == 8
	/*
	 * 8 BPP is byte aligned
	 */
	*((uint8_t*)GP_PIXEL_ADDR_{{ ps.suffix }}(c, x, y)) = p;
%% elif ps.size == 1 or ps.size == 2 or ps.size == 4 or ps.size == 8
	/*
	 * Whole pixel is stored only and only in one byte
	 *
	 * The full list = {1, 2, 4, 8}
	 */
	GP_SET_BITS1_ALIGNED(GP_PIXEL_ADDR_OFFSET_{{ ps.suffix }}(x), {{ ps.size }},
	                     GP_PIXEL_ADDR_{{ ps.suffix }}(c, x, y), p);
%% elif ps.size <= 10 or ps.size == 12 or ps.size == 16
	/*
	 * The pixel is stored in one or two bytes
	 *
	 * The max from subgroup (of mod 8 factor group) generated by
	 * pixel_size mod 8 + pixel_size <= 16
	 *
	 * The full list = {3, 5, 6, 7, 9, 10, 12, 16}
	 *
	 * Hint: If the pixel size is coprime to 8 the group is generated by
	 *       pixel_size mod 8 and maximal size thus is pixel_size + 7
	 */
	GP_SET_BITS2_ALIGNED(GP_PIXEL_ADDR_OFFSET_{{ ps.suffix }}(x), {{ ps.size }},
	                     GP_PIXEL_ADDR_{{ ps.suffix }}(c, x, y), p);
%% elif ps.size <= 18 or ps.size == 20 or ps.size == 24
	/*
	 * The pixel is stored in two or three bytes
	 *
	 * The max from subgroup (of mod 8 factor group) generated by
	 * pixel_size mod 8 + pixel_size <= 24
	 *
	 * The full list = {11, 13, 14, 15, 17, 18, 20, 24}
	 *
	 * Hint: If the pixel size is coprime to 8 the group is generated by
	 *       pixel_size mod 8 and maximal size thus is pixel_size + 7
	 */
	GP_SET_BITS3_ALIGNED(GP_PIXEL_ADDR_OFFSET_{{ ps.suffix }}(x), {{ ps.size }},
	                     GP_PIXEL_ADDR_{{ ps.suffix }}(c, x, y), p);
%% elif ps.size <= 23 or ps.size == 25 or ps.size == 26 or ps.size == 28 or ps.size == 32
	/*
	 * The pixel is stored in three or four bytes
	 *
	 * The max from subgroup (of mod 8 factor group) generated by
	 * pixel_size mod 8 + pixel_size <= 32
	 *
	 * The full list = {19, 21, 22, 23, 25, 26, 28, 32}
	 *
	 * Hint: If the pixel size is coprime to 8 the group is generated by
	 *       pixel_size mod 8 and maximal size thus is pixel_size + 7
	 */
	GP_SET_BITS4_ALIGNED(GP_PIXEL_ADDR_OFFSET_{{ ps.suffix }}(x), {{ ps.size }},
	                     GP_PIXEL_ADDR_{{ ps.suffix }}(c, x, y), p);
%% else
	#error not implemented
%% endif
}

static inline void GP_PutPixel_Raw_Clipped_{{ ps.suffix }}(GP_Context *c, GP_Coord x, GP_Coord y, GP_Pixel p)
{
	if (GP_PIXEL_IS_CLIPPED(c, x, y))
		return;

	GP_PutPixel_Raw_{{ ps.suffix }}(c, x, y, p);
}

%% endfor

%% endblock body
