/*
 * (C) Copyright 2007-2012
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Jerry Wang <wangflord@allwinnertech.com>
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

#ifndef  __SW_USB_TYPEDEF_H__
#define  __SW_USB_TYPEDEF_H__


#undef int8
typedef signed char           int8;

#undef int16
typedef signed short          int16;

#undef int32
typedef signed int            int32;

#undef uint8
typedef unsigned char         uint8;

#undef uint16
typedef unsigned short        uint16;

#undef uint32
typedef unsigned int          uint32;

/*
#undef s8
typedef signed char           s8;

#undef s16
typedef signed short          s16;

#undef s32
typedef signed int            s32;

#undef u8
typedef unsigned char         u8;

#undef u16
typedef unsigned short        u16;

#undef u32
typedef unsigned int          u32;

#undef __s8
typedef signed char           __s8;

#undef __s16
typedef signed short          __s16;

#undef __s32
typedef signed int            __s32;

#undef __u8
typedef unsigned char         __u8;

#undef __u16
typedef unsigned short        __u16;

#undef __u32
typedef unsigned int          __u32;

#undef __bool
typedef signed char           __bool;
*/

#if !defined (NULL)
   #define      NULL    0
#endif

#undef  __hdle
typedef unsigned int        __hdle;

/* ����ĳ��bitλΪ1 */
#undef  x_set_bit
#define x_set_bit( value, bit )      		( (value) |=  ( 1U << (bit) ) )

/* ��ĳ��bitλ���� */
#undef  x_clear_bit
#define x_clear_bit( value, bit )    		( (value) &= ~( 1U << (bit) ) )

/* ��ĳ��bitλ��ֵȡ�� */
#undef  x_reverse_bit
#define x_reverse_bit( value, bit )  		( (value) ^=  ( 1U << (bit) ) )

/* �ж�ĳ��bitλ�Ƿ�Ϊ1 */
#undef  x_test_bit
#define x_test_bit( value, bit )     		( (value)  &  ( 1U << (bit) ) )

/* ȡ��Сֵ */
#undef  x_min
#define x_min( x, y )          				( (x) < (y) ? (x) : (y) )

/* ȡ���ֵ */
#undef  x_max
#define x_max( x, y )          				( (x) > (y) ? (x) : (y) )

/* ȡ����ֵ */
#undef  x_absolute
#define x_absolute(p)        				((p) > 0 ? (p) : -(p))

#endif   //__SW_USB_TYPEDEF_H__

