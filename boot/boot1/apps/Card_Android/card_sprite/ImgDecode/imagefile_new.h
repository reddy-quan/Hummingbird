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
//----------------------------------------------------------------------------------------------------------//
//                                                                                                          //
//                                                                                                          //
//                                                                                                          //
//                                                                                                          //
//    I MMMMMMMMMMI                                                                                         //
//      I FF      BBI                                                                                       //
//      I FF      I EE  FFMMFFI MMMMLL  I MMMMMMEE          EEMMBBLLBBFF    FFMMMMBBI   I MMBBBBMMMMI       //
//      I FF        EEI   I BBBBI   I     I     LLFF      EELL  I BBFF    FFFF    I BBI   I BBI   I EE      //
//      I FF        EEI   I FF          I BBMMMMMMFF      FF      I FF  I EE        I LL  I FF      EEI     //
//      I FF      I EE    I FF          EEI     I FF      FF      I FF  I EE        I LL  I FF      EEI     //
//      I FF      FFI     I FF          EEI     BBFF      FFI     EEFF    FFI       FFI   I FF      EEI     //
//    I MMMMMMMMMMFF    BBMMMMMMMMFF    LLMMMMMMFFMMFF    I BBMMMMFFFF    I BBMMMMMMLL  I MMMMFF  BBMMBBI   //
//                                                                I FF                                      //
//                                                                FFLL                                      //
//                                                          BBMMMMEE                                        //
//                                                                                                          //
//                                                                                                          //
//----------------------------------------------------------------------------------------------------------//
//                                                                                                          //
//                                              Phonex System                                               //
//                                                                                                          //
//                               (c) Copyright 2011-2012, Sam Liu 																					//
//                                                                                                          //
//                                           All Rights Reserved                                            //
//                                                                                                          //
// File    : imagefile.h                                                                                    //
// By      : Sam Liu                                                                                        //
// Version : V3.00                                                                                          //
// Time    : 2011-07-02 10:35:12                                                                             //
//                                                                                                          //
//----------------------------------------------------------------------------------------------------------//
//                                                                                                          //
// HISTORY                                                                                                  //
//                                                                                                          //
// 1 2011-07-02 10:35:12	                                                                                  //
//                                                                                                          //
//                                                                                                          //
//                                                                                                          //
//----------------------------------------------------------------------------------------------------------//

#ifndef __IMAGE_FORMAT__H__
#define __IMAGE_FORMAT__H__	1

#include  "../card_sprite_i.h"
//#define IMAGE_VER	100
//------------------------------------------------------------------------------------------------------------
#define IMAGE_MAGIC			"IMAGEWTY"
#define	IMAGE_HEAD_VERSION	0x00000300

#define IMAGE_HEAD_SIZE     	  1024
#define IMAGE_ITEM_TABLE_SIZE     1024
//------------------------------------------------------------------------------------------------------------
///Image�ļ�ͷ���ݽṹ
//------------------------------------------------------------------------------------------------------------
#pragma pack(push, 1)
typedef struct tag_ImageHead
{
	u8	magic[8];		//IMAGE_MAGIC
	u32 version;		//���ṹ�İ汾�ţ�IMAGE_HEAD_VERSION
	u32	size;			//���ṹ�ĳ���
	u32 attr;			//���ṹ�����ԣ���ʽ����version��ȷ�������ܣ�ѹ����
	u32 imagever;		//image�İ汾���ɽű�ָ��
	u32 lenLo;			//image�ļ����ܳ��� ��λ
	u32 lenHi;			//image�ļ����ܳ��� ��λ
	u32	align;			//���ݵĶ���߽磬ȱʡ1024
	u32 pid;			//PID��Ϣ
	u32 vid;			//VID��Ϣ
	u32 hardwareid; 	//Ӳ��ƽ̨ID
	u32 firmwareid; 	//���ƽ̨ID
	u32 itemattr;		//item�������,"����"
	u32	itemsize;		//item������Ĵ�С
	u32	itemcount;		//item������ĸ���
	u32	itemoffset;		//item��ƫ����
	u32	imageattr;		//image�ļ�����
	u32 appendsize;		//�������ݵĳ���
	u32 appendoffsetLo;	//�������ݵ�ƫ����
	u32 appendoffsetHi;	//�������ݵ�ƫ����
	u8  reserve[980];	//Ԥ��
}ImageHead_t;
#pragma pack(pop)


//------------------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------------------
#define IMAGE_ALIGN_SIZE				0x400
#define HEAD_ATTR_NO_COMPRESS 	0x4d //1001101

//------------------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------------------
#pragma pack(push, 1)
typedef struct tagImageHeadAttr{
	u32	res		: 12;
	u32 len		: 8;
	u32 encode	: 7;		///HEAD_ATTR_NO_COMPRESS
	u32 compress: 5;
}ImageHeadAttr_t;
#pragma pack(pop)

//------------------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------------------
#define	IMAGE_ITEM_VERSION	0x00000100
#define MAINTYPE_LEN		8
#define SUBTYPE_LEN			16
#define FILE_PATH			256
#define IMAGE_ITEM_RCSIZE   640 // ������Ԥ����С


//------------------------------------------------------------------------------------------------------------
///���������ݽṹ
//------------------------------------------------------------------------------------------------------------
#pragma pack(push, 1)
typedef struct tag_ImageItem
{
	u32 version;				//���ṹ�İ汾��IMAGE_ITEM_VERSION
	u32	size;					//���ṹ�ĳ���
	u8	mainType[MAINTYPE_LEN];	//�������ļ�������
	u8	subType[SUBTYPE_LEN];	//���������ͣ�Ĭ����image���ýű�ָ��
	u32	attr;					//�������ļ�������,��ʽ����version��ȷ�������ܣ�ѹ����
	u8	name[FILE_PATH];		//�ļ����� 260
	u32	datalenLo;				//�ļ�������image�ļ��еĳ���
	u32	datalenHi;				//��λ �ļ�������image�ļ��еĳ���
	u32 filelenLo;				//�ļ���ʵ�ʳ���
	u32 filelenHi;				//��λ �ļ���ʵ�ʳ���
	u32 offsetLo;				//�ļ���ʼλ��ƫ����
	u32 offsetHi;				//��λ �ļ���ʼλ��ƫ����
	u8	encryptID[64];			//���ܲ��ID��������ļ������ܣ����ֶ�"\0"��ʾ������
	u32 checksum;				//�������ļ���У���
	u8	res[IMAGE_ITEM_RCSIZE];	//����
}ImageItem_t;

#pragma pack(pop)

//------------------------------------------------------------------------------------------------------------
// THE END !
//------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------
// THE END !
//------------------------------------------------------------------------------------------------------------

#endif //__IMAGE_FORMAT__H__

