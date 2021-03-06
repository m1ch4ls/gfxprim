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
 * Copyright (C) 2009-2013 Cyril Hrubis <metan@ucw.cz>                       *
 *                                                                           *
 *****************************************************************************/

/*
 * The ellipse drawing algorithm.
 *
 * The ellipse is drawn from top to down. We start at the top, and iterate X
 * until we accumulate enough change in Y to pass to the next line. In each
 * step (both in X and Y), four pixels are drawn. (The ellipse is symmetrical
 * along X and Y axis so we can mirror each pixel and save computations.)
 *
 * The math:
 * From the canonical implicit equation of an ellipse:
 *
 * x^2/a^2 + y^2/b^2 = 1             and therefore
 * x^2*b^2 + y^2*a^2 - a^2*b^2 = 0
 *
 * which has an exact solution for a non-integer x.
 * For an integer approximation, we need to find
 * an integer x so that
 *
 * x^2*b^2 + y^2*a^2 - a^2*b^2 = error
 *
 * where error is as close to 0 as possible.
 *
 * Optimization:
 * We can save a significant amount of multiplications
 * by calculating next error values from the previous ones.
 * For error(x+1):
 *
 * error(x+1) = (x+1)^2*b^2 + y^2*a^2 + a^2*b^2
 *
 * which can be rewritten as (after expanding (x+1)^2):
 *
 * error(x+1) = x^2*b^2 + 2*x^2*b^2 + b^2 + y^2*a^2 + a^2*b^2
 *
 * and, after substituting error(x) which we already know:
 *
 * error(x+1) = error(x) + 2*x*b^2 + b^2
 *
 * The same applies to x-1, y+1 and y-1.
 */

/*
 * This macro defines an ellipse drawing function.
 * Arguments:
 *     CONTEXT_T - user-defined type of drawing context (passed to PUTPIXEL)
 *     PIXVAL_T  - user-defined pixel value type (passed to PUTPIXEL)
 *     PUTPIXEL  - a pixel drawing function f(context, x, y, pixval)
 *     FN_NAME   - name of the function to be defined
 */
#define DEF_ELLIPSE_FN(FN_NAME, CONTEXT_T, PIXVAL_T, PUTPIXEL) \
static void FN_NAME(CONTEXT_T context, int xcenter, int ycenter, \
	unsigned int a, unsigned int b, PIXVAL_T pixval) \
{ \
	/* Precompute quadratic terms. */ \
	int a2 = a*a; \
	int b2 = b*b; \
\
	int x, y, error; \
\
	if (a == 0) { \
		for (y = -b; y <= (int)b; y++) \
			PUTPIXEL(context, xcenter, ycenter + y, pixval); \
		return; \
	} \
\
	if (b == 0) { \
		for (x = -a; x <= (int)a; x++) \
			PUTPIXEL(context, xcenter + x, ycenter, pixval); \
		return; \
	} \
\
	for (x = 0, error = -b2*a, y = b; y >= 0; y--) { \
		while (error < 0) { \
\
			/* Calculate error(x+1) from error(x). */ \
			error += 2*x*b2 + b2; \
			x++; \
\
			PUTPIXEL(context, xcenter-x+1, ycenter-y, pixval); \
			PUTPIXEL(context, xcenter+x-1, ycenter-y, pixval); \
			PUTPIXEL(context, xcenter-x+1, ycenter+y, pixval); \
			PUTPIXEL(context, xcenter+x-1, ycenter+y, pixval); \
		} \
\
		/* Calculate error(y-1) from error(y). */ \
		error += -2*y*a2 + a2; \
\
		PUTPIXEL(context, xcenter-x+1, ycenter-y, pixval); \
		PUTPIXEL(context, xcenter+x-1, ycenter-y, pixval); \
		PUTPIXEL(context, xcenter-x+1, ycenter+y, pixval); \
		PUTPIXEL(context, xcenter+x-1, ycenter+y, pixval); \
	} \
}
