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
#include "dec_img.h"


#include "../imgencodedecode/encode.h"
#include "../fileformat/imagefile.h"				//private head file

#define ITEM_PHOENIX_TOOLS 	  "PXTOOLS "


#define min( x, y )          ( (x) < (y) ? (x) : (y) )
#define max( x, y )          ( (x) > (y) ? (x) : (y) )

#define HEAD_ID				0		//ͷ���ܽӿ�����
#define TABLE_ID			1		//����ܽӿ�����
#define DATA_ID				2		//���ݼ��ܽӿ�����
#define IF_CNT				3		//���ܽӿڸ���	����ֻ��ͷ���ܣ�����ܣ����ݼ���3��
#define	MAX_KEY_SIZE 		32		//���볤��

#pragma pack(push, 1)
typedef struct tag_IMAGE_HANDLE
{

	CSzFile      * fp;			//�ļ����

	ImageHead_t  ImageHead;		//imgͷ��Ϣ

	ImageItem_t *ItemTable;		//item��Ϣ��

	RC_ENDECODE_IF_t rc_if_decode[IF_CNT];//���ܽӿ�
	
	__bool			bWithEncpy; // �Ƿ����
	
}IMAGE_HANDLE;

#define INVALID_INDEX		0xFFFFFFFF

#pragma pack(push, 1)
typedef struct tag_ITEM_HANDLE{
	__u32	index;					//��ItemTable�е�����
	__u64 pos;
}ITEM_HANDLE;
#pragma pack(pop)

static  __u8 *buffer_encode;
static  int   buffer_encode_size;

