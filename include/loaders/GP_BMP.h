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

#ifndef LOADERS_GP_BMP_H
#define LOADERS_GP_BMP_H

#include "core/GP_Context.h"
#include "core/GP_ProgressCallback.h"

/*
 * Opens up a bmp file, checks signature, parses metadata.
 *
 * The width and height and pixel type are filled upon succcessful return.
 */
GP_RetCode GP_OpenBMP(const char *src_path, FILE **f,
                      GP_Size *w, GP_Size *h, GP_PixelType *pixel_type);

/*
 * Reads a BMP from a opened file.
 * 
 * Upon successful return context to store bitmap is allocated and image is
 * loaded.
 *
 */
GP_RetCode GP_ReadBMP(FILE *f, GP_Context **res,
                      GP_ProgressCallback *callback);

/*
 * Does both GP_OpenBMP and GP_ReadBMP.
 */
GP_RetCode GP_LoadBMP(const char *src_path, GP_Context **res,
                      GP_ProgressCallback *callback);


#endif /* LOADERS_GP_BMP_H */