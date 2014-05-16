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
#ifndef   __CARD_SPRITE_FLASH__
#define   __CARD_SPRITE_FLASH__


extern    int   update_boot0_info (void *buf0, char *buf);
extern    int   update_boot1_info (void *buf0, char *buf);

extern    int   sprite_flash_init (int *type);
extern    int   sprite_flash_exit (int type);

extern    int   sprite_flash_open (void);
extern    int   sprite_flash_close(void);
extern    int   sprite_flash_read (__u32 nSectNum,  __u32 nSectorCnt, void * pBuf);
extern    int   sprite_flash_write(__u32 nSectNum,  __u32 nSectorCnt, void * pBuf);


extern    int 	sprite_flash_hardware_scan(void *mbr_i,void *flash_info, int erase_flash);

extern    int   create_stdmbr(void *mbr_i);

#endif



