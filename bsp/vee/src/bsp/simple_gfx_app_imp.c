/*
 * Copyright 2022 NXP
 * Copyright 2025 MicroEJ Corp. This file has been modified by MicroEJ Corp.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <math.h>

#define AMP_ORANGE 30
#define AMP_BLUE   75
#define AMP_GREEN  30

int Java_com_nxp_example_simplegfx_SimpleGFXNatives_getOrangeRectangleX(int t, int rectangleLength, int rectangleHeight, int displaySize)
{
	int centre = displaySize / 2;
	double sine_orange = AMP_ORANGE * cos(t * M_PI / 45.0);
	return centre - rectangleLength - rectangleLength / 2 - AMP_ORANGE - (int)sine_orange;
}

int Java_com_nxp_example_simplegfx_SimpleGFXNatives_getOrangeRectangleY(int t, int rectangleLength, int rectangleHeight, int displaySize)
{
	int centre = displaySize / 2;
	return centre - rectangleHeight / 2;
}

int Java_com_nxp_example_simplegfx_SimpleGFXNatives_getBlueRectangleX(int t, int rectangleLength, int rectangleHeight, int displaySize)
{
	int centre = displaySize / 2;
	return centre - rectangleLength / 2;
}

int Java_com_nxp_example_simplegfx_SimpleGFXNatives_getBlueRectangleY(int t, int rectangleLength, int rectangleHeight, int displaySize)
{
	int centre = displaySize / 2;
	double sine_blue = AMP_BLUE * sin(t * M_PI / 45.0);
	return centre - rectangleHeight / 2 + (int)sine_blue;
}

int Java_com_nxp_example_simplegfx_SimpleGFXNatives_getGreenRectangleX(int t, int rectangleLength, int rectangleHeight, int displaySize)
{
	int centre = displaySize / 2;
	double sine_green = AMP_GREEN * cos(t * M_PI / 45.0);
	return centre + rectangleLength - rectangleLength / 2 + AMP_GREEN + (int)sine_green;
}

int Java_com_nxp_example_simplegfx_SimpleGFXNatives_getGreenRectangleY(int t, int rectangleLength, int rectangleHeight, int displaySize)
{
	int centre = displaySize / 2;
	return centre - rectangleHeight / 2;
}
