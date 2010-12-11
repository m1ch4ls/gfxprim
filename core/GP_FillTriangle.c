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
 * Copyright (C) 2009-2010 Jiri "BlueBear" Dluhos                            *
 *                         <jiri.bluebear.dluhos@gmail.com>                  *
 *                                                                           *
 * Copyright (C) 2009-2010 Cyril Hrubis <metan@ucw.cz>                       *
 *                                                                           *
 *****************************************************************************/

#include "GP.h"
#include "GP_FnPerBpp.h"
#include "algo/FillTriangle.algo.h"

#include <stdlib.h>

/* Generate drawing functions for various bit depths. */
DEF_FILLTRIANGLE_FN(GP_FillTriangle8bpp, GP_Context *, GP_Pixel, GP_Line8bpp, GP_HLine8bpp)
DEF_FILLTRIANGLE_FN(GP_FillTriangle16bpp, GP_Context *, GP_Pixel, GP_Line16bpp, GP_HLine16bpp)
DEF_FILLTRIANGLE_FN(GP_FillTriangle24bpp, GP_Context *, GP_Pixel, GP_Line24bpp, GP_HLine24bpp)
DEF_FILLTRIANGLE_FN(GP_FillTriangle32bpp, GP_Context *, GP_Pixel, GP_Line32bpp, GP_HLine32bpp)

GP_RetCode GP_FillTriangle(GP_Context * context, int x0, int y0, int x1, int y1,
                           int x2, int y2, GP_Color color)
{
	GP_CHECK_CONTEXT(context);

	GP_Pixel pixel;
	pixel.type = context->pixel_type;
	GP_RetCode ret = GP_ColorToPixel(color, &pixel);

	GP_FN_PER_BPP(GP_FillTriangle, x0, y0, x1, y1, x2, y2, pixel);

	return ret;
}

GP_RetCode GP_TFillTriangle(GP_Context* context, int x0, int y0, int x1, int y1,
                            int x2, int y2, GP_Color color)
{
	GP_TRANSFORM_POINT(context, x0, y0);
	GP_TRANSFORM_POINT(context, x1, y1);
	GP_TRANSFORM_POINT(context, x2, y2);
	return GP_FillTriangle(context, x0, y0, x1, y1, x2, y2, color);
}