//------------------------------------------------------------------------------------------------------------
//
// ����˵��
//
//
// ����˵��
//
//
// ����ֵ
//
//
// ����
//    ��
//
//------------------------------------------------------------------------------------------------------------
HIMAGE 	Img_Open	(__u32 start_sector, int size)
{
	IMAGE_HANDLE  *pImage;
	ImageHead_t ImageHead;
	char	seed[]	= "img";
	char key_buff[MAX_KEY_SIZE];
	__u32 key_len = MAX_KEY_SIZE;
	__u32 ItemTableSize;
	__u8 * ItemTableBuf, * pItemTableDecode;
	__u32 i;

    pImage = (IMAGE_HANDLE *)wBoot_malloc(sizeof(IMAGE_HANDLE));
    if(!pImage)
    {
        __wrn("DEC_IMG: fail to get memory for pImage\n");

        return NULL;
    }
	memset(pImage, 0, sizeof(IMAGE_HANDLE));
	buffer_encode_size = size;
	//------------------------------------------------
	//��ʼ�����ܽӿ�
	//------------------------------------------------
	for (i = 0; i< IF_CNT; i++)
	{
		pImage->rc_if_decode[i].handle 		= NULL;
		pImage->rc_if_decode[i].Initial		= Initial;
		pImage->rc_if_decode[i].EnDecode	= Decode;
		pImage->rc_if_decode[i].UnInitial	= UnInitial;
		memset(key_buff, i, key_len);		//ǰ������ݳ�ʼ��Ϊ0 note
		key_buff[key_len - 1] = seed[i];	//���һ���ֽ��޸�    note

		pImage->rc_if_decode[i].handle = pImage->rc_if_decode[i].Initial(key_buff, &key_len, i);
		if (NULL == pImage->rc_if_decode[i].handle)
		{
		    wBoot_free(pImage);
		    __wrn("DEC_IMG: pImage->rc_if_decode[i].handle is NULL\n");

			return NULL;
		}
	}
	//------------------------------------------------
	//��img�ļ�
	//------------------------------------------------
	pImage->fp = File_Open(start_sector, 0, 4, 0);
	if (NULL == pImage->fp)
	{
	    wBoot_free(pImage);
		__wrn("DEC_IMG: open img file failed\n");

		return NULL;
	}

	//------------------------------------------------
	//��imgͷ
	//------------------------------------------------
	if(File_Read(&ImageHead, sizeof(ImageHead_t), 1, pImage->fp))
	{
        
	    wBoot_free(pImage);
		__wrn("DEC_IMG: read imagehead fail!\n");

        return  NULL;

    }
     
	if(memcmp(ImageHead.magic, IMAGE_MAGIC, 8) == 0)
	{
		pImage->bWithEncpy = 0;
       // __wrn("DEC_IMG: no encpy used!\n");
	}
	else
	{
		pImage->bWithEncpy = 1;
       // __wrn("DEC_IMG:  encpy used!\n");
	}
	
	if(pImage->bWithEncpy)
	{
    	__u8 * pHead = (__u8 *) &ImageHead;
    	__u8 * pHeadDecode = (__u8 *)&pImage->ImageHead;
    	__u8 * pin, *pout;

    	for (i = 0; i < sizeof(ImageHead_t); i+= ENCODE_LEN)
    	{
    		pin = pHead + i;
    		pout= pHeadDecode + i;
    		if (OK  != pImage->rc_if_decode[HEAD_ID].EnDecode(pImage->rc_if_decode[HEAD_ID].handle, pin , pout))
    		{
    		    wBoot_free(pImage);
    			File_Close(pImage->fp);
    			__wrn("DEC_IMG: decode img file head failed\n");

    			return NULL;
    		}
    	}
    }
    else
    {
        memcpy((void *)(&(pImage->ImageHead)), (void*)&ImageHead, sizeof(ImageHead_t));
    }
	//------------------------------------------------
	//�Ƚ�magic
	//------------------------------------------------
	//__wrn("DEC_IMG: img header magic:%s\n",pImage->ImageHead.magic);
	if (memcmp(pImage->ImageHead.magic, IMAGE_MAGIC, 8))
	{
	    wBoot_free(pImage);
		File_Close(pImage->fp);
		__wrn("DEC_IMG: img header magic error\n");

		return NULL;
	}
	ItemTableSize = pImage->ImageHead.itemcount * sizeof(ImageItem_t);
	pImage->ItemTable = (ImageItem_t*)wBoot_malloc(ItemTableSize);
	if(!pImage->ItemTable)
	{
	    wBoot_free(pImage);
		File_Close(pImage->fp);
		__wrn("DEC_IMG: fail to get memory for ItemTable\n");

		return NULL;
	}

	//------------------------------------------------
	//read ItemTable
	//------------------------------------------------
	ItemTableBuf = (__u8 *)wBoot_malloc(ItemTableSize);
	if(!ItemTableBuf)
	{
	    wBoot_free(pImage->ItemTable);
	    wBoot_free(pImage);
		File_Close(pImage->fp);
		__wrn("DEC_IMG: fail to get memory for ItemTableBuf\n");

		return NULL;
	}
  //  __wrn("DEC_IMG: pImage->ImageHead.itemoffset=0x%x\n",pImage->ImageHead.itemoffset);
  // __wrn("DEC_IMG: pImage->ImageHead.itemSize=0x%x\n",pImage->ImageHead.itemsize);
  //  __wrn("DEC_IMG: pImage->ImageHead.itemcount=%d\n",pImage->ImageHead.itemcount);
    
	File_Seek(pImage->fp, pImage->ImageHead.itemoffset, SZ_SEEK_SET);
	if(File_Read(ItemTableBuf, ItemTableSize, 1, pImage->fp))
	{
		__wrn("DEC_IMG: read ItemTable failed\n");
	    return 0;
	}
	File_Seek(pImage->fp, 0, SZ_SEEK_CUR);
	//------------------------------------------------
	// decode ItemTable
	//------------------------------------------------
	pItemTableDecode = (__u8 *)pImage->ItemTable;
    if(pImage->bWithEncpy)
	{
	    __u8 *pin, *pout;

    	for (i = 0; i < ItemTableSize; i+= ENCODE_LEN)
    	{
    		pin = ItemTableBuf + i;
    		pout= pItemTableDecode + i;

    		if (OK  != pImage->rc_if_decode[TABLE_ID].EnDecode(pImage->rc_if_decode[TABLE_ID].handle, pin , pout))
    		{
    		    wBoot_free(pImage->ItemTable);
    		    wBoot_free(ItemTableBuf);
    			File_Close(pImage->fp);
    			wBoot_free(pImage);
    			__wrn("DEC_IMG: decode img file ItemTable failed\n");

    			return NULL;
    		}
    	}
    }else
	{
	
		memcpy(pItemTableDecode, ItemTableBuf, ItemTableSize);
	
	}
	
    wBoot_free(ItemTableBuf);

    buffer_encode = (__u8 *)wBoot_malloc(buffer_encode_size);
    if(!buffer_encode)
    {
        wBoot_free(pImage->ItemTable);
	    wBoot_free(ItemTableBuf);
		File_Close(pImage->fp);
		wBoot_free(pImage);
		__wrn("DEC_IMG: fail to get memory for buffer_encode\n");

		return NULL;
    }

	return pImage;
}



