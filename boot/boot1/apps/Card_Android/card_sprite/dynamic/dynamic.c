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
#include  "card_sprite_i.h"


typedef struct _tag_Dynamic_head
{
	u8   magic[8];   	// ��d�� 'y' 'n' 'a' 'm' 'i' 'c'
	u16  version;     	// �汾
	u16  count;        	// ���ݵĸ���
	u32  tableOffset; 	// ��������ƫ��
	u32  dataOffset;  	// ���ݵ�ƫ��
	u32  sumCheck;   	// У���
	u32  dataAlign;   	// ���ݶ������
	u32  used;			// �Ƿ�ʹ�ù�
	u8   reserve[32];	// Ԥ��
}Dyamic_head;


typedef struct _tag_Dynamic_Item	// �������
{
	char name[16]; 		// ���ƣ����Ȳ�����16���ַ��ĳ���
	char alians[16]; 	// �������Ȳ�����16���ַ��ĳ���
	u32  nDataLen;		// ���ݳ���
	u32  Checksum;		// У���
	u8   reserve[24]; 	// Ԥ��

}Dynamic_Item;


typedef struct _tag_Dynamic_index	//������������¼�ù����ļ������ͱ��
{
	char magic[8];   	 // ��d�� 'y' 'n' 'a' 'm' 'i' 'c'
	char last_file[16];	 //��ǰӦ��ʹ�õ��ļ���
	int  total_count;	 //�ܹ����ļ�����
	int  used_count;	 //ʹ�ù��Ĵ���
	int  err_times;		 //����Ĵ���
	int  successed_times;//�ɹ��Ĵ���
	int  reserved;
}Dynamic_index;


static __s32 dynamic_data_get(void *buffer, int size, void *dynamic_buffer, int dynamic_size);
//static __s32 simple_check_data_valid( void *mem_base, __u32 size );
//static __s32 simple_data_checksum(void *mem_base, __u32 size);

__s32 fetch_dynamic_data(char *buffer, int buffer_size)
{
	int		ret, i;
	H_FILE  index_file = NULL;
	H_FILE  data_file  = NULL;
	int     index_length;
	int     data_length;
	char    index_buffer[1024];
	char    *data_buffer;
	Dynamic_index  *dindex;
	char    data_file_name[32];
	Dyamic_head    *ddata;
	int		index_number;
	char    tmp_data_file_name[16];

	ret = -1;
//	if(wBoot_fsmount('Z'))		//��ʵ�ǹұ�׼MBR���û���
//	{
//		__inf("log error: cant find user disk\n");
//
//		return;
//	}
	//�������ļ�
	index_file = wBoot_fopen("z:\\dynamic\\dindex.bin", "rb+");
	if(!index_file)
	{
		sprite_wrn("dynamic data error: cant find dynamic data index file\n");

		goto dynamic_data_err;
	}
	index_length = wBoot_flen(index_file);
	if(!index_length)
	{
		sprite_wrn("dynamic data error: dynamic data index file is bad\n");

		goto dynamic_data_err;
	}
	//������������
	memset(index_buffer, 0, 1024);
	wBoot_fread(index_buffer, 1, index_length, index_file);
	dindex = (Dynamic_index *)index_buffer;
	//У����������
	if(memcmp(dindex->magic, "dynamic", sizeof("dynamic")))
	{
		sprite_wrn("dynamic data error: bad dynamic data index\n");

		goto dynamic_data_err;
	}
	//���������ļ�����
	memset(data_file_name, 0, 32);
	strcpy(data_file_name, "z:\\dynamic\\");

	while(dindex->used_count < dindex->total_count)
	{
		dindex->used_count ++;
		dindex->err_times ++;
		memset(tmp_data_file_name, 0, 16);
		strcpy(tmp_data_file_name, dindex->last_file);
		index_number = 0;
		for(i=1;(i<8) && (tmp_data_file_name[i] != '.');i++)
		{
			index_number = index_number * 10 + (tmp_data_file_name[i] - '0');
		}
		//���浱ǰҪ���ҵ��ļ���
		strcpy(data_file_name + sizeof("z:\\dynamic\\") - 1, dindex->last_file);
		//�����µ������ļ�����
		index_number ++;
		for(i=7;i>0;i--)
		{
			tmp_data_file_name[i] = (index_number%10) + '0';
			index_number /= 10;
		}
		//��д�������ļ���(����)
		strcpy(dindex->last_file, tmp_data_file_name);
		//�������ļ�
		data_file = wBoot_fopen(data_file_name, "rb+");
		if(!data_file)
		{
			//��Ӧ�������ļ�������
			sprite_wrn("dynamic data error: dynamic data file %s is not exist\n", data_file_name);
			data_file = NULL;

			continue;
		}
		data_length = wBoot_flen(data_file);
		if(!data_length)
		{
			sprite_wrn("dynamic data error: dynamic data file %s is bad\n", data_file_name);
			wBoot_fclose(data_file);
			data_file = NULL;

			continue;
		}
		data_buffer = wBoot_malloc(data_length);
		if(!data_buffer)
		{
			sprite_wrn("dynamic data error: not enough memory for dynamic data\n");

			goto dynamic_data_err;
		}
		wBoot_fread(data_buffer, 1, data_length, data_file);
		//������ݵ�У���
//		if(simple_check_data_valid(data_buffer, data_length))
//		{
//			sprite_wrn("dynamic data error: dynamic data checksum error\n");
//			wBoot_free(data_buffer);
//			data_buffer = NULL;
//			wBoot_fclose(data_file);
//			data_file = NULL;
//
//			continue;
//		}
		ddata = (Dyamic_head *)data_buffer;
		if(ddata->used)	//�Ѿ���ʹ�ù����л���һ���ļ�
		{
			wBoot_free(data_buffer);
			data_buffer = NULL;
			wBoot_fclose(data_file);
			data_file = NULL;
			wBoot_fclose(data_file);
		}
		else
		{
			ddata->used = 1;
			//ȡ����������
			dynamic_data_get(data_buffer, data_length, buffer, buffer_size);
			dindex->successed_times ++;
			dindex->err_times --;
			//�����ԭ�ļ�
//			simple_data_checksum(data_buffer, data_length);
			wBoot_fseek(data_file, 0, 0);
			wBoot_write(data_buffer, 1, data_length, data_file);
			wBoot_fclose(data_file);

			break;
		}
	}
	//�޸������ļ�
	strcpy(dindex->last_file, tmp_data_file_name);
	wBoot_fseek(index_file, 0, 0);
	wBoot_write(index_buffer, 1, index_length, index_file);
	ret = 0;
	//���
dynamic_data_err:
	if(data_buffer)
	{
		wBoot_free(data_buffer);
	}
	if(data_file)
	{
		wBoot_fclose(data_file);
	}
	if(index_file)
	{
		wBoot_fclose(index_file);
	}

	return ret;
}


