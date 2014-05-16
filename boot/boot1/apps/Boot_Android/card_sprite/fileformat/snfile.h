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
//------------------------------------------------------------------------------------------------------------
//
//2008-10-07 10:22:17
//
//scott
//
//snfile.h
//
//------------------------------------------------------------------------------------------------------------

#ifndef __SN_FILE____H____
#define __SN_FILE____H____

#include "../include/basetypes.h"

#define FLOAT_SIZE			sizeof(float)	//

//------------------------------------------------------------------------------------------------------------
// sn file name
//------------------------------------------------------------------------------------------------------------
//#define SN_FILE_NAME_LEN	128			//sn�ļ�������


// sn file head is same with key file head
#define SN_FILE_HEAD_LEN	1024		//sn�ļ�ͷ������󳤶�


//------------------------------------------------------------------------------------------------------------
// sn head 0
//------------------------------------------------------------------------------------------------------------
#define SN_FILE_CRC_DEF		0x746f6373	//Ĭ�ϵ�crcֵ
#define SN_FILE_CRC_OFF		0			//crc����ƫ����
#define SN_FILE_CRC_LEN		4			//crc���ݳ���


#define SN_MAGIC			0x2e4e532e  //".SN."
#define SN_MAGIC_OFF		4			//magic����ƫ����
#define SN_MAGIC_LEN		4			//magic���ݳ���

//#define SN_HEADLEN		...
#define SN_HEADLEN_OFF		12			//ͷ���ȵ�ƫ����
#define SN_HEADLEN_LEN		4			//ͷ���ȵ����ݳ���

#define SN_HEADOFF_OFF		20			//ͷƫ�Ƶ�ƫ����
#define SN_HEADOFF_LEN		4			//ͷƫ�Ƶ����ݳ���

#define SN_HEAD_START		0x20


//------------------------------------------------------------------------------------------------------------
//sn head
//------------------------------------------------------------------------------------------------------------
#pragma pack(push, 1)
typedef struct tag_SN_HEAD{
	__u16 version;			//�汾
	__u16 size;				//����
	__u32 res0;				//����
	__u32 attr;				//����
	__u32 dataoff;			//��������������ƫ����
	__u32 datalen;			//���ݵĳ���
	__u32 SNCount;			//���ļ����кŵĸ���
	__s32 Year;				//��
	__s32 Month;				//��
	__s32 Day;				//��
	__s32 Hour;				//ʱ
	__s32 Minute;				//��
	__s32 Second;				//��
	__u32 FreeSNIndex;		//��ǰδʹ�õ�SN�������ţ�ȡ��SN����Ҫ�ۼ�.(0 - SNCount-1)
	__u32 res1;
}SN_HEAD_t;
#pragma pack(pop)


#define SN_HEAD_SIZE			sizeof(SN_HEAD_t)	//
#define SN_HEADLEN				SN_HEAD_SIZE		//
//------------------------------------------------------------------------------------------------------------
//sn data
//------------------------------------------------------------------------------------------------------------

//sn item

#define SN_PID_LEN				8
#define SN_SID_LEN				8
#define SN_SNVER_LEN			4
#define SN_GUID_LEN				36
#define SN_ITEM_LEN				128
#define SN_RES_LEN	    		(SN_ITEM_LEN - sizeof(u32) - SN_PID_LEN - SN_SID_LEN - SN_SNVER_LEN - SN_GUID_LEN)

//------------------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------------------
#pragma pack(push, 1)
typedef struct tag_SN_ITEM{
	__u32 ProductNum;           //��Ʒ��ˮ��
	__s8  pid[SN_PID_LEN];		//������
	__s8  sid[SN_SID_LEN];		//����
	__s8  snver[SN_SNVER_LEN];	//snver ���к�Ӧ�ð汾
	__s8  GUID[SN_GUID_LEN];	//GUID
	__u8  res[SN_RES_LEN];		//����
}SN_ITEM_t;
#pragma pack(pop)


#define SN_ITEM_SIZE sizeof(SN_ITEM_t)

//------------------------------------------------------------------------------------------------------------
//sn mark
//------------------------------------------------------------------------------------------------------------

#define SN_MARK_LEN				32
#define MARK_INFO_LEN			(SN_MARK_LEN - sizeof(u32) - sizeof(u32))

//attr
#define ITEM_ATTR_UNUSED		0	//���к�δʹ��
#define ITEM_ATTR_USED			1	//���к�ʹ����
#define ITEM_ATTR_INUSE			2	//���к�����ʹ��
#define ITEM_ATTR_ERROR			3	//���кų���
#define ITEM_ATTR_INVALID		4	//���кŷǷ�

//------------------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------------------
#pragma pack(push, 1)
typedef struct tag_SN_ITEM_MARK{
	__u32 attr;					//���к����� unused, used, inuse, invalid, error
	__u32 value;					//����ֵ
	__s8  info[MARK_INFO_LEN];	//��Ϣ
}SN_ITEM_MARK_t;
#pragma pack(pop)

#define SN_ITEM_MARK_SIZE	sizeof(SN_ITEM_MARK_t)

// sn data length : item = mark
#define SN_LEN			(SN_ITEM_SIZE + SN_ITEM_MARK_SIZE)


#pragma pack(push, 1)
typedef struct tag_SN{
	SN_ITEM_t		item;		//���к�
	SN_ITEM_MARK_t	mark;		//���кű��
}SN_t;
#pragma pack(pop)


//------------------------------------------------------------------------------------------------------------
// encode sn head
//------------------------------------------------------------------------------------------------------------
#define GUID_LEN					36
#define ENCODE_SN_HEAD_LEN			128


#pragma pack(push, 1)
typedef struct tag_ENCODE_SN_HEAD{
	__s8 	GUID[GUID_LEN];				//GUID
	__u32	encodelen;					//���ܺ�ĳ���
	__u32 origionlen;					//����ǰ�ĳ���
	__u32 method;						//�����㷨
	__u8  res[ENCODE_SN_HEAD_LEN - GUID_LEN - 12];//����
}ENCODE_SN_HEAD_t;
#pragma pack(pop)

#define ENCODE_SN_HEAD_SIZE sizeof(ENCODE_SN_HEAD_t)



#endif //__SN_FILE____H____