//------------------------------------------------------------------------------------------------------------
//
// ����˵��
//
//
// ����˵��
//
//
// ����ֵ
//
//
// ����
//    ��
//
//------------------------------------------------------------------------------------------------------------
HIMAGEITEM 	Img_OpenItem	(HIMAGE hImage, char * MainType, char * subType)
{
	IMAGE_HANDLE* pImage = (IMAGE_HANDLE *)hImage;
	ITEM_HANDLE * pItem  = NULL;
	__u32 i;

	pItem = (ITEM_HANDLE *)wBoot_malloc(sizeof(ITEM_HANDLE));
	if(!pItem)
	{
		__wrn("DEC_IMG: fail to get memory for pItem\n");
	    return NULL;
	}

	pItem->index = INVALID_INDEX;
	pItem->pos = 0;

	for (i = 0; i < pImage->ImageHead.itemcount ; i++)
	{
    //  __wrn("pImage->ItemTable[%d].mainType=%s,subType=%s\n",i,pImage->ItemTable[i].mainType,\
    //    pImage->ItemTable[i].subType);
        if (memcmp(ITEM_PHOENIX_TOOLS, MainType, MAINTYPE_LEN) == 0)//
		{
			if (memcmp(MainType, pImage->ItemTable[i].mainType, MAINTYPE_LEN) == 0 )
			{
				pItem->index = i;
				return pItem;
			}
		}
		else if (memcmp(MainType, pImage->ItemTable[i].mainType, MAINTYPE_LEN) == 0 &&
		         memcmp(subType,  pImage->ItemTable[i].subType,  SUBTYPE_LEN)  == 0)
		{
			pItem->index = i;
			return pItem;
		}
	}

	wBoot_free(pItem);
	pItem = NULL;

	return NULL;
}



//------------------------------------------------------------------------------------------------------------
//
// ����˵��
//
//
// ����˵��
//
//
// ����ֵ
//
//
// ����
//    ��
//
//------------------------------------------------------------------------------------------------------------
__u64 Img_GetItemSize	(HIMAGE hImage, HIMAGEITEM hItem)
{
    IMAGE_HANDLE *pImage = (IMAGE_HANDLE *)hImage;
    ITEM_HANDLE  *pItem  = (ITEM_HANDLE  *)hItem;
    __u64      nItemSize;

 	nItemSize  = pImage->ItemTable[pItem->index].filelenLo;
    nItemSize |= (__u64)pImage->ItemTable[pItem->index].filelenHi << 32;
//	return pImage->ItemTable[pItem->index].filelen;
  	return nItemSize;
    //�����е�һ����ܣ���˶��ᰴ�շ������
}


//------------------------------------------------------------------------------------------------------------
//
// ����˵��
//
//
// ����˵��
//
//
// ����ֵ
//     ����ʵ�ʶ�ȡ���ݵĳ���
//
// ����
//    ��
//
//------------------------------------------------------------------------------------------------------------

static __u32 __Img_ReadItemData(HIMAGE hImage, HIMAGEITEM hItem, void * buffer, __u64 Length);

