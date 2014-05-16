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

#ifndef  __USBC_I_H__
#define  __USBC_I_H__

#include "usb_bsp.h"

#define  USBC_MAX_OPEN_NUM    8

/* ��¼USB�Ĺ�����Ϣ */
typedef struct __fifo_info{
    __u32 port0_fifo_addr;
	__u32 port0_fifo_size;

    __u32 port1_fifo_addr;
	__u32 port1_fifo_size;

	__u32 port2_fifo_addr;
	__u32 port2_fifo_size;
}__fifo_info_t;

/* ��¼��ǰUSB port���е�Ӳ����Ϣ */
typedef struct __usbc_otg{
    __u32 port_num;
	__u32 base_addr;        /* usb base address 		*/

	__u32 used;             /* �Ƿ����ڱ�ʹ��   		*/
    __u32 no;               /* �ڹ��������е�λ�� 		*/
}__usbc_otg_t;

#endif   //__USBC_I_H__

