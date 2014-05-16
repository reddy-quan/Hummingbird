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

#ifndef __DISP_LCD_H__
#define __DISP_LCD_H__

#include "disp_display_i.h"


__s32 Disp_lcdc_init(__u32 sel);
__s32 Disp_lcdc_exit(__u32 sel);

#ifdef __LINUX_OSAL__
__s32 Disp_lcdc_event_proc(__s32 irq, void *parg);
#else
__s32 Disp_lcdc_event_proc(void *parg);
#endif
__s32 Disp_lcdc_pin_cfg(__u32 sel, __disp_output_type_t out_type, __u32 bon);
__u32 Disp_get_screen_scan_mode(__disp_tv_mode_t tv_mode);

__u32 tv_mode_to_width(__disp_tv_mode_t mode);
__u32 tv_mode_to_height(__disp_tv_mode_t mode);
__u32 vga_mode_to_width(__disp_vga_mode_t mode);
__u32 vga_mode_to_height(__disp_vga_mode_t mode);

void LCD_delay_ms(__u32 ms) ;
void LCD_delay_us(__u32 ns);

extern void LCD_get_panel_funs_0(__lcd_panel_fun_t * fun);
extern void LCD_get_panel_funs_1(__lcd_panel_fun_t * fun);

#endif
