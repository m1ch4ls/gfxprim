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
 * Copyright (C) 2009-2013 Cyril Hrubis <metan@ucw.cz>                       *
 *                                                                           *
 *****************************************************************************/

%% extends "base.h.t"

%%- block descr
Macros to mix two pixels. The source must have alpha channel.
%%- endblock

%% block body

#include "core/GP_GammaCorrection.h"
#include "core/GP_Pixel.h"

//TODO: Fix blit where both source and destination have alpha channel

%% for src in pixeltypes
%%  if not src.is_unknown() and not src.is_palette()
%%   for dst in pixeltypes
%%    if not dst.is_unknown() and not dst.is_palette()
%%     if src.is_alpha()

static inline GP_Pixel GP_MixPixels_{{ src.name }}_{{ dst.name }}(GP_Pixel src, GP_Pixel dst)
{
	/* Extract the alpha channel */
	unsigned int alpha = GP_Pixel_GET_A_{{ src.name }}(src);

	/* Convert the pixel to RGB888, mix the values */
	GP_Pixel src_rgb = 0, dst_rgb = 0, res = 0;

	GP_Pixel_{{ src.name }}_TO_RGB888(src, src_rgb);
	GP_Pixel_{{ dst.name }}_TO_RGB888(dst, dst_rgb);

	int sr, sg, sb;
	int dr, dg, db;

	sr = GP_Pixel_GET_R_RGB888(src_rgb);
	sg = GP_Pixel_GET_G_RGB888(src_rgb);
	sb = GP_Pixel_GET_B_RGB888(src_rgb);
	
	dr = GP_Pixel_GET_R_RGB888(dst_rgb);
	dg = GP_Pixel_GET_G_RGB888(dst_rgb);
	db = GP_Pixel_GET_B_RGB888(dst_rgb);

%%      set a_max = 2 ** src.chans['A'][2] - 1

	dr = (dr * ({{ a_max }} - alpha) + sr * alpha + {{ a_max // 2 }}) / {{ a_max }};
	dg = (dg * ({{ a_max }} - alpha) + sg * alpha + {{ a_max // 2 }}) / {{ a_max }};
	db = (db * ({{ a_max }} - alpha) + sb * alpha + {{ a_max // 2 }}) / {{ a_max }};

	dst_rgb = GP_Pixel_CREATE_RGB888(dr, dg, db);

	GP_Pixel_RGB888_TO_{{ dst.name }}(dst_rgb, res);

	return res;
}

%%     endif
%%    endif
%%   endfor
%%  endif
%% endfor

%% endblock