// ���ݷ�����м��ٴ���İ汾 scott 2009-06-22 10:37:17
__u32 Img_ReadItemData(HIMAGE hImage, HIMAGEITEM hItem, void * buffer, __u64 Length)
{
	__u32 readlen = 0;
	IMAGE_HANDLE* pImage = (IMAGE_HANDLE *)hImage;
	ITEM_HANDLE * pItem  = (ITEM_HANDLE  *)hItem;
    __u64      filelen;
    __u64      datalen;
    __u64      offset;
    __u64      pos=0;
	__u32 this_read;
	__u32 i;

    pEnDecode pfDecode = pImage->rc_if_decode[DATA_ID].EnDecode;
	u8* pInPutBuffer = (u8*)buffer;
	u8* pBufferRead = pInPutBuffer ;
    filelen  = pImage->ItemTable[pItem->index].filelenLo;
    filelen |= (__u64)pImage->ItemTable[pItem->index].filelenHi << 32;
	if (pItem->pos >=filelen) //filelen <= datalen
	{
		__wrn("DEC_IMG: file pointer position is larger than file length\n");
		return 0;
	}
	//------------------------------------------------
	//Լ�����ݲ��ᳬ���������ݵķ�Χ
	//------------------------------------------------
	datalen  = pImage->ItemTable[pItem->index].datalenLo;
    datalen |= (__u64)pImage->ItemTable[pItem->index].datalenHi << 32;
	Length = min((__u64)Length, datalen - pItem->pos);
	//------------------------------------------------
	//���ܺ��������16byte���з��飬��Ҫ�����߽�����
	//------------------------------------------------
	if ((pItem->pos % ENCODE_LEN) == 0)	//pos�����Ƿ���ı߽�����
	{
	    __u8 *pin, *pout;
	    __u32 n;

	    offset  = pImage->ItemTable[pItem->index].offsetLo;
        offset |= (__u64)pImage->ItemTable[pItem->index].offsetHi << 32;
       // __inf("item offset low=%d\n",pImage->ItemTable[pItem->index].offsetLo);
       // __inf("item offset high=%d\n",pImage->ItemTable[pItem->index].offsetHi);
        pos = offset + pItem->pos;
		File_Seek(pImage->fp, pos, SZ_SEEK_SET);
		readlen = 0;
		while(readlen < Length)
		{
			//------------------------------------------------
			//ÿ�ζ�ȡn������
			//------------------------------------------------
			this_read = min(buffer_encode_size, (Length - readlen));
			n = (this_read + ENCODE_LEN - 1) / ENCODE_LEN;	//
			//memset(buffer_encode, 0, n * ENCODE_LEN);
			//һ�ζ�n������,�ٶȸ��� note has bug
			if(pImage->bWithEncpy == 0)
			{
				pBufferRead = pInPutBuffer + readlen;
				if(File_Read(pBufferRead, n * ENCODE_LEN, 1, pImage->fp))	//OK ����ͨ���������ȡ�����ķ���
           		 {
                	//�������������
                	__wrn("DEC_IMG: read ItemData failed\n");
                	return 0;
            	 }
			
			}else
			{
			
			
				if(File_Read(buffer_encode, n * ENCODE_LEN, 1, pImage->fp))	//OK ����ͨ���������ȡ�����ķ���
           		 {
            	    //�������������
               		 __wrn("DEC_IMG: read ItemData failed\n");
               		 return 0;
            	 }
				File_Seek(pImage->fp, 0, SZ_SEEK_CUR);
				//------------------------------------------------
				//�������ݽ���
				//------------------------------------------------
				pin  = buffer_encode;
				pout = (__u8 *)buffer;
				pout = pout + readlen;	//ʵ��������ݵ�ƫ����

				for (i = 0; i < n; i++)	//���������н���
				{
					//------------------------------------------------
					//ÿ�ν���һ������
					//------------------------------------------------
					if (OK !=  pfDecode(pImage->rc_if_decode[DATA_ID].handle, pin , pout))
					{
						__wrn("DEC_IMG: decode ItemData failed\n");
						return 0;
					}
					pin += ENCODE_LEN;
					pout+= ENCODE_LEN;
				}
			}
			//------------------------------------------------
			//����ʵ����Ч���ݳ���
			//------------------------------------------------
			readlen += this_read;
		}
		pItem->pos += readlen;
		return readlen;
	}
	else
	{
		//------------------------------------------------
		//����ǿ��ֻ����������������������ǰ��һЩ�̼������ܻ����𲻼��ݵ����⣬
		//���������ֻ������ԭʼ�汾��������
		//------------------------------------------------
		__wrn("__Img_ReadItemData\n");
		return  __Img_ReadItemData(hImage, hItem,  buffer, Length);
	}

	//return 0;
}


