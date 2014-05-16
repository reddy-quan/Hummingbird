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

#define TOLERANCE     3         // tolerance for the loop "erase--write--write error--good block by checking"

//#pragma arm section  code="write_in_one_blk"
/*******************************************************************************
*��������: write_in_one_blk
*����ԭ�ͣ�int32 write_in_one_blk( __u32 blk_num, void *buf,
*                                  __u32 size, __u32 blk_size )
*��������: ���ڴ��д�buf��ʼ��size��С�����������blk_numָ���Ŀ顣
*��ڲ���: blk_num           �����ʵ�nand flash��Ŀ��
*          buf               ����������ʼ��ַ
*          size              ��д�����ݵĴ�С
*          blk_size          �����ʵ�nand flash��ĳߴ�
*�� �� ֵ: ADV_NF_OK             д��ɹ�
*          ADV_NF_NEW_BAD_BLOCK  ��ǰ�����˻���
*          ADV_NF_OVERTIME_ERR   ������ʱ
*��    ע: 1. �����򲻼��鵱ǰ��ĺû�����ĺû������ڵ��ñ�����ǰ����
*          2. ��������Ӧ�ôӵ�ǰ�����ʼ��ַ�����Ρ������ŷš�
*******************************************************************************/
__s32 write_in_one_blk( __u32 blk_num, void *buf, __u32 size, __u32 blk_size )
{
    __u32 j;
    __s32  status;
    __u32 copy_base;
    __u32 copy_end;
    __u32 blk_end;
    __u32 scts_per_copy;

    for( j = 0, scts_per_copy = size >> NF_SCT_SZ_WIDTH; j < TOLERANCE; j++ )
    {
	    status = NF_erase( blk_num );
	    if( status == NF_OVERTIME_ERR )
	    	return ADV_NF_OVERTIME_ERR;
        else if( status == NF_ERASE_ERR )
        {
        	__wrn("fail in erasing block %u.\n", blk_num);
        	NF_mark_bad_block( blk_num );         // �ڱ�ǻ���ǰ���Ȳ���
        	return ADV_NF_NEW_BAD_BLOCK;
        }
		__msg("Succeed in erasing block %u.\n", blk_num);

        for( copy_base = blk_num * blk_size, copy_end = copy_base + size, blk_end = copy_base + blk_size;
        	 copy_end <= blk_end;
        	 copy_base += size, copy_end = copy_base + size )
        {
    	    status = NF_write( copy_base >> NF_SCT_SZ_WIDTH, buf, scts_per_copy );
    		__msg("finish in progmming address %x on block %u.\n", copy_base, blk_num );
    	    if( status == NF_OVERTIME_ERR )
    	    	return ADV_NF_OVERTIME_ERR;
    	    else if( status == NF_PROG_ERR )
    	    {
	            if( NF_verify_block( blk_num ) == NF_GOOD_BLOCK )
	            	goto try_again;               // ���衰��--д--д����--У���Ǻÿ顱����������޴�����TOLERANCE���Ļ���
	            else
					goto failure;		          // ������ȷʵ�ǻ��飬�Ȳ�������
	        }
	    }
	    break;	                                  // �ɹ�������˵�ǰ�飬������ǰѭ��

try_again:
		continue;
	}


	if( j < TOLERANCE )                           // �ɹ�
	{
//	   	__msg("succeed in programming block %u.\n", blk_num);
		return ADV_NF_OK;
	}


failure:
		__msg("fail in programming block %u, it is bad block.\n", blk_num);
	/* �Ȳ��������� */
	NF_erase( blk_num );
	NF_mark_bad_block( blk_num );
	return ADV_NF_NEW_BAD_BLOCK;
}
//#pragma arm section  code="write_in_many_blks"
/*******************************************************************************
*��������: write_in_many_blks
*����ԭ�ͣ�int32 write_in_many_blks( __u32 start_blk, __u32 last_blk_num, void *buf,
*				             	     __u32 size, __u32 blk_size, __u32 * blks )
*��������: ���ڴ��е�����˳��д�뵽�������
*��ڲ���: start_blk         �����ʵ�nand flash��ʼ���
*          last_blk_num      ���һ����Ŀ�ţ��������Ʒ��ʷ�Χ
*          buf               ����������ʼ��ַ
*          size              ��д�����ݵĴ�С
*          blk_size          �����ʵ�nand flash�Ŀ��С
*          blks              ��ռ�ݵĿ�������������
*�� �� ֵ: NO_NEW_BAD_BLOCK  д��ɹ���û�������»���
*          NF_LACK_BLKS      ��������
*          NF_NEW_BAD_BLOCK  д��ɹ������������»���
*          OVERTIME_ERR   ������ʱ
*��    ע: ���ݴ���ʼ�鿪ʼ���Ρ������ŷš�
*******************************************************************************/
__s32 write_in_many_blks( __u32 start_blk, __u32 last_blk_num, void *buf,
					      __u32 size, __u32 blk_size, __u32 * blks )
{
    __u32 buf_base;
	__u32 buf_off;
    __u32 size_writed;
    __u32 rest_size;
    __u32 blk_num;
    __u32 blk_status;
    __s32 status;


	for( blk_num = start_blk, buf_base = (__u32)buf, buf_off = 0, blk_status = ADV_NF_NO_NEW_BAD_BLOCK;
         blk_num <= last_blk_num && buf_off < size;
         blk_num++ )
    {
    	status = NF_read_status( blk_num );
    	if( status == NF_OVERTIME_ERR )
    		return ADV_NF_OVERTIME_ERR;
    	else if( status == NF_BAD_BLOCK )		            // �����ǰ���ǻ��飬�������һ��
    		continue;

    	rest_size = size - buf_off ;                        // δ���벿�ֵĳߴ�
    	size_writed = ( rest_size < blk_size ) ?  rest_size : blk_size ;  // ȷ���˴δ�����ĳߴ�

    	status = write_in_one_blk( blk_num, (void *)buf_base, size_writed, blk_size );
    	if( status == NF_OVERTIME_ERR )
    		return ADV_NF_OVERTIME_ERR;
    	else if( status == NF_ERASE_ERR )
    	{
    		blk_status = ADV_NF_NEW_BAD_BLOCK;
       		continue;
       	}

    	buf_base += size_writed;
    	buf_off  += size_writed;
    }

    *blks = blk_num - start_blk;                            // ����ռ�õĿ������������飩
    if( buf_off == size )
		return blk_status;                                  // �ɹ������ػ�����������
	else
		return ADV_NF_LACK_BLKS;                                // ʧ�ܣ���Ϊ�鲻��
}
/*
************************************************************************************************************
*
*                                             function
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
__s32  Nand_Burn_Boot1(__u32 Boot1_buf, __u32 length )
{
    __u32 	            i;
    __u32               write_blks;
	__s32               status;
	__s32				ret = -1;

    if(NF_ERROR==NF_open( ))                                             // ��nand flash
    {
        __inf("%s: NF_open fail !\n",__FUNCTION__);
		return -1;
    }
    if( length <= NF_BLOCK_SIZE )
    {
        for( i = BOOT1_START_BLK_NUM;  i <= BOOT1_LAST_BLK_NUM;  i++ )
        {
            //if( NF_read_status( i ) == NF_BAD_BLOCK )		// �����ǰ���ǻ��飬�������һ��
            //    continue;

            /* �ڵ�ǰ���������Boot1�ı��� */
            status = write_in_one_blk( i, (void *)Boot1_buf, length, NF_BLOCK_SIZE );
            if( status == ADV_NF_OVERTIME_ERR )
                goto over_time;
        }
    }
    else
    {
    	for( i = BOOT1_START_BLK_NUM;  i <= BOOT1_LAST_BLK_NUM;  i++ )
        {
            //if( NF_read_status( i ) == NF_BAD_BLOCK )		// �����ǰ���ǻ��飬�������һ��
            //    continue;

        	status = write_in_many_blks( i, BOOT1_LAST_BLK_NUM, (void*)Boot1_buf,
            	                        length, NF_BLOCK_SIZE, &write_blks );
            if( status == ADV_NF_OVERTIME_ERR )
                goto over_time;

        	i = i + write_blks -1;
        }
	}

	ret = 0;
over_time:

    NF_close( );

    return ret;
}








