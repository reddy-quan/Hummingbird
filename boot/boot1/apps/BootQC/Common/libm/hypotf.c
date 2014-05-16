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

float hypotf(float x, float y)
{
	float a = x, b = y, t1, t2, y1, y2, w;
	s32_t j, k, ha, hb;

	GET_FLOAT_WORD(ha, x);
	ha &= 0x7fffffff;
	GET_FLOAT_WORD(hb, y);
	hb &= 0x7fffffff;
	if (hb > ha)
	{
		a = y;
		b = x;
		j = ha;
		ha = hb;
		hb = j;
	}
	else
	{
		a = x;
		b = y;
	}
	SET_FLOAT_WORD(a, ha);
	SET_FLOAT_WORD(b, hb);
	if ((ha - hb) > 0xf000000)
	{
		return a + b;
	}
	k = 0;
	if (ha > 0x58800000)
	{
		if (ha >= 0x7f800000)
		{
			w = a + b;
			if (ha == 0x7f800000)
				w = a;
			if (hb == 0x7f800000)
				w = b;
			return w;
		}
		ha -= 0x22000000;
		hb -= 0x22000000;
		k += 68;
		SET_FLOAT_WORD(a, ha);
		SET_FLOAT_WORD(b, hb);
	}
	if (hb < 0x26800000)
	{
		if (hb <= 0x007fffff)
		{
			if (hb == 0)
				return a;
			SET_FLOAT_WORD(t1, 0x7e800000);
			b *= t1;
			a *= t1;
			k -= 126;
		}
		else
		{
			ha += 0x22000000;
			hb += 0x22000000;
			k -= 68;
			SET_FLOAT_WORD(a, ha);
			SET_FLOAT_WORD(b, hb);
		}
	}
	w = a - b;
	if (w > b)
	{
		SET_FLOAT_WORD(t1, ha&0xfffff000);
		t2 = a - t1;
		w = sqrtf(t1 * t1 - (b * (-b) - t2 * (a + t1)));
	}
	else
	{
		a = a + a;
		SET_FLOAT_WORD(y1, hb&0xfffff000);
		y2 = b - y1;
		SET_FLOAT_WORD(t1, ha+0x00800000);
		t2 = a - t1;
		w = sqrtf(t1 * y1 - (w * (-w) - (t1 * y2 + t2 * b)));
	}
	if (k != 0)
	{
		SET_FLOAT_WORD(t1, 0x3f800000+(k<<23));
		return t1 * w;
	}
	else
		return w;
}

