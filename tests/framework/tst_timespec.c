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

#include "tst_timespec.h"

#define NSEC_IN_SEC 1000000000

void timespec_diff(int *sec, int *nsec,
                   const struct timespec *start,
                   const struct timespec *stop)
{
	if (stop->tv_nsec < start->tv_nsec) {
		*sec  = stop->tv_sec - start->tv_sec - 1;
		*nsec = stop->tv_nsec + 1000000000 - start->tv_nsec;
	} else {
		*sec  = stop->tv_sec  - start->tv_sec;
		*nsec = stop->tv_nsec - start->tv_nsec;
	}
}

double timespec_to_double(const struct timespec *t)
{
	double res;
	
	res = t->tv_sec;
	res *= NSEC_IN_SEC;
	res += t->tv_nsec;

	return res;
}

void double_to_timespec(const double time, struct timespec *res)
{
	res->tv_sec = time / NSEC_IN_SEC;
	res->tv_nsec = time - res->tv_sec * NSEC_IN_SEC;
}

void timespec_sub(const struct timespec *a, const struct timespec *b,
                  struct timespec *res)
{
	res->tv_sec = a->tv_sec - b->tv_sec;
	time_t nsec = a->tv_nsec;

	if (b->tv_nsec > a->tv_nsec) {
		res->tv_sec--;
		nsec += NSEC_IN_SEC;
	}

	res->tv_nsec = nsec - b->tv_nsec;
}

void timespec_add(const struct timespec *a, struct timespec *res)
{
	res->tv_sec  += a->tv_sec;
	res->tv_nsec += a->tv_nsec;

	if (res->tv_nsec >= NSEC_IN_SEC) {
		res->tv_sec += res->tv_nsec / NSEC_IN_SEC;
		res->tv_nsec %= NSEC_IN_SEC;
	}
}

void timespec_div(struct timespec *res, unsigned int div)
{
	long sec = res->tv_sec;

	res->tv_sec /= div;
	sec %= div;

	res->tv_nsec = (sec * NSEC_IN_SEC + res->tv_nsec)/div;
}
