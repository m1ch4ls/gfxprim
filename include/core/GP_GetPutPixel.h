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
 * Copyright (C) 2011      Tomas Gavenciak <gavento@ucw.cz>                  *
 * Copyright (C) 2011-2012 Cyril Hrubis <metan@ucw.cz>                       *
 *                                                                           *
 *****************************************************************************/

#ifndef CORE_GP_GETPUTPIXEL_H
#define CORE_GP_GETPUTPIXEL_H

#include "GP_Context.h"
#include "GP_Transform.h"
#include "GP_FnPerBpp.h"
#include "GP_Pixel.h"

/*
 * Generated header
 */
#include "GP_GetPutPixel.gen.h"

/*
 * GetPixel with context transformations and clipping.
 * Returns 0 for clipped pixels or pixels outside bitmap.
 */
GP_Pixel GP_GetPixel(const GP_Context *context, GP_Coord x, GP_Coord y);

/*
 * Version of GetPixel without transformations nor border checking.
 */
static inline GP_Pixel GP_GetPixel_Raw(const GP_Context *context,
                                       GP_Coord x, GP_Coord y)
{
	GP_FN_RET_PER_BPP(GP_GetPixel_Raw, context->bpp, context->bit_endian,
		context, x, y);

	GP_ABORT("Invalid context pixel type");
}

/*
 * Version of GetPixel without transformations and with border checking.
 */
static inline GP_Pixel GP_GetPixel_Raw_Clipped(const GP_Context *context,
                                               GP_Coord x, GP_Coord y)
{
	if (GP_PIXEL_IS_CLIPPED(context, x, y))
		return 0;

	return GP_GetPixel_Raw(context, x, y);
}

/*
 * PutPixel with context transformations and clipping.
 * NOP for clipped pixels or pixels outside bitmap.
 */
void GP_PutPixel(GP_Context *context, GP_Coord x, GP_Coord y, GP_Pixel p);

/*
 * Version of PutPixel without transformations nor border checking.
 */
static inline void GP_PutPixel_Raw(GP_Context *context,
                                   GP_Coord x, GP_Coord y, GP_Pixel p)
{
	GP_FN_PER_BPP(GP_PutPixel_Raw, context->bpp, context->bit_endian,
		context, x, y, p);
}

/*
 * Version of PutPixel without transformation and with border checking.
 */
static inline void GP_PutPixel_Raw_Clipped(GP_Context *context,
                                           GP_Coord x, GP_Coord y, GP_Pixel p)
{
	GP_FN_PER_BPP(GP_PutPixel_Raw_Clipped, context->bpp, context->bit_endian,
	              context, x, y, p);
}

/*
 * Returns pixel offset.
 */
uint8_t GP_PixelAddrOffset(GP_Coord x, GP_PixelType pixel_type);

#endif /* CORE_GP_GETPUTPIXEL_H */
