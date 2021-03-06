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

%% extends "common.c.t"

{% block descr %}Floyd Steinberg dithering RGB888 -> any pixel{% endblock %}

%% block body

#include "core/GP_Core.h"
#include "core/GP_Pixel.h"
#include "GP_Filter.h"

#include <string.h>

%% macro distribute_error(errors, x, y, w, err)
			if ({{ x }} + 1 < {{ w }})
				{{ errors }}[{{ y }}%2][{{ x }}+1] += 7 * {{ err }} / 16;

			if ({{ x }} > 1)
				{{ errors }}[!({{ y }}%2)][{{ x }}-1] += 3 * {{ err }} / 16;

			{{ errors }}[!({{ y }}%2)][{{ x }}] += 5 * {{ err }} / 16;

			if ({{ x }} + 1 < {{ w }})
				{{ errors }}[!({{ y }}%2)][{{ x }}+1] += {{ err }} / 16;
%% endmacro

%% for pt in pixeltypes
%% if pt.is_gray() or pt.is_rgb() and not pt.is_alpha()
/*
 * Floyd Steinberg RGB888 to {{ pt.name }}
 */
int GP_FilterFloydSteinberg_RGB888_to_{{ pt.name }}_Raw(const GP_Context *src, GP_Context *dst,
                                      GP_ProgressCallback *callback)
{
%% for c in pt.chanslist
	float errors_{{ c[0] }}[2][src->w];
%% endfor

	GP_DEBUG(1, "Floyd Steinberg %s to %s %ux%u",
	            GP_PixelTypeName(src->pixel_type),
	            GP_PixelTypeName(GP_PIXEL_{{ pt.name }}),
		    src->w, src->h);

	GP_Coord x, y;

%% for c in pt.chanslist
	memset(errors_{{ c[0] }}[0], 0, src->w * sizeof(float));
	memset(errors_{{ c[0] }}[1], 0, src->w * sizeof(float));
%% endfor

	for (y = 0; y < (GP_Coord)src->h; y++) {
		for (x = 0; x < (GP_Coord)src->w; x++) {
			GP_Pixel pix = GP_GetPixel_Raw_24BPP(src, x, y);

%% for c in pt.chanslist
%% if pt.is_gray()
			float val_{{ c[0] }} = GP_Pixel_GET_R_RGB888(pix) +
			                       GP_Pixel_GET_G_RGB888(pix) +
			                       GP_Pixel_GET_B_RGB888(pix);
%% else
			float val_{{ c[0] }} = GP_Pixel_GET_{{ c[0] }}_RGB888(pix);
%% endif
			val_{{ c[0] }} += errors_{{ c[0] }}[y%2][x];

			float err_{{ c[0] }} = val_{{ c[0] }};
%% if pt.is_gray()
			GP_Pixel res_{{ c[0] }} = {{ 2 ** c[2] - 1}} * val_{{ c[0] }} / (3 * 255);
			err_{{ c[0] }} -= res_{{ c[0] }} * (3 * 255) / {{ 2 ** c[2] - 1}};
%% else
			GP_Pixel res_{{ c[0] }} = {{ 2 ** c[2] - 1}} * val_{{ c[0] }} / 255;
			err_{{ c[0] }} -= res_{{ c[0] }} * 255 / {{ 2 ** c[2] - 1}};
%% endif

{{ distribute_error("errors_%s"|format(c[0]), 'x', 'y', '(GP_Coord)src->w', 'err_%s'|format(c[0])) }}

			{{ clamp_val("res_%s"|format(c[0]), c[2]) }}
%% endfor

%% if pt.is_gray()
			GP_PutPixel_Raw_{{ pt.pixelsize.suffix }}(dst, x, y, res_V);
%% else
			GP_Pixel res = GP_Pixel_CREATE_{{ pt.name }}(res_{{ pt.chanslist[0][0] }}{% for c in pt.chanslist[1:] %}, res_{{ c[0] }}{% endfor %});

			GP_PutPixel_Raw_{{ pt.pixelsize.suffix }}(dst, x, y, res);
%% endif
		}

%% for c in pt.chanslist
		memset(errors_{{ c[0] }}[y%2], 0, src->w * sizeof(float));
%% endfor

		if (GP_ProgressCallbackReport(callback, y, src->h, src->w))
			return 1;
	}

	GP_ProgressCallbackDone(callback);
	return 0;
}

%% endif
%% endfor

int GP_FilterFloydSteinberg_RGB888_Raw(const GP_Context *src,
                                              GP_Context *dst,
                                              GP_ProgressCallback *callback)
{
	switch (dst->pixel_type) {
%% for pt in pixeltypes
%% if pt.is_gray() or pt.is_rgb() and not pt.is_alpha()
	case GP_PIXEL_{{ pt.name }}:
		return GP_FilterFloydSteinberg_RGB888_to_{{ pt.name }}_Raw(src, dst, callback);
%% endif
%% endfor
	default:
		return 1;
	}
}

%% endblock body