//static __s32 simple_check_data_valid( void *mem_base, __u32 size )
//{
//	__u32 *buf;
//	__u32 count;
//	__u32 src_sum;
//	__u32 sum;
//	Dyamic_head    *ddata;
//
//	ddata = (Dyamic_head *)mem_base;
//
//	/* ����У��� */
//	src_sum = ddata->sumCheck;                  // ��Boot_file_head�еġ�check_sum���ֶ�ȡ��У���
//	ddata->sumCheck = 0;              		// ��STAMP_VALUEд��Boot_file_head�еġ�check_sum���ֶ�
//
//	count = size >> 2;                      // �� �֣�4bytes��Ϊ��λ����
//	sum = 0;
//	buf = (__u32 *)mem_base;
//	do
//	{
//		sum += *buf++;                         // �����ۼӣ����У���
//		sum += *buf++;                         // �����ۼӣ����У���
//		sum += *buf++;                         // �����ۼӣ����У���
//		sum += *buf++;                         // �����ۼӣ����У���
//	}while( ( count -= 4 ) > (4-1) );
//
//	while( count-- > 0 )
//		sum += *buf++;
//
//	ddata->sumCheck = src_sum;                  // �ָ�Boot_file_head�еġ�check_sum���ֶε�ֵ
//
//	if( sum == src_sum )
//		return 0;               // У��ɹ�
//	else
//		return -1;                 // У��ʧ��
//}

//static __s32 simple_data_checksum(void *mem_base, __u32 size)
//{
//	Dyamic_head    *ddata;
//	__u32           *buf;
//	__u32            loop;
//	__u32            i;
//	__u32            sum;
//
//	ddata = (Dyamic_head *)mem_base;
//	buf = (__u32 *)mem_base;
//	ddata->sumCheck = 0;              // fill stamp
//	loop = size >> 2;
//    /* ���㵱ǰ�ļ����ݵġ�У��͡�*/
//    for( i = 0, sum = 0;  i < loop;  i++ )
//    	sum += buf[i];
//
//    /* write back check sum */
//    ddata->sumCheck = sum;
//
//    return 0;
//}

static __s32 dynamic_data_get(void *buffer, int size, void *dynamic_buffer, int dynamic_size)
{
	Dyamic_head 		*ddata;
	Dynamic_Item		*ditem;
	dynamic_data_form	*dform;
	dynamic_boot_head	*dhead;
	char			    *base;
	char				*d_base;
	int					 used_size;
	int					 item_size;
	int					 i;

	if(!dynamic_buffer)
	{
		return -1;
	}
	memset(dynamic_buffer, 0, dynamic_size);
	base  = (char *)buffer;
	ddata = (Dyamic_head *)buffer;
	used_size = 0;

	dhead = (dynamic_boot_head *)dynamic_buffer;
	d_base= (char *)dynamic_buffer + sizeof(dynamic_boot_head);

	memcpy(dhead->magic, "dynamic", sizeof("dynamic"));

	for(i=0;i<ddata->count;i++)
	{
		//��̬���ݶ��׵�ַ
		ditem = (Dynamic_Item *)(base + ddata->tableOffset + i * sizeof(Dynamic_Item));
		//��ȡ��ǰ��̬���ݼ���ռ�õĳ���
		item_size = sizeof(dynamic_data_form) + ditem->nDataLen;
		//�ù����ܳ���
		if(item_size & 0x03)
		{
			item_size = ((item_size + 4)>>2)<<2;
		}
		used_size += item_size;
		if(used_size > dynamic_size)
		{
			sprite_wrn("dynamic data error: not enough memory to store dynamic data\n");

			return -1;
		}
		dform = (dynamic_data_form *)d_base;
		memcpy(dform->name, ditem->name, 16);
		dform->datasize = ditem->nDataLen;
		dform->usedsize = ditem->nDataLen;
		if(dform->usedsize & 0x03)
		{
			dform->usedsize = ((dform->usedsize + 4)>>2)<<2;
		}
		memcpy(d_base + sizeof(dynamic_data_form), base + ddata->dataOffset + i * 512, ditem->nDataLen);
		d_base += sizeof(dynamic_data_form);
		d_base += dform->usedsize;

		dhead->count ++;
	}

	return 0;
}

