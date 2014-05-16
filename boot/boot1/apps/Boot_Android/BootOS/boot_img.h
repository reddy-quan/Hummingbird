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
#ifndef  __BOOT_IMG__
#define  __BOOT_IMG__


#define    BOOT_OS_MELIS                1
#define    BOOT_OS_LINUX                2
#define    BOOT_OS_WINCE                3
#define    BOOT_OS_ANDROID              4


typedef struct _boot_img_cfg
{
    char  file_name[20];
    char  file_name_bk[20];
    __u32 base_addr;
    __u32 used_size;
    __u32 full_size;
}
boot_img_cfg_t;

typedef struct boot_img_t
{
    char               img_name[32];         //32���ֽ����������ļ����ƣ�������ȫ·��
    __u32              img_size;             //������󳤶�
    __u32              img_full_size;        //����ʵ�ʳ���
    __u32              img_base;             //�����ŵ��ڴ��е�λ��
}
boot_img_set_t;

typedef struct  boot_sys_img_t
{
    char               os_name[32];          //OS����
    __s32              start_mode;           //����ģʽ������ 0:���վ����ļ��ķ�ʽ���� 1:���շ������ݵķ�ʽ����
    boot_img_set_t     img_set[16];          //ָ����
    boot_img_set_t     script_info;          //ָ��ű���Ϣ
    __u32              img_count;            //��ǰOS��ӵ�еľ������
    char               logo_name[32];        //������ʾlogo��·��
    __s32              logo_show;            //�Ƿ���Ҫ��ʾ����LOGO
    __u32              logo_address;         //����logo��ַ
    __s32			   logo_off;             //����ϵͳǰ�Ƿ���Ҫ�ر�LOGO
}
boot_sys_img_set_t;

typedef struct boot_global_info
{
    __s32              os_count;             //�ܹ���OS����
    char               os_name[8][32];       //OS����, 0����û��ѡ��1����MELIS��2����LINUX��3����WINCE
    char               start_os_name[32];    //��ǰ������OS������
    __s32              start_os_index;       //��ǰ������OS���±�
    __s32              last_os_name;         //�ϴ�������OS
    __s32              user_set_timeout;     //����Ƕ�ϵͳ����������ϵͳѡ�����ʱʱ��
    __s32              display_device;       //��ʾ��LCD��TV��HDMI��
    __s32              display_mode;         //��ʾģʽ
    pic_name_info_t    os_pic[8];            //OS
}
boot_global_info_t;


#endif   //__BOOT_IMG__