//ԭʼ�İ汾���������У�����ÿ�ζ�img�ļ���16byte���ٶȲ��ߣ���Ҫ��������
__u32 __Img_ReadItemData(HIMAGE hImage, HIMAGEITEM hItem, void * buffer, __u64 Length)
{

	IMAGE_HANDLE* pImage = (IMAGE_HANDLE *)hImage;
	ITEM_HANDLE * pItem  = (ITEM_HANDLE  *)hItem;
    __u64      filelen;
    __u64      datalen;
    __u64      offset;
    __u64      pos=0;
    __u64      readlen=0;
	__u8 buffer_encode[ENCODE_LEN];


	pEnDecode pfDecode = pImage->rc_if_decode[DATA_ID].EnDecode;
	if (NULL == pImage || NULL == pItem || NULL == buffer || 0 == Length)
	{
		return 0;
	}

   	filelen  = pImage->ItemTable[pItem->index].filelenLo;
    filelen |= (__u64)pImage->ItemTable[pItem->index].filelenHi << 32;
	if (pItem->pos >=filelen) //filelen <= datalen
	{
		return 0;
	}
	//------------------------------------------------
	//Լ�����ݲ��ᳬ���������ݵķ�Χ
	//------------------------------------------------
	datalen = pImage->ItemTable[pItem->index].datalenLo;
    datalen |= (__u64)pImage->ItemTable[pItem->index].datalenHi << 32;
	Length = min((__u64)Length, datalen - pItem->pos);
	if(pImage->bWithEncpy == 0)
	{
   		offset = pImage->ItemTable[pItem->index].offsetLo;
        offset |= (__u64)pImage->ItemTable[pItem->index].offsetHi << 32;
        pos = offset + pItem->pos;
		File_Seek(pImage->fp, pos, SZ_SEEK_SET);
        
		if(File_Read(buffer, Length, 1, pImage->fp))
		{
			return 0;
		}
		pItem->pos += Length;
		return Length;
	}

	//------------------------------------------------
	//���ܺ��������16byte���з��飬��Ҫ�����߽�����
	//------------------------------------------------
	if ((pItem->pos % ENCODE_LEN) == 0)	//pos�����Ƿ���ı߽�����
	{
	    __u8 *pin, *pout;

   		 offset = pImage->ItemTable[pItem->index].offsetLo;
        offset |= (__u64)pImage->ItemTable[pItem->index].offsetHi << 32;
        pos = offset + pItem->pos;

		File_Seek(pImage->fp, pos, SZ_SEEK_SET);

		while(readlen < Length)
		{
			//ÿ�ζ�ȡһ������
			//memset(buffer_encode, 0, ENCODE_LEN);
			if(File_Read(buffer_encode, ENCODE_LEN, 1, pImage->fp))
			{
			    return 0;
			}
			File_Seek(pImage->fp, 0, SZ_SEEK_CUR);
			//�������ݽ���
			pin = buffer_encode;
			pout= (__u8 *)buffer;
			pout= pout + readlen;
			if (OK != pfDecode(pImage->rc_if_decode[DATA_ID].handle, pin , pout))
				return 0;
			//����ʵ����Ч���ݳ���
			readlen += min(Length- readlen, ENCODE_LEN);
		}
		pItem->pos += readlen;
		return readlen;
	}
	else //pos���ڱ߽�
	{
		//pos���ڱ߽磬��ͷ����seek

        offset = pImage->ItemTable[pItem->index].offsetLo;
        offset |= (__u64)pImage->ItemTable[pItem->index].offsetHi << 32;		
		pos = offset +pItem->pos - (pItem->pos % ENCODE_LEN);
		File_Seek(pImage->fp, pos, SZ_SEEK_SET);

		//-----------------------------------
		//**********************OOOOOOOOOOOOO     *��ʾ�Ѿ���ȡ������ O��ʾδ��ȡ������
		//-----------------------------------
		if ((0 < Length) && (Length < ENCODE_LEN)) //��ȡ�����ݲ���һ�����鳤��
		{
		    __u8 *pin, *pout;
			__u32 read = ENCODE_LEN - (pItem->pos % ENCODE_LEN); //������δ��ȡ�����ݳ���
			if (Length <= read)	//��Ҫ��ȡ������С�ڵ��ڷ�����δ��ȡ�����ݳ��� ֻ�ö�һ�����鼴��
			{
				//memset(buffer_encode, 0, ENCODE_LEN);
				if(File_Read(buffer_encode, ENCODE_LEN, 1, pImage->fp))
				{
				    return 0;
				}
				//�������ݽ���
				pin = buffer_encode;
				pout= (__u8 *)buffer;
				pout     = pout + readlen;
				if (OK != pfDecode(pImage->rc_if_decode[DATA_ID].handle, pin , pout))
					return 0;

				readlen = Length;
				pItem->pos += readlen;
				return readlen;
			}
			else //��Ҫ���������������
			{
				//��һ������
				__u8 *pin, *pout;
				__u32 Left_Length;
				__u32 read = ENCODE_LEN - pItem->pos % ENCODE_LEN;
				//memset(buffer_encode, 0, ENCODE_LEN);
				if(File_Read(buffer_encode, ENCODE_LEN, 1, pImage->fp))
				{
				    return 0;
				}
				//�������ݽ���
				pin  = buffer_encode;
				pout = (__u8 *)buffer;
				pout = pout + readlen;
				if (OK != pfDecode(pImage->rc_if_decode[DATA_ID].handle, pin , pout))
					return 0;

				readlen += read;

				//�ڶ�������
				Left_Length = Length - read;			//ʣ�������
				//memset(buffer_encode, 0, ENCODE_LEN);
				if(File_Read(buffer_encode, ENCODE_LEN, 1, pImage->fp))
				{
				    return 0;
				}
				File_Seek(pImage->fp, 0, SZ_SEEK_CUR);
				//�������ݽ���
				pin = buffer_encode;
				pout= (__u8 *)buffer;
				pout     = pout + readlen;
				if (OK != pfDecode(pImage->rc_if_decode[DATA_ID].handle, pin , pout))
					return 0;
				readlen += Left_Length;

				pItem->pos += readlen;
				return readlen;
			}
		}
		else if (Length >= ENCODE_LEN) //��ȡ�����ݲ�����һ�����鳤��
		{
			__u8 *pin, *pout;
			__u32 Left_Length, Left_readlen;
			__u32 read = ENCODE_LEN - pItem->pos % ENCODE_LEN;
			//memset(buffer_encode, 0, ENCODE_LEN);
			if(File_Read(buffer_encode, ENCODE_LEN, 1, pImage->fp))
			{
			    return 0;
			}
			//�������ݽ���
			pin = buffer_encode;
			pout= (__u8 *)buffer;
			pout     = pout + readlen;
			if (OK != pfDecode(pImage->rc_if_decode[DATA_ID].handle, pin , pout))
				return 0;

			readlen += read;

			//------------------------------------------------
			//ʣ������ݰ��շ�����д���
			//------------------------------------------------
			Left_Length = Length - read;
			Left_readlen= 0;
			while(Left_readlen < Left_Length)
			{
				//ÿ�ζ�ȡһ������
				//memset(buffer_encode, 0, ENCODE_LEN);
				if(File_Read(buffer_encode, ENCODE_LEN, 1, pImage->fp))
				{
				    return 0;
				}
				File_Seek(pImage->fp, 0, SZ_SEEK_CUR);
				//�������ݽ���
				pin = buffer_encode;
				pout= (__u8 *)buffer;
				pout     = pout + readlen;
				if (OK != pfDecode(pImage->rc_if_decode[DATA_ID].handle, pin , pout))
					return 0;
				//����ʵ����Ч���ݳ���
				Left_readlen += min(Left_Length - Left_readlen, ENCODE_LEN);
			}

			readlen += Left_readlen;
		}

		pItem->pos += readlen;
		return readlen;
	}

//	return 0;
}



