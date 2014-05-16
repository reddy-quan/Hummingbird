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
#include  "update_data.h"
#include  "update_data/card_sprite_flash/card_sprite_flash.h"
#include  "string.h"
#include  "sparse.h"
#include  "sparse_format.h"

#define  SPARSE_HEADER_MAJOR_VER 1

__u32  android_format_checksum;
__u32  sparse_format_type;
__u32  chunk_count;
int  last_rest_size;
int  chunk_length;
__u32  flash_start;
sparse_header_t globl_header;
/*
************************************************************************************************************
*
*                                             unsparse_probe
*
*    �������ƣ�
*
*    �����б�
*
*    ����ֵ  ��
*
*    ˵��    ��
*
*
************************************************************************************************************
*/
static __u32 add_sum(void *pbuf, __u32 len)
{
	//ȷ��len��4�ֽڶ���ģ���Ȼ����Ҫ���¼���
	__u32 sum = 0;
	__u32 *addr = (__u32 *)pbuf;
	__u32 data_len = len;
	__u32 rest;

    rest = len & 0x03;
	data_len = (len & (~0x03))>>2;

	while(data_len --)
	{
		sum += *addr ++;
	}

    if(rest == 0)
    {
    	;
    }
	else if(rest == 1)
	{
		sum += (*addr) & 0xff;
	}
	else if(rest == 2)
	{
		sum += (*addr) & 0xffff;
	}
	else if(rest == 3)
	{
		sum += (*addr) & 0xffffff;
	}

	return sum;
}
/*
************************************************************************************************************
*
*                                             unsparse_probe
*
*    �������ƣ�
*
*    �����б�
*
*    ����ֵ  ��
*
*    ˵��    ��
*
*
************************************************************************************************************
*/
int unsparse_probe(char *source, __u32 length, __u32 android_format_flash_start)
{
	sparse_header_t *header = (sparse_header_t*) source;

	if (header->magic != SPARSE_HEADER_MAGIC)
	{
		__inf("sparse: bad magic\n");

		return -1;
	}

	if ((header->major_version != SPARSE_HEADER_MAJOR_VER) ||
	    (header->file_hdr_sz != sizeof(sparse_header_t)) ||
	    (header->chunk_hdr_sz != sizeof(chunk_header_t)))
	{
		__inf("sparse: incompatible format\n");

		return -1;
	}
	android_format_checksum  = 0;
	last_rest_size = 0;
	chunk_count = 0;
	chunk_length = 0;
	sparse_format_type = SPARSE_FORMAT_TYPE_TOTAL_HEAD;
	flash_start = android_format_flash_start;

	return 0;
}
/*
************************************************************************************************************
*
*                                             DRAM_Write
*
*    �������ƣ�
*
*    �����б�
*
*    ����ֵ  ��
*
*    ˵��    ��
*
*
************************************************************************************************************
*/
int unsparse_deal(void)
{
	return 0;
}
/*
************************************************************************************************************
*
*                                             DRAM_Write
*
*    �������ƣ�
*
*    �����б�
*
*    ����ֵ  ��
*
*    ˵��    ��
*
*
************************************************************************************************************
*/
int  unsparse_direct_write(void *pbuf, int length)
{
	int   unenough_length;
	int   this_rest_size;
	int   tmp_down_size;
	char *tmp_buf, *tmp_dest_buf;
	chunk_header_t   *chunk;

    //���ȼ��㴫�������ݵ�У���
	android_format_checksum += verify_sum(pbuf, length);

    this_rest_size = last_rest_size + length;
    tmp_buf = (char *)pbuf - last_rest_size;
	last_rest_size = 0;

    while(this_rest_size > 0)
    {
		switch(sparse_format_type)
		{
			case SPARSE_FORMAT_TYPE_TOTAL_HEAD:
			{
				memcpy(&globl_header, tmp_buf, sizeof(sparse_header_t));
            	this_rest_size -= sizeof(sparse_header_t);
            	tmp_buf += sizeof(sparse_header_t);

                sparse_format_type = SPARSE_FORMAT_TYPE_CHUNK_HEAD;

				break;
			}
			case SPARSE_FORMAT_TYPE_CHUNK_HEAD:
			{
				if(this_rest_size < sizeof(chunk_header_t))
				{
					__inf("sparse: chunk head data is not enough\n");
					last_rest_size = this_rest_size;
					tmp_dest_buf = (char *)pbuf - this_rest_size;
		    		memcpy(tmp_dest_buf, tmp_buf, this_rest_size);
					this_rest_size = 0;

		    		break;
				}
				chunk = (chunk_header_t *)tmp_buf;
				/* move to next chunk */
				tmp_buf += sizeof(chunk_header_t);        //��ʱtmp_buf�Ѿ�ָ����һ��chunk����data��ʼ��ַ
				this_rest_size -= sizeof(chunk_header_t); //ʣ������ݳ���
				chunk_length = chunk->chunk_sz * globl_header.blk_sz;   //��ǰ���ݿ���Ҫд������ݳ���
				//__inf("chunk index = %d\n", chunk_count ++);

				switch (chunk->chunk_type)
				{
					case CHUNK_TYPE_RAW:

						if (chunk->total_sz != (chunk_length + sizeof(chunk_header_t)))
						{
							__inf("sparse: bad chunk size for chunk %d, type Raw\n", chunk_count);

							return -1;
						}
						//���ﲻ�������ݲ��֣�ת����һ��״̬
						sparse_format_type = SPARSE_FORMAT_TYPE_CHUNK_DATA;

						break;

					case CHUNK_TYPE_DONT_CARE:
						if (chunk->total_sz != sizeof(chunk_header_t))
						{
							__inf("sparse: bogus DONT CARE chunk\n");

							return -1;
						}
						flash_start += (chunk_length>>9);
						sparse_format_type = SPARSE_FORMAT_TYPE_CHUNK_HEAD;

						break;

					default:
						__inf("sparse: unknown chunk ID %x\n", chunk->chunk_type);

						return -1;
				}
				break;
			}
			case SPARSE_FORMAT_TYPE_CHUNK_DATA:
			{
				//�����ж������Ƿ��㹻��ǰchunk����,������㣬����������Ҫ�����ݳ���
				unenough_length = (chunk_length >= this_rest_size)? (chunk_length - this_rest_size):0;
				if(!unenough_length)
				{
					//�����㹻��ֱ��д��
					if(sprite_flash_write(flash_start, chunk_length>>9, tmp_buf))
					{
						__inf("sparse: flash write failed\n");

						return -1;
					}
					if(chunk_length & 511)
					{
						__inf("data is not sector align 0\n");

						return -1;
					}
					flash_start += (chunk_length>>9);
					tmp_buf += chunk_length;
					this_rest_size -= chunk_length;
					chunk_length = 0;

					sparse_format_type = SPARSE_FORMAT_TYPE_CHUNK_HEAD;
				}
				else  //����ȱʧ���ݵ����
				{
					if(this_rest_size < 8 * 1024) //�ȿ����������Ƿ���8k
					{
						//������ʱ����������ݷŵ���һ�����ݵ�ǰ�����ȴ���һ�δ���
						tmp_dest_buf = (char *)pbuf - this_rest_size;
						memcpy(tmp_dest_buf, tmp_buf, this_rest_size);
                        last_rest_size = this_rest_size;
						this_rest_size = 0;

						break;
					}
					//���������ݳ���16kʱ
					//��ȱʧ���ݳ��Ȳ���4kʱ,����ֻȱ��ʮ���ֽ�
					if(unenough_length < 4 * 1024)
					{
						//����ƴ�ӷ���������д�����������ݣ�Ȼ������һ�ΰ�δ��д���������ݺ�ȱʧ����һ����¼
						tmp_down_size = this_rest_size + unenough_length - 4 * 1024;
					}
					else //���ﴦ��ȱʧ���ݳ���8k(����)�����,ͬʱ��������Ҳ����16k
					{
						//ֱ����¼��ǰȫ������;
						tmp_down_size = this_rest_size & (~(512 -1));  //��������
					}
					if(sprite_flash_write(flash_start, tmp_down_size>>9, tmp_buf))
					{
						__inf("sparse: flash write failed\n");

						return -1;
					}
					if(tmp_down_size & 511)
					{
						__inf("data is not sector align 1\n");

						return -1;
					}
					tmp_buf += tmp_down_size;
					flash_start += (tmp_down_size>>9);
					chunk_length -= tmp_down_size;
					this_rest_size -= tmp_down_size;
					tmp_dest_buf = (char *)pbuf - this_rest_size;
					memcpy(tmp_dest_buf, tmp_buf, this_rest_size);
					last_rest_size = this_rest_size;
					this_rest_size = 0;

					sparse_format_type = SPARSE_FORMAT_TYPE_CHUNK_DATA;
				}

				break;
			}

			default:
			{
				__inf("sparse: unknown status\n");

				return -1;
			}
		}
	}

	return 0;
}
/*
************************************************************************************************************
*
*                                             unsparse_checksum
*
*    �������ƣ�
*
*    �����б�
*
*    ����ֵ  ��
*
*    ˵��    ��
*
*
************************************************************************************************************
*/
__u32 unsparse_checksum(void)
{
	//return verify_sum(android_format_dram_base, android_format_size);
	return android_format_checksum;
}


