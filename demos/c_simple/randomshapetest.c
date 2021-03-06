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

#include <stdio.h>
#include <stdlib.h>

#include <GP.h>

static GP_Backend *win;

/* Globally used colors. */
static GP_Pixel white, black;

/* Holding flag (pauses drawing). */
static int pause_flag = 0;

/* Shape to draw */
#define SHAPE_FIRST	1
#define SHAPE_CIRCLE	1
#define SHAPE_ELLIPSE	2
#define SHAPE_TRIANGLE	3
#define SHAPE_RECTANGLE	4
#define SHAPE_TETRAGON   5
#define SHAPE_POLYGON	6
#define SHAPE_RECTANGLE_AA 7
#define SHAPE_LAST	7
static int shape = SHAPE_FIRST;

/* Draw outlines? */
static int outline_flag = 0;

/* Draw filled shapes? */
static int fill_flag = 1;

/* Do a clipping test? */
static int cliptest_flag = 0;

void random_point(const GP_Context *c, int *x, int *y)
{
	if (cliptest_flag) {
		*x = random() % (3*c->w) - c->w;
		*y = random() % (3*c->h) - c->h;
	} else {
		*x = random() % c->w;
		*y = random() % c->h;
	}
}

void random_point_AA(const GP_Context *c, int *x, int *y)
{
	*x = random() % (c->w<<8);
	*y = random() % (c->h<<8);
}

void draw_random_circle(GP_Pixel pixel)
{
	int x, y;
	random_point(win->context, &x, &y);
	int r = random() % 50;

	if (fill_flag)
		GP_FillCircle(win->context, x, y, r, pixel);

	if (outline_flag)
		GP_Circle(win->context, x, y, r, white);
}

void draw_random_ellipse(GP_Pixel pixel)
{
	int x, y;
	random_point(win->context, &x, &y);
	int rx = random() % 50;
	int ry = random() % 50;

	if (fill_flag)
		GP_FillEllipse(win->context, x, y, rx, ry, pixel);

	if (outline_flag)
		GP_Ellipse(win->context, x, y, rx, ry, white);
}

void draw_random_triangle(GP_Pixel pixel)
{
	int x0, y0, x1, y1, x2, y2;
	random_point(win->context, &x0, &y0);
	random_point(win->context, &x1, &y1);
	random_point(win->context, &x2, &y2);

	if (fill_flag)
		GP_FillTriangle(win->context, x0, y0, x1, y1, x2, y2, pixel);

	if (outline_flag)
		GP_Triangle(win->context, x0, y0, x1, y1, x2, y2, white);
}

void draw_random_rectangle(GP_Pixel pixel)
{
	int x0, y0, x1, y1;
	random_point(win->context, &x0, &y0);
	random_point(win->context, &x1, &y1);

	if (fill_flag)
		GP_FillRect(win->context, x0, y0, x1, y1, pixel);

	if (outline_flag)
		GP_Rect(win->context, x0, y0, x1, y1, white);
}

void draw_random_tetragon(GP_Pixel pixel)
{
	int x0, y0, x1, y1, x2, y2, x3, y3;
	random_point(win->context, &x0, &y0);
	random_point(win->context, &x1, &y1);
	random_point(win->context, &x2, &y2);
	random_point(win->context, &x3, &y3);

	if (fill_flag)
		GP_FillTetragon(win->context, x0, y0, x1, y1, x2, y2, x3, y3, pixel);

	if (outline_flag)
		GP_Tetragon(win->context, x0, y0, x1, y1, x2, y2, x3, y3, pixel);
}

void draw_random_polygon(GP_Pixel pixel)
{
	GP_Coord xy[10];
	int i;

	for (i = 0; i < 5; i++) {
		random_point(win->context, xy + 2*i, xy + 2*i + 1);
	}

	GP_FillPolygon_Raw(win->context, 5, xy, pixel);
}

void draw_random_rectangle_AA(GP_Pixel pixel)
{
	int x0, y0, x1, y1;
	random_point_AA(win->context, &x0, &y0);
	random_point_AA(win->context, &x1, &y1);

//	if (fill_flag)
		GP_FillRect_AA(win->context, x0, y0, x1, y1, pixel);

//	if (outline_flag)
//		GP_Rect(win->context, x0, y0, x1, y1, white);
}


void clear_screen(void)
{
	GP_Fill(win->context, black);
	GP_BackendFlip(win);
}

void redraw_screen(void)
{
	if (pause_flag)
		return;

	/* Pick a random color for drawing. */
	GP_Pixel pixel;
	pixel = GP_RGBToPixel(random() % 256, random() % 256,
	                      random() % 256, win->context->pixel_type);

	switch (shape) {
	case SHAPE_CIRCLE:
		draw_random_circle(pixel);
	break;
	case SHAPE_ELLIPSE:
		draw_random_ellipse(pixel);
	break;
	case SHAPE_TRIANGLE:
		draw_random_triangle(pixel);
	break;
	case SHAPE_RECTANGLE:
		draw_random_rectangle(pixel);
	break;
	case SHAPE_TETRAGON:
		draw_random_tetragon(pixel);
	break;
	case SHAPE_POLYGON:
		draw_random_polygon(pixel);
	break;
	case SHAPE_RECTANGLE_AA:
		draw_random_rectangle_AA(pixel);
	break;
	}
}

void event_loop(void)
{
	GP_Event ev;

	while (GP_BackendGetEvent(win, &ev)) {
		switch (ev.type) {
		case GP_EV_KEY:
			if (ev.code != GP_EV_KEY_DOWN)
				continue;

			switch (ev.val.key.key) {
			case GP_KEY_SPACE:
				shape++;
				if (shape > SHAPE_LAST)
					shape = SHAPE_FIRST;
				clear_screen();
				pause_flag = 0;
			break;
			case GP_KEY_P:
				pause_flag = !pause_flag;
			break;
			case GP_KEY_F:
				fill_flag = !fill_flag;
				if (!fill_flag && !outline_flag)
					outline_flag = 1;
			break;
			case GP_KEY_O:
				outline_flag = !outline_flag;
				if (!fill_flag && !outline_flag)
					fill_flag = 1;
			break;
			case GP_KEY_C:
				cliptest_flag = !cliptest_flag;
			break;
			case GP_KEY_X:
				clear_screen();
			break;
			case GP_KEY_ESC:
				GP_BackendExit(win);
				exit(0);
			break;
			}
		}
	}
}

int main(void)
{
	const char *backend_opts = "X11";

	win = GP_BackendInit(backend_opts, "Random Shape Test", stderr);

	if (win == NULL) {
		fprintf(stderr, "Failed to initalize backend '%s'\n",
		        backend_opts);
		return 1;
	}

	white = GP_ColorToContextPixel(GP_COL_WHITE, win->context);
	black = GP_ColorToContextPixel(GP_COL_BLACK, win->context);

	for (;;) {
		GP_BackendPoll(win);
		event_loop();

		usleep(20000);

		if (pause_flag)
			continue;

		redraw_screen();
		GP_BackendFlip(win);
	}
}

