/*
* (C) Copyright 2007-2013
* Allwinner Technology Co., Ltd. <www.allwinnertech.com>
* Martin zheng <zhengjiewen@allwinnertech.com>
*
* See file CREDITS for list of people who contributed to this
* project.
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation; either version 2 of
* the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston,
* MA 02111-1307 USA
*/
#include <math.h>

static const double
	tiny = 1.0e-300,
	zero = 0.0,
	pi_o_4 = 7.8539816339744827900E-01,
	pi_o_2 = 1.5707963267948965580E+00,
	pi = 3.1415926535897931160E+00,
	pi_lo = 1.2246467991473531772E-16;

double atan2(double y, double x)
{
	double z;
	s32_t k, m, hx, hy, ix, iy;
	u32_t lx, ly;

	EXTRACT_WORDS(hx,lx,x);
	ix = hx & 0x7fffffff;
	EXTRACT_WORDS(hy,ly,y);
	iy = hy & 0x7fffffff;
	if (((ix | ((lx | -lx) >> 31)) > 0x7ff00000) ||
		((iy | ((ly | -ly) >> 31)) > 0x7ff00000))
		return x + y;
	if (((hx - 0x3ff00000) | lx) == 0)
		return atan(y);
	m = ((hy >> 31) & 1) | ((hx >> 30) & 2);

	if ((iy | ly) == 0)
	{
		switch (m)
		{
		case 0:
		case 1:
			return y;
		case 2:
			return pi + tiny;
		case 3:
			return -pi - tiny;
		}
	}

	if ((ix | lx) == 0)
		return (hy < 0) ? -pi_o_2 - tiny : pi_o_2 + tiny;

	if (ix == 0x7ff00000)
	{
		if (iy == 0x7ff00000)
		{
			switch (m)
			{
			case 0:
				return pi_o_4 + tiny;
			case 1:
				return -pi_o_4 - tiny;
			case 2:
				return 3.0 * pi_o_4 + tiny;
			case 3:
				return -3.0 * pi_o_4 - tiny;
			}
		}
		else
		{
			switch (m)
			{
			case 0:
				return zero;
			case 1:
				return -zero;
			case 2:
				return pi + tiny;
			case 3:
				return -pi - tiny;
			}
		}
	}

	if (iy == 0x7ff00000)
		return (hy < 0) ? -pi_o_2 - tiny : pi_o_2 + tiny;

	k = (iy - ix) >> 20;
	if (k > 60)
		z = pi_o_2 + 0.5 * pi_lo;
	else if (hx < 0 && k < -60)
		z = 0.0;
	else
		z = atan(fabs(y / x));
	switch (m)
	{
	case 0:
		return z;
	case 1:
	{
		u32_t zh;
		GET_HIGH_WORD(zh,z);
		SET_HIGH_WORD(z,zh ^ 0x80000000);
	}
		return z;
	case 2:
		return pi - (z - pi_lo);
	default:
		return (z - pi_lo) - pi;
	}
}