//------------------------------------------------------------------------------------------------------------
//
// ����˵��
//
//
// ����˵��
//
//
// ����ֵ
//
//
// ����
//    ��
//
//------------------------------------------------------------------------------------------------------------
__u32 Img_CloseItem	(HIMAGE hImage, HIMAGEITEM hItem)
{
	ITEM_HANDLE * pItem = (ITEM_HANDLE *)hItem;
	if (NULL == pItem)
		return __LINE__;

	pItem = NULL;

	return OK;
}



//------------------------------------------------------------------------------------------------------------
//
// ����˵��
//
//
// ����˵��
//
//
// ����ֵ
//
//
// ����
//    ��
//
//------------------------------------------------------------------------------------------------------------
void  Img_Close	(HIMAGE hImage)
{
	IMAGE_HANDLE * pImage = (IMAGE_HANDLE *)hImage;

	if (NULL != pImage->fp)
	{
		File_Close(pImage->fp);
		pImage->fp = NULL;
	}

	if (NULL != pImage->ItemTable)
	{
		pImage->ItemTable = NULL;
	}

	memset(pImage, 0, sizeof(IMAGE_HANDLE));
	pImage = NULL;

	if(buffer_encode)
	{
	    wBoot_free(buffer_encode);
    }
	return ;
}


//------------------------------------------------------------------------------------------------------------
// THE END !
//------------------------------------------------------------------------------------------------------------



