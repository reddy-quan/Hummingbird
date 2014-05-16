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
#ifndef  __boo1_h
#define  __boo1_h


#include "egon_def.h"


#define BOOT1_MAGIC                     "eGON.BT1"
#define MAGIC_FLAG	                    "PHOENIX_CARD_IMG"
/****************************************************************************
*                                                                           *
*     ����ĺ궨�Ǹ� NAND ʹ��                                              *
*                                                                           *
*                                                                           *
*****************************************************************************/
#define BOOT1_START_BLK_NUM             BLKS_FOR_BOOT0

#define BLKS_FOR_BOOT1_IN_16K_BLK_NF    32
#define BLKS_FOR_BOOT1_IN_32K_BLK_NF    8
#define BLKS_FOR_BOOT1_IN_128K_BLK_NF   5
#define BLKS_FOR_BOOT1_IN_256K_BLK_NF   5
#define BLKS_FOR_BOOT1_IN_512K_BLK_NF   5
#define BLKS_FOR_BOOT1_IN_1M_BLK_NF     5
#define BLKS_FOR_BOOT1_IN_2M_BLK_NF     5
#define BLKS_FOR_BOOT1_IN_4M_BLK_NF     5
/****************************************************************************
*                                                                           *
*     ����ĺ궨�Ǹ� SDMMC ʹ��                                             *
*                                                                           *
*                                                                           *
*****************************************************************************/
/* ���ڿ������������ĺ궨�� */
#define BOOT1_START_SECTOR_IN_SDMMC    38192
#define BOOTIMG_SECTOR_IN_SDMMC        40240         //epdk100.img���ڿ��ϵĴ��λ��(����)

//�̶����ɸı�
#define BOOT1_SDMMC_START_ADDR         38192
/****************************************************************************
*                                                                           *
*     ����ĺ궨�Ǹ� NOR ʹ��                                               *
*                                                                           *
*                                                                           *
*****************************************************************************/
#define BOOT0_START_SECTOR_IN_SPINOR      0          //��bromָ��
#define BOOT1_START_SECTOR_IN_SPINOR     128          //�ճ�64k��boot0ʹ�ã�ͬʱ��Լnor�ռ�
#define BOOT1_SPI_NOR_START_ADDR        ( BOOT0_SPI_NOR_START_ADDR + SPI_NOR_SIZE_FOR_BOOT0 )       // add for spi nor. by Gary, 2009-12-8 11:47:17
#define SPI_NOR_SIZE_FOR_BOOT1          ( SZ_64K * 6 )                                              // add for spi nor. by Gary, 2009-12-8 11:47:17
/****************************************************************************
*                                                                           *
*     ����ĺ궨�Ǹ� COMMON ʹ��                                            *
*                                                                           *
*                                                                           *
*****************************************************************************/

/* work mode */
#define WORK_MODE_DEVELOP	0x01	//���ڿ���
#define WORK_MODE_TEST		0x02	//���ڲ���
#define WORK_MODE_PRODUCT	0x04	//��������
#define WORK_MODE_UPDATE	0x08	//��������

#define BOOT_SCRIPT_BUFFER_SIZE           (32 * 1024)
//����洢����
#define   START_FROM_NAND_FLASH       0
#define   START_FROM_SDCARD           1
#define   START_FROM_NOR_FLASH        2

//NAND������ݽṹ
typedef struct boot1_nand_para_set
{
    __u32  nand_good_block_ratio;
    __u32  reserved[15];
}boot1_nand_para_set_t;

typedef struct _boot_nand_connect_info_t
{
	__u8  id[8];
	__u8  chip_cnt;
	__u8  chip_connect;
	__u8  rb_cnt;
	__u8  rb_connect;
	__u32 good_block_ratio;
}boot_nand_connect_info_t;


//SD��������ݽṹ
typedef struct _boot_sdcard_info_t
{
	__s32               card_ctrl_num;                //�ܹ��Ŀ��ĸ���
	__s32				boot_offset;                  //ָ��������֮���߼�����������Ĺ���
	__s32 				card_no[4];                   //��ǰ�����Ŀ���, 16-31:GPIO��ţ�0-15:ʵ�ʿ����������
	__s32 				speed_mode[4];                //�����ٶ�ģʽ��0�����٣�����������
	__s32				line_sel[4];                  //�������ƣ�0: 1�ߣ�������4��
	__s32				line_count[4];                //��ʹ���ߵĸ���
}
boot_sdcard_info_t;


typedef struct _boot_core_para_t
{
    __u32  user_set_clock;                 // ����Ƶ�� M��λ
    __u32  user_set_core_vol;              // ���ĵ�ѹ mV��λ
    __u32  vol_threshold;                  // �������޵�ѹ
}
boot_core_para_t;
/******************************************************************************/
/*                              file head of Boot1                            */
/******************************************************************************/
typedef struct _boot1_private_head_t
{
	__u32              prvt_head_size;
	__u8               prvt_head_vsn[4];                // the version of Boot1_private_hea
	__s32						uart_port;              // UART���������
	normal_gpio_cfg             uart_ctrl[2];           // UART������(���Դ�ӡ��)GPIO��Ϣ
	boot_dram_para_t      		dram_para;              // dram init para
	char                        script_buf[32 * 1024];  // �ű�����
	boot_core_para_t            core_para;              // �ؼ�����
	__s32						twi_port;               // TWI���������
	normal_gpio_cfg             twi_ctrl[2];            // TWI������GPIO��Ϣ�����ڿ���TWI
	__s32						debug_enable;           // debugʹ�ܲ���
    __s32                       hold_key_min;           // hold key��Сֵ
    __s32                       hold_key_max;           // hold key���ֵ
    __u32                       work_mode;              // ģʽ������������������
    __u32                       storage_type;           // �洢��������  0��nand   1��sdcard    2: spinor
    normal_gpio_cfg             storage_gpio[32];       // �洢�豸 GPIO��Ϣ
    char                        storage_data[512 - sizeof(normal_gpio_cfg) * 32];      // �û�����������Ϣ
    //boot_nand_connect_info_t    nand_connect_info;    // nand ����
}boot1_private_head_t;

typedef struct _boot1_file_head_t
{
	boot_file_head_t      boot_head;
	boot1_private_head_t  prvt_head;
}boot1_file_head_t;


#endif     //  ifndef __boo1_h

/* end of boo1.h */
