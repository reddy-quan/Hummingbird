/*
************************************************************************************************************************
*                                                         eGON
*                                         the Embedded GO-ON Bootloader System
*
*                             Copyright(C), 2006-2009, SoftWinners Microelectronic Co., Ltd.
*											       All Rights Reserved
*
* File Name   : load_boot1_from_sdmmc.c
*
* Author      : Gary.Wang
*
* Version     : 1.1.0
*
* Date        : 2009.12.08
*
* Description :
*
* Others      : None at present.
*
*
* History     :
*
*  <Author>        <time>       <version>      <description>
*
* Gary.Wang      2009.12.08       1.1.0        build the file
*
************************************************************************************************************************
*/
#include "boot0_i.h"
#include "bsp_mmc_for_boot/bsp_sdc_for_boot.h"
#include "load_boot1_from_sdmmc.h"

extern const boot0_file_head_t  BT0_head;
/*******************************************************************************
*��������: load_boot1_from_sdmmc
*����ԭ�ͣ�int32 load_boot1_from_sdmmc( __u8 card_no )
*��������: ��һ�ݺõ�Boot1��sdmmc flash�����뵽SRAM�С�
*��ڲ���: void
*�� �� ֵ: OK                         ���벢У��ɹ�
*          ERROR                      ���벢У��ʧ��
*��    ע:
*******************************************************************************/
__s32 load_boot1_from_sdmmc( char *buf)
{
    __u32  length;
    __s32  card_no, index;
	boot_file_head_t  *bfh;
	boot_sdcard_info_t  *sdcard_info = (boot_sdcard_info_t *)buf;

	card_no = BT0_head.boot_head.platform[0];
	msg("boot card number =%d\n", card_no);
	//for(i=0;i<4;i++)
	{
		/* open sdmmc */

		index= (card_no==0) ? 0 : 1;
		card_no = sdcard_info->card_no[index];
//		if(card_no < 0)
//		{
//			msg("bad card number %d in card boot\n", card_no);

//			continue;
//		}
	//	msg("sdcard %d line count =%d\n", card_no, sdcard_info->line_count[index] );
		//msg("sdcard %d line sel =%d\n", card_no, sdcard_info->line_sel[index] );
		if(!sdcard_info->line_sel[index])
		{
			sdcard_info->line_sel[index] = 4;
		}
		if( SDMMC_PhyInit( card_no, sdcard_info->line_sel[index] ) == -1)   //���ٿ���4������
		{
			msg("Fail in Init sdmmc.\n");
			goto __card_op_fail__;
		}
		msg("sdcard %d init ok\n", card_no);
		/* load 1k boot1 head */
	    if( SDMMC_PhyRead( BOOT1_START_SECTOR_IN_SDMMC, 1024/512, (void *)BOOT1_BASE, card_no ) != (1024/512))
		{
			msg("Fail in reading boot1 head.\n");
			goto __card_op_fail__;
		}
		/* check head */
		if( check_magic( (__u32 *)BOOT1_BASE, BOOT1_MAGIC ) != CHECK_IS_CORRECT )
		{
			msg("ERROR! NOT find the head of Boot1.\n");
			goto __card_op_fail__;
		}
		/* check length */
		bfh = (boot_file_head_t *) BOOT1_BASE;
	    length =  bfh->length;
		msg("The size of Boot1 is %x.\n", length );
	    if( ( length & ( SF_ALIGN_SIZE - 1 ) ) != 0 )
	    {
	    	msg("boot0 length is NOT align.\n");
	    	goto __card_op_fail__;
	    }
	    if( SDMMC_PhyRead( BOOT1_START_SECTOR_IN_SDMMC, length/512, (void *)BOOT1_BASE, card_no )!= (length/512))
		{
			msg("Fail in reading boot1 head.\n");
			goto __card_op_fail__;
		}
		/* ���У��� */
	    if( check_sum( (__u32 *)BOOT1_BASE, length ) != CHECK_IS_CORRECT )
	    {
	        msg("Fail in checking boot1.\n");
	       	goto __card_op_fail__;
	    }

		if(card_no == 0)
		{
			bfh->eGON_vsn[2] = 1;
		}
		else
		{
			bfh->eGON_vsn[2] = 2;
		}
	    bfh->eGON_vsn[0] = (card_no & 0xff);
	    bfh->eGON_vsn[1] = (card_no >> 16) & 0xff;
		msg("Succeed in loading boot1 from sdmmc flash.\n");

		SDMMC_PhyExit( card_no );

		return OK;

__card_op_fail__:
		SDMMC_PhyExit(card_no );

	//	continue;
	}

	return ERROR;
}

