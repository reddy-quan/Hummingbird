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
#ifndef  __BOOT_HARDWARE_RES_H__
#define  __BOOT_HARDWARE_RES_H__

#include  "types.h"

#define    CARD_SPRITE_SUCCESSED               1
#define    CARD_SPRITE_FAIL					  -1
#define    CARD_SPRITE_NORMAL				   0

typedef struct
{
    unsigned    layer_hd;                     //������ʾ��ͼ����
    unsigned    disp_hd;                      //��ʾ�������
    unsigned    led_hd[32];                   //����LED����ʾ�����32��
    unsigned    tmr_hd;                       //TIMER���
    int			led_count;					  //ʵ�ʵ�LED�ĸ���
    int         led_status[32];               //ÿ��LED��Ӧ��״̬
    int         display_source;
}
boot_hardware_res;

extern  boot_hardware_res     board_res;

#endif   //__BOOT_HARDWARE_RES_H__

