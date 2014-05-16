/*
* (C) Copyright 2007-2012
* Allwinner Technology Co., Ltd. <www.allwinnertech.com>
* Neil Peng<penggang@allwinnertech.com>
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
#include "nand_flash.h"


//#pragma arm section  code="load_and_check_in_one_blk"
/*******************************************************************************
*��������: load_and_check_in_one_blk
*����ԭ�ͣ�int32 load_and_check_in_one_blk( __u32 blk_num, void *buf,
*                       __u32 size, __u32 blk_size, const char *magic )
*��������: ��nand flash��ĳһ�����ҵ�һ����ñ��ݽ������뵽RAM�С�����ɹ�����
*          ��OK�����򣬷���ERROR��
*��ڲ���: blk_num           �����ʵ�nand flash��Ŀ��
*          buf               ����������ʼ��ַ
*          size              ���������ݵĴ�С
*          blk_size          �����ʵ�nand flash��ĳߴ�
*          magic             �ļ���magic
*�� �� ֵ: ADV_NF_OK                ����ɹ�
*          ADV_NF_OVERTIME_ERR      ������ʱ
*          ADV_NF_ERROR             ����ʧ��
*��    ע: 1. �����������鵱ǰ��ĺû�����ĺû������ڵ��ñ�����ǰ����
*          2. ��������Ӧ�ôӵ�ǰ�����ʼ��ַ�����Ρ������ŷš�
*******************************************************************************/
__s32 load_and_check_in_one_blk( __u32 blk_num, void *buf, __u32 size, __u32 blk_size, const char *magic )
{
	__u32 copy_base;
	__u32 copy_end;
	__u32 blk_end;
	__u32 blk_base = blk_num * blk_size;
	__s32  status;


	for( copy_base = blk_base, copy_end = copy_base + size, blk_end = blk_base + blk_size;
         copy_end <= blk_end;
         copy_base += size, copy_end = copy_base + size )
    {
        status = NF_read( copy_base >> NF_SCT_SZ_WIDTH, (void *)buf, size >> NF_SCT_SZ_WIDTH ); // ����һ������
        if( status == NF_OVERTIME_ERR )
            return ADV_NF_OVERTIME_ERR;
        else if( status == NF_ECC_ERR )
        	continue;

        /* У�鱸���Ƿ���ã������ã�����򷵻�OK */
        if( check_file( (__u32 *)buf, size, magic ) == 0 )
        {
            __msg("The file stored in %X of block %u is perfect.\n", ( copy_base - blk_base ), blk_num );
			return ADV_NF_OK;
		}
	}

	return ADV_NF_ERROR;                              // ��ǰ���в�������õı���
}



//#pragma arm section  code="load_in_many_blks"
/*******************************************************************************
*��������: load_in_many_blks
*����ԭ�ͣ�int32 load_in_many_blks( __u32 start_blk, __u32 last_blk_num, void *buf,
*						            __u32 size, __u32 blk_size, __u32 *blks )
*��������: ��nand flash��ĳһ��start_blk��ʼ������file_length���ȵ����ݵ��ڴ��С�
*��ڲ���: start_blk         �����ʵ�nand flash��ʼ���
*          last_blk_num      ���һ����Ŀ�ţ��������Ʒ��ʷ�Χ
*          buf               �ڴ滺��������ʼ��ַ
*          size              �ļ��ߴ�
*          blk_size          �����ʵ�nand flash�Ŀ��С
*          blks              ��ռ�ݵĿ�������������
*�� �� ֵ: ADV_NF_OK                �����ɹ�
*          ADV_NF_OVERTIME_ERR   ������ʱ
*          ADV_NF_LACK_BLKS      ��������
*��    ע: 1. ������ֻ���룬��У��
*******************************************************************************/
__s32 load_in_many_blks( __u32 start_blk, __u32 last_blk_num, void *buf,
						 __u32 size, __u32 blk_size, __u32 *blks )
{
	__u32 buf_base;
	__u32 buf_off;
    __u32 size_loaded;
    __u32 cur_blk_base;
    __u32 rest_size;
    __u32 blk_num;


	for( blk_num = start_blk, buf_base = (__u32)buf, buf_off = 0;
         blk_num <= last_blk_num && buf_off < size;
         blk_num++ )
    {
    	__msg("current block is %d and last block is %d.\n", blk_num, last_blk_num);
    	if( NF_read_status( blk_num ) == NF_BAD_BLOCK )		// �����ǰ���ǻ��飬�������һ��
    		continue;

    	cur_blk_base = blk_num * blk_size;
    	rest_size = size - buf_off ;                        // δ���벿�ֵĳߴ�
    	size_loaded = ( rest_size < blk_size ) ?  rest_size : blk_size ;  // ȷ���˴δ�����ĳߴ�

    	if( NF_read( cur_blk_base >> NF_SCT_SZ_WIDTH, (void *)buf_base, size_loaded >> NF_SCT_SZ_WIDTH )
    		== NF_OVERTIME_ERR )
       		return ADV_NF_OVERTIME_ERR;

    	buf_base += size_loaded;
    	buf_off  += size_loaded;
    }


    *blks = blk_num - start_blk;                            // �ܹ��漰�Ŀ���
    if( buf_off == size )
		return ADV_NF_OK;                                          // �ɹ�������OK
	else
	{
		__msg("lack blocks with start block %d and buf size %x.\n", start_blk, size);
		return ADV_NF_LACK_BLKS;                                // ʧ�ܣ���������
	}
}


