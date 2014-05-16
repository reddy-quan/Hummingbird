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
#ifndef  __UI_H__
#define  __UI_H__

#include  "types.h"
#include  "display_interface/display_interface.h"

#define BAS_SCREEN_SIZE                (3072000)
#define DECOMPRESS_UI_PIC_MAX           8
#define FILE_NAME_MAX                   16

typedef struct __display_info_set
{
    __u8   *screen_buf;                       //�洢��Ļ��ַ
    __u32   screen_height;                    //�洢��Ļ�߶ȣ��ַ���λ����
    __u32   screen_width;                     //�洢��Ļ���
    __u8   *crt_addr;                         //��ǰ������ʾ�ĵ�ַ
    __u8   *fb_addr;                          //ͼ���ַ
    __u32   rest_screen_height;               //ʣ��Ĵ洢��Ļ�߶ȣ�ʣ���ܸ߶�, �ַ���λ����
    __u32   rest_screen_width;                //ʣ��Ĵ洢��Ļ���, ʣ���ܿ��, �ַ���λ����
    __u32   rest_display_height;              //ʣ�����ʾ�߶�
    __u32   total_height;                     //������ʾ�ܵĸ߶�
    __u32   current_height;                   //��ǰ�Ѿ�ʹ�õĸ߶�
    __u32   x;                                //��ʾλ�õ�x����
    __u32   y;                                //��ʾλ�õ�y����
}
display_info_set_t;


typedef struct _shell_char_t
{
    char    name;                             //�ַ���ʹ����32λ������ʵ����һ��char����
    __u32   x;                                //��ʾ�ַ���x����
    __u32   y;                                //��ʾ�ַ���y����
    __u32   width;                            //�ַ�����ʾ���
    __u32   *addr;                            //�ַ���ʾʱ�����ʼ��ַ
}
_shell_char_set_t;

typedef struct _debug_shell_t
{
    __u32                index;               //��¼��ǰ���ڴ�ӡ���ַ������
    __u32                tail;                //ĩβ�ַ���ţ������ڵ��ַ�
    _shell_char_set_t    str[260];            //�������ڴ�ӡ���ַ�
}
_debug_shell_set_t;



typedef struct _cursor_t
{
    volatile __u32       x;                   //����x����
    volatile __u32       y;                   //����y����

}
_cursor_set_t;


typedef  __disp_layer_info_t   display_layer_info_t;


extern   __u32   WORD_SIZE;
//---------------------------------------------------------------
//  �� ����
//---------------------------------------------------------------
#define  BOOT_PICTURE_TYPE_NULL         0x00
#define  BOOT_PICTURE_TYPE_BMP          0x01
#define  BOOT_PICTURE_TYPE_JPG          0x02

extern   int  LCD_DisplayInit(void);
extern   int  LCD_Change_frame_buffer(__u8 *addr);
extern   void debug_printf( const char * str, ...);
extern   int  debug_setdisplay_color(__u32 user_set_color);
extern   int  debug_setdisplay_clear(void);

#endif   //__UI_H__

