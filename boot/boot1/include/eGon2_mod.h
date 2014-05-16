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
#ifndef	_EGON2_MOD_H_
#define	_EGON2_MOD_H_


/* base drivers    */
#define EGON2_EMOD_TYPE_DRV_NOP               0x00        /* 0��������ϵͳ����                                            */
#define EGON2_EMOD_TYPE_DRV_RTC               0x01        /* rtc����type���                                              */
#define EGON2_EMOD_TYPE_DRV_KEY               0x02        /* ��������type���                                             */
#define EGON2_EMOD_TYPE_DRV_DISP              0x03        /* ��ʾ����type���                                             */
#define EGON2_EMOD_TYPE_DRV_MOUSE             0x04        /* �������type���                                             */
#define EGON2_EMOD_TYPE_DRV_TP                0x05        /* touch pannel����type���                                     */
#define EGON2_EMOD_TYPE_DRV_FM                0x07        /* FM radio driver���                                          */
#define EGON2_EMOD_TYPE_DRV_POWER             0x08        /* PWOER    driver���                                          */
/* storage drivers */
#define EGON2_EMOD_TYPE_DRV_NAND              0x09
#define EGON2_EMOD_TYPE_DRV_SDMMC             0x0a
#define EGON2_EMOD_TYPE_DRV_NOR               0x0b
#define EGON2_EMOD_TYPE_DRV_HDD               0x0c
#define EGON2_EMOD_TYPE_DRV_MS                0x0d
/* bus drivers */
#define EGON2_EMOD_TYPE_DRV_IIS               0x10
#define EGON2_EMOD_TYPE_DRV_SPI               0x11
#define EGON2_EMOD_TYPE_DRV_IIC               0x12
#define EGON2_EMOD_TYPE_DRV_IR                0x13
#define EGON2_EMOD_TYPE_DRV_HDMI              0x14
#define EGON2_EMOD_TYPE_DRV_UPRL              0x15
/* usb driver (0~15)                                */
#define EGON2_EMOD_TYPE_DRV_USBD_BASE         0x16
#define EGON2_EMOD_TYPE_DRV_USBH_BASE         0x17
/* new added driver */
#define EGON2_EMOD_TYPE_DRV_MP                0x18


#define EGON2_MOD_ENTER_STANDBY				  0xf0
#define EGON2_MOD_EXIT_STANDBY				  0xf1



#define EMOD_COUNT_MAX                        64

struct eGon2_drv_func
{
    int          (* eGon2_init  )(void                                                                         );
    int          (* eGon2_exit  )(void                                                                         );
    unsigned int (* eGon2_open  )(unsigned int  mid, void   * open_arg                                         );
    int          (* eGon2_close )(unsigned int  hd                                                             );
    unsigned int (* eGon2_read  )(void       *pdata, unsigned int size, unsigned int n  , unsigned int  hd     );
    unsigned int (* eGon2_write )(const void *pdata, unsigned int size, unsigned int n  , unsigned int  hd     );
    int          (* eGon2_ioctl )(unsigned int  hd , unsigned int cmd , signed int aux,   void         *pbuffer);
    int			 (* eGon2_standby)(unsigned int cmd ,void    *pbuffer										   );
};

//eGon2���õ���ģ�����ݽṹ��������������һ����������һ��Ӧ��
typedef struct _eGon2_mod_section
{
    char                     magic[8];                          //MAGIC�ַ������ڱ�ʶ��eGON2������/Ӧ�ô���
    unsigned int             version;                           //�汾����
    unsigned int             mod_id;                            //ģ���ID��ÿ��elf��Ӧ����һ��ģ��ID��������������Ӧ�ã��Ҹ�����ͬ
    int                     (*main)(int argc, char **argv);     //pcb���һ��������׵�ַ
    struct  eGon2_drv_func   demo_func;                         //���������б��г�����������Ҫ��7������
}
eGon2_mod_t;

/*
    Ӧ�ó�����ռ���ڴ�ķ���s

    Ӧ�ó������������ƣ���ʼ��ַ  0x80800000

*/


/*

    ������ռ���ڴ�ķ���

    ��������(ģ��ID)                            �ڴ��ַ                ������С(k)

    EGON2_EMOD_TYPE_DRV_NOP                     0x80900000                < 256
    EGON2_EMOD_TYPE_DRV_RTC                     0x80940000                < 256
    EGON2_EMOD_TYPE_DRV_KEY                     0x80980000                < 256
    EGON2_EMOD_TYPE_DRV_DISP                    0x809c0000                < 256
    EGON2_EMOD_TYPE_DRV_MOUSE                   0x80a00000                < 256
    EGON2_EMOD_TYPE_DRV_TP                      0x80a40000                < 256
    EGON2_EMOD_TYPE_DRV_FM                      0x80a80000                < 256
    EGON2_EMOD_TYPE_DRV_POWER                   0x80ac0000                < 256

::��Ҫ����    EGON2_EMOD_TYPE_DRV_NAND                    0x80b00000                < 256
    EGON2_EMOD_TYPE_DRV_SDMMC                   0x80b40000                < 256
    EGON2_EMOD_TYPE_DRV_HDD                     0x80b80000                < 256
    EGON2_EMOD_TYPE_DRV_MS                      0x80bc0000                < 256

    EGON2_EMOD_TYPE_DRV_IIS                     0x80c00000                < 256
    EGON2_EMOD_TYPE_DRV_SPI                     0x80c40000                < 256
    EGON2_EMOD_TYPE_DRV_IIC                     0x80c80000                < 256
    EGON2_EMOD_TYPE_DRV_IR                      0x80cc0000                < 256
    EGON2_EMOD_TYPE_DRV_HDMI                    0x80d00000                < 256
    EGON2_EMOD_TYPE_DRV_UPRL                    0x80d40000                < 256

    EGON2_EMOD_TYPE_DRV_USBD_BASE               0x80d80000                < 256
    EGON2_EMOD_TYPE_DRV_USBH_BASE               0x80dc0000                < 256

:: �������Ӵ��������ÿ��ռ��512k
    EGON2_EMOD_TYPE_DRV_NAND                    0x80e00000                < 512

::�����ӵ�����
    EGON2_EMOD_TYPE_DRV_MP                      0x80e80000                < 512

*/


#endif  /* _EGON2_MOD_H_ */


