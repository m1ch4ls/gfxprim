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
 * Copyright (C) 2009-2011 Cyril Hrubis <metan@ucw.cz>                       *
 *                                                                           *
 *****************************************************************************/

/*

  GP_Context interpolations.

  Nearest Neighbour
  ~~~~~~~~~~~~~~~~~

  Fast, but produces pixelated images. Works however well for images with sharp
  edges mostly consisting of big one color regions (eg doesn't blur the
  result on upscaling).

  Bilinear
  ~~~~~~~~

  Faster than Bicubic, but less precise.

  Bilinear LF
  ~~~~~~~~~~~

  Bilinear with low-pass filter on downscaling, this is the best choice for
  fast up and downscaling.

  Bicubic
  ~~~~~~~

  Works well for upscaling as is. To get decent result on downscaling,
  low-pass filter (for example gaussian blur) must be used on original image
  before scaling is done.

 */

#ifndef FILTERS_GP_RESIZE_H
#define FILTERS_GP_RESIZE_H

#include "GP_Filter.h"

/* Nearest Neighbour */
int GP_FilterResizeNN(const GP_Context *src, GP_Context *dst,
                      GP_ProgressCallback *callback);

GP_Context *GP_FilterResizeNNAlloc(const GP_Context *src,
                                   GP_Size w, GP_Size h,
                                   GP_ProgressCallback *callback);


typedef enum GP_InterpolationType {
	GP_INTERP_NN,            /* Nearest Neighbour                         */
	GP_INTERP_LINEAR_INT,    /* Bilinear - fixed point arithmetics        */
	GP_INTERP_LINEAR_LF_INT, /* Bilinear + low pass filter on downscaling */
	GP_INTERP_CUBIC,         /* Bicubic                                   */
	GP_INTERP_CUBIC_INT,     /* Bicubic - fixed point arithmetics         */
	GP_INTERP_MAX = GP_INTERP_CUBIC_INT,
} GP_InterpolationType;

const char *GP_InterpolationTypeName(enum GP_InterpolationType interp_type);

/*
 * Just interpolate the source context into destination context.
 */
int GP_FilterResize_Raw(const GP_Context *src, GP_Context *dst,
                        GP_InterpolationType type,
                        GP_ProgressCallback *callback);

/*
 * If destination is non NULL, the w and h are used to create subcontext from
 * destination which is then used to interpolate the image to.
 *
 * Otherwise if destination is NULL, the context of size w and h is allocated
 * and returned.
 *
 * In both cases the pointer to destination or NULL in case of failure is
 * returned.
 */
GP_Context *GP_FilterResize(const GP_Context *src, GP_Context *dst,
                            GP_InterpolationType type,
                            GP_Size w, GP_Size h,
                            GP_ProgressCallback *callback);

#endif /* FILTERS_GP_RESIZE_H */
