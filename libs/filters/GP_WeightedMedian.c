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
 * Copyright (C) 2009-2012 Cyril Hrubis <metan@ucw.cz>                       *
 *                                                                           *
 *****************************************************************************/

#include "core/GP_Context.h"
#include "core/GP_GetPutPixel.h"
#include "core/GP_TempAlloc.h"
#include "core/GP_Clamp.h"

#include "core/GP_Debug.h"

#include "GP_WeightedMedian.h"

#include <string.h>

static unsigned int sum_weights(GP_MedianWeights *weights)
{
	unsigned int i;
	unsigned int sum = 0;

	for (i = 0; i < weights->w * weights->h; i++)
		sum += weights->weights[i];

	return sum;
}

static inline void hist_add(unsigned int *hist, unsigned int val,
                            unsigned int count)
{
	hist[val] += count;
}

static inline unsigned int hist_med(unsigned int *hist, unsigned int size,
                                    unsigned int threshold)
{
	unsigned int i;
	unsigned int acc = 0;

	for (i = 0; i < size; i++) {
		acc += hist[i];
		if (acc >= threshold)
			return i;
	}

	GP_BUG("Threshold not reached");
	return 0;
}

static inline void hist_clear(unsigned int *hist, unsigned int size)
{
	memset(hist, 0, sizeof(unsigned int) * size);
}

static inline unsigned int get_weight(GP_MedianWeights *weights,
                                      unsigned int x, unsigned int y)
{
	return weights->weights[y * weights->w + x];
}

static int GP_FilterWeightedMedian_Raw(const GP_Context *src,
                                       GP_Coord x_src, GP_Coord y_src,
                                       GP_Size w_src, GP_Size h_src,
                                       GP_Context *dst,
                                       GP_Coord x_dst, GP_Coord y_dst,
                                       GP_MedianWeights *weights,
				       GP_ProgressCallback *callback)
{
	int x, y, sum = sum_weights(weights);
	unsigned int x1, y1;

	GP_CHECK(src->pixel_type == GP_PIXEL_RGB888);

	GP_DEBUG(1, "Weighted Median filter size %ux%u xmed=%u ymed=%u sum=%u",
	            w_src, h_src, weights->w, weights->h, sum);

	unsigned int w = w_src +  weights->w;
	unsigned int size = w * weights->h;

	GP_TempAllocCreate(temp, 3 * size * sizeof(unsigned int));

	unsigned int *R = GP_TempAllocGet(temp, size * sizeof(unsigned int));
	unsigned int *G = GP_TempAllocGet(temp, size * sizeof(unsigned int));
	unsigned int *B = GP_TempAllocGet(temp, size * sizeof(unsigned int));

	/* prefil the sampled array */
	for (x = 0; x < (int)w; x++) {
		int xi = GP_CLAMP(x_src + x - (int)weights->w/2, 0, (int)src->w - 1);
			
		for (y = 0; y < (int)weights->h; y++) {
			int yi = GP_CLAMP(y_src + y - (int)weights->h, 0, (int)src->h - 1);

			GP_Pixel pix = GP_GetPixel_Raw_24BPP(src, xi, yi);

			R[y * w + x] = GP_Pixel_GET_R_RGB888(pix);
			G[y * w + x] = GP_Pixel_GET_G_RGB888(pix);
			B[y * w + x] = GP_Pixel_GET_B_RGB888(pix);
		}
	}

	unsigned int hist_R[256];
	unsigned int hist_G[256];
	unsigned int hist_B[256];

	hist_clear(hist_R, 256);
	hist_clear(hist_G, 256);
	hist_clear(hist_B, 256);

	/* Apply the weighted median filter */
	for (y = 0; y < (int)h_src; y++) {
		for (x = 0; x < (int)w_src; x++) {
			/* compute weighted histogram and then median */
			for (x1 = 0; x1 < weights->w; x1++) {
				for (y1 = 0; y1 < weights->h; y1++) {
					unsigned int weight = get_weight(weights, x1, y1);
					hist_add(hist_R, R[y1 * w + x + x1], weight);
					hist_add(hist_G, G[y1 * w + x + x1], weight);
					hist_add(hist_B, B[y1 * w + x + x1], weight);
				}
			}

			unsigned int r = hist_med(hist_R, 256, sum/2);
			unsigned int g = hist_med(hist_G, 256, sum/2);
			unsigned int b = hist_med(hist_B, 256, sum/2);
	
			GP_PutPixel_Raw_24BPP(dst, x_dst + x, y_dst + y,
			                      GP_Pixel_CREATE_RGB888(r, g, b));

			hist_clear(hist_R, 256);
			hist_clear(hist_G, 256);
			hist_clear(hist_B, 256);
		}
		
		for (x = 0; x < (int)w; x++) {
			int xi = GP_CLAMP(x_src + x - (int)weights->w/2, 0, (int)src->w - 1);
			
			for (y1 = 0; y1 < weights->h; y1++) {
				int yi = GP_CLAMP(y_src + y + (int)y1 - (int)weights->h/2, 0, (int)src->h - 1);

				GP_Pixel pix = GP_GetPixel_Raw_24BPP(src, xi, yi);

				R[y1 * w + x] = GP_Pixel_GET_R_RGB888(pix);
				G[y1 * w + x] = GP_Pixel_GET_G_RGB888(pix);
				B[y1 * w + x] = GP_Pixel_GET_B_RGB888(pix);
			}
		}	
		
		if (GP_ProgressCallbackReport(callback, y, h_src, w_src)) {
			GP_TempAllocFree(temp);
			return 1;
		}
	}

	GP_TempAllocFree(temp);
	GP_ProgressCallbackDone(callback);

	return 0;
}

int GP_FilterWeightedMedianEx(const GP_Context *src,
                              GP_Coord x_src, GP_Coord y_src,
                              GP_Size w_src, GP_Size h_src,
                              GP_Context *dst,
                              GP_Coord x_dst, GP_Coord y_dst,
                              GP_MedianWeights *weights,
                              GP_ProgressCallback *callback)
{
	GP_CHECK(src->pixel_type == dst->pixel_type);
	
	/* Check that destination is large enough */
	GP_CHECK(x_dst + (GP_Coord)w_src <= (GP_Coord)dst->w);
	GP_CHECK(y_dst + (GP_Coord)h_src <= (GP_Coord)dst->h);

	//GP_CHECK(xmed >= 0 && ymed >= 0);

	return GP_FilterWeightedMedian_Raw(src, x_src, y_src, w_src, h_src,
	                                   dst, x_dst, y_dst, weights, callback);
}

GP_Context *GP_FilterWeightedMedianExAlloc(const GP_Context *src,
                                           GP_Coord x_src, GP_Coord y_src,
                                           GP_Size w_src, GP_Size h_src,
                                           GP_MedianWeights *weights,
                                           GP_ProgressCallback *callback)
{
	int ret;

	//GP_CHECK(xmed >= 0 && ymed >= 0);

	GP_Context *dst = GP_ContextAlloc(w_src, h_src, src->pixel_type);

	if (dst == NULL)
		return NULL;

	ret = GP_FilterWeightedMedian_Raw(src, x_src, y_src, w_src, h_src,
	                                  dst, 0, 0, weights, callback);

	if (ret) {
		GP_ContextFree(dst);
		return NULL;
	}

	return dst;
}