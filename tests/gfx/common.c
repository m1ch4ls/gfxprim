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

#include <stdio.h>

#include "common.h"

static void dump_buffer(const char *pattern, int x, int y)
{
	int i, j;

	for (i = 0; i < y; i++) {
		for (j = 0; j < x; j++)
			printf("%2x ", pattern[j + i * y]);
		printf("\n");
	}
}

void dump_buffers(const char *pattern, const GP_Context *c)
{
	printf("Expected pattern\n");
	dump_buffer(pattern, c->w, c->h);
	printf("Rendered pattern\n");
	dump_buffer((char*)c->pixels, c->w, c->h);
}

int compare_buffers(const char *pattern, const GP_Context *c)
{
	GP_Size x, y;
	int err = 0;

	for (x = 0; x < c->w; x++) {
		for (y = 0; y < c->h; y++) {
			if (pattern[x + y * c->h] !=
			    ((char*)c->pixels)[x + y * c->h]) {
				err++;
			}
		}
	}

	if (err)
		dump_buffers(pattern, c);	

	return err;
}