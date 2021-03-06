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
 * Copyright (C) 2009-2012 Cyril Hrubis <metan@ucw.cz>                       *
 *                                                                           *
 *****************************************************************************/

 /*

   This example shows how to mix SDL with GFXprim.

  */

#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <GP.h>
#include <backends/GP_SDL_Context.h>

#define W 320
#define H 240

static SDL_Surface *display = NULL;
static GP_Context context;

static GP_Pixel black_pixel, darkgray_pixel;

void redraw_screen(void)
{
	SDL_LockSurface(display);

	GP_Fill(&context, black_pixel);

	GP_Line(&context, 0, 0, W-1, H-1, darkgray_pixel);
	GP_Line(&context, 0, H-1, W-1, 0, darkgray_pixel);

	SDL_UnlockSurface(display);
}

void event_loop(void)
{
	SDL_Event event;

	while (SDL_WaitEvent(&event) > 0) {
		switch (event.type) {

		case SDL_VIDEOEXPOSE:
			redraw_screen();
			SDL_Flip(display);
		break;

		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
			case SDLK_ESCAPE:
				return;
			default:
			break;
			}
		break;

		case SDL_QUIT:
			return;
		}
	}
}

int main(void)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
		fprintf(stderr, "Could not initialize SDL: %s\n", SDL_GetError());
		return 1;
	}

	display = SDL_SetVideoMode(W, H, 0, SDL_SWSURFACE);

	if (display == NULL) {
		fprintf(stderr, "Could not open display: %s\n", SDL_GetError());
		goto fail;
	}

	GP_ContextFromSurface(&context, display);

	black_pixel     = GP_ColorToContextPixel(GP_COL_BLACK, &context);
	darkgray_pixel  = GP_ColorToContextPixel(GP_COL_GRAY_DARK, &context);

	redraw_screen();
	SDL_Flip(display);

	event_loop();

	SDL_Quit();
	return 0;

fail:
	SDL_Quit();
	return 1;
}

