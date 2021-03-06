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

#include <string.h>

#include "core/GP_Debug.h"

#include "GP_FilterParam.h"

GP_FilterParam *GP_FilterParamCreate(GP_PixelType pixel_type)
{
	GP_FilterParam *ret;

	ret = malloc((GP_PixelTypes[pixel_type].numchannels + 1)
	             * sizeof(GP_FilterParam));

	if (ret == NULL) {
		GP_WARN("Malloc Failed");
		return NULL;
	}

	GP_FilterParamInitChannels(ret, pixel_type);

	return ret;
}

void GP_FilterParamDestroy(GP_FilterParam *self)
{
	free(self);
}

static unsigned int count_channels(GP_FilterParam params[])
{
	unsigned int i = 0;

	while (params[i].channel_name[0] != '\0')
		i++;

	return i;
}

GP_FilterParam *GP_FilterParamChannel(GP_FilterParam params[],
                                      const char *channel_name)
{
	unsigned int i;

	for (i = 0; params[i].channel_name[0] != '\0'; i++)
		if (!strcmp(params[i].channel_name, channel_name))
			return &params[i];

	return NULL;
}

uint32_t GP_FilterParamChannels(GP_FilterParam params[])
{
	return count_channels(params);
}

int GP_FilterParamCheckPixelType(GP_FilterParam params[],
                                 GP_PixelType pixel_type)
{
	unsigned int i, num_channels;
	const GP_PixelTypeChannel *channels;

	num_channels = GP_PixelTypes[pixel_type].numchannels;
	channels     = GP_PixelTypes[pixel_type].channels;

	i = count_channels(params);

	if (i != num_channels)
		return 1;

	for (i = 0; i < num_channels; i++)
		if (GP_FilterParamChannel(params, channels[i].name) == NULL)
			return 1;

	return 0;
}

int GP_FilterParamCheckChannels(GP_FilterParam params[],
                                const char *channel_names[])
{
	unsigned int i;

	for (i = 0; channel_names[i] != NULL; i++)
		if (GP_FilterParamChannel(params, channel_names[i]) == NULL)
			return 1;

	if (i != count_channels(params))
		return 1;

	return 0;
}

void GP_FilterParamInitChannels(GP_FilterParam params[],
                                GP_PixelType pixel_type)
{
	unsigned int i, num_channels;
	const GP_PixelTypeChannel *channels;

	num_channels = GP_PixelTypes[pixel_type].numchannels;
	channels     = GP_PixelTypes[pixel_type].channels;

	for (i = 0; i < num_channels; i++) {
		strcpy(params[i].channel_name, channels[i].name);
		memset(&params[i].val, 0, sizeof(GP_FilterParamVal));
	}

	params[i].channel_name[0] = '\0';
}

void GP_FilterParamSetIntAll(GP_FilterParam params[],
                             int32_t val)
{
	unsigned int i;

	for (i = 0; params[i].channel_name[0] != '\0'; i++)
		params[i].val.i = val;
}

int GP_FilterParamSetInt(GP_FilterParam params[], const char *channel_name,
                         int32_t val)
{
	GP_FilterParam *param;
	param = GP_FilterParamChannel(params, channel_name);

	if (param == NULL)
		return 1;

	param->val.i = val;
	return 0;
}

void GP_FilterParamSetFloatAll(GP_FilterParam params[],
                               float val)
{
	unsigned int i;

	for (i = 0; params[i].channel_name[0] != '\0'; i++)
		params[i].val.f = val;
}

int GP_FilterParamSetFloat(GP_FilterParam params[], const char *channel_name,
                         float val)
{
	GP_FilterParam *param;
	param = GP_FilterParamChannel(params, channel_name);

	if (param == NULL)
		return 1;

	param->val.f = val;
	return 0;
}

void GP_FilterParamSetUIntAll(GP_FilterParam params[],
                              uint32_t val)
{
	unsigned int i;

	for (i = 0; params[i].channel_name[0] != '\0'; i++)
		params[i].val.ui = val;
}

int GP_FilterParamSetUInt(GP_FilterParam params[], const char *channel_name,
                          uint32_t val)
{
	GP_FilterParam *param;
	param = GP_FilterParamChannel(params, channel_name);

	if (param == NULL)
		return 1;

	param->val.ui = val;
	return 0;
}

void GP_FilterParamSetPtrAll(GP_FilterParam params[],
                             void *ptr)
{
	unsigned int i;

	for (i = 0; params[i].channel_name[0] != '\0'; i++)
		params[i].val.ptr = ptr;
}

int GP_FilterParamSetPtr(GP_FilterParam params[], const char *channel_name,
                         void *ptr)
{
	GP_FilterParam *param;
	param = GP_FilterParamChannel(params, channel_name);

	if (param == NULL)
		return 1;

	param->val.ptr = ptr;
	return 0;
}

void GP_FilterParamFreePtrAll(GP_FilterParam params[])
{
	unsigned int i;

	for (i = 0; params[i].channel_name[0] != '\0'; i++)
		free(params[i].val.ptr);
}

void GP_FilterParamPrintInt(GP_FilterParam params[])
{
	unsigned int i;

	for (i = 0; params[i].channel_name[0] != '\0'; i++)
		printf("Chann '%s' = %i\n", params[i].channel_name, params[i].val.i);
}

void GP_FilterParamPrintUInt(GP_FilterParam params[])
{
	unsigned int i;

	for (i = 0; params[i].channel_name[0] != '\0'; i++)
		printf("Chann '%s' = %u\n", params[i].channel_name, params[i].val.ui);
}

void GP_FilterParamPrintFloat(GP_FilterParam params[])
{
	unsigned int i;

	for (i = 0; params[i].channel_name[0] != '\0'; i++)
		printf("Chann '%s' = %f\n", params[i].channel_name, params[i].val.f);
}

void GP_FilterParamPrintPtr(GP_FilterParam params[])
{
	unsigned int i;

	for (i = 0; params[i].channel_name[0] != '\0'; i++)
		printf("Chann '%s' = %p\n", params[i].channel_name, params[i].val.ptr);
}
