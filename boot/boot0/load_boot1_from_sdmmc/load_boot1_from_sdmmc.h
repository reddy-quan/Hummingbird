/*
************************************************************************************************************************
*                                                         eGON
*                                         the Embedded GO-ON Bootloader System
*
*                             Copyright(C), 2006-2009, SoftWinners Microelectronic Co., Ltd.
*											       All Rights Reserved
*
* File Name   : load_boot1_from_spinor.h
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
#ifndef  __load_boot1_from_sdmmc_h
#define  __load_boot1_from_sdmmc_h


/*******************************************************************************
*��������: load_boot1_from_spinor
*����ԭ�ͣ�int32 load_boot1_from_spinor( void )
*��������: ��һ�ݺõ�Boot1��spi nor flash�����뵽SRAM�С�
*��ڲ���: void
*�� �� ֵ: OK                         ���벢У��ɹ�
*          ERROR                      ���벢У��ʧ��
*��    ע:
*******************************************************************************/
extern __s32 load_boot1_from_sdmmc( char *buf);



#endif     //  ifndef __load_boot1_from_spi_nor_h

/* end of load_boot1_from_spinor.h */
