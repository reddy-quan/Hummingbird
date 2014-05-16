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
#ifndef  __SPRITE_RES_IMG__
#define  __SPRITE_RES_IMG__



typedef struct boot_global_info
{
    __s32              display_device;       //��ʾ��LCD��TV��HDMI��
    __s32              display_mode;         //��ʾģʽ
    __s32			   erase_flash;          //�Ƿ���Ҫ����nand flash
    char               user_reserved[512];   //�û����������������Զ���
}
boot_global_info_t;


#endif   //__SPRITE_RES_IMG__

