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

#ifndef GP_LINE_H
#define GP_LINE_H

#include "core/GP_Context.h"

void GP_Line8bpp(GP_Context *context, GP_Coord x0, GP_Coord y0,
                 GP_Coord x1, GP_Coord y1, GP_Pixel pixel);

void GP_Line16bpp(GP_Context *context, GP_Coord x0, GP_Coord y0,
                  GP_Coord x1, GP_Coord y1, GP_Pixel pixel);

void GP_Line24bpp(GP_Context *context, GP_Coord x0, GP_Coord y0,
                  GP_Coord x1, GP_Coord y1, GP_Pixel pixel);

void GP_Line32bpp(GP_Context *context, GP_Coord x0, GP_Coord y0,
                  GP_Coord x1, GP_Coord y1, GP_Pixel pixel);

void GP_Line(GP_Context *context, GP_Coord x0, GP_Coord y0,
             GP_Coord x1, GP_Coord y1, GP_Pixel pixel);

void GP_TLine(GP_Context *context, GP_Coord x0, GP_Coord y0,
              GP_Coord x1, GP_Coord y1, GP_Pixel pixel);

#endif /* GP_LINE_H */
