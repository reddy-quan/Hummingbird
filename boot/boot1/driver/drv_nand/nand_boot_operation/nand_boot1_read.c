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


__s32  Nand_Load_Boot1(__u32 Boot1_buf, __u32 buf_length )
{
    __u32  i;
    __s32  status;
    __u32 length;
    __u32 read_blks;
	boot_file_head_t  *bfh;

	NF_open( );                         // ��nand flash
	__msg("Succeed in opening nand flash.\n");

    for( i = BOOT1_START_BLK_NUM;  i <= BOOT1_LAST_BLK_NUM;  i++ )
    {
    	if( NF_read_status( i ) == NF_BAD_BLOCK )		// �����ǰ���ǻ��飬�������һ��
            continue;

        /* ���뵱ǰ����ǰ��512�ֽڵ����ݵ�SRAM�У�Ŀ���ǻ�ȡ�ļ�ͷ */
        if( NF_read( i << ( NF_BLK_SZ_WIDTH - NF_SCT_SZ_WIDTH ), (void *)Boot1_buf, 1 )  == NF_OVERTIME_ERR )
			continue;
		__msg("Succeed in reading Boot1 file head.\n");

		/* �쿴�Ƿ����ļ�ͷ */
		if( check_magic( (__u32 *)Boot1_buf, BOOT1_MAGIC ) != 0 )
		{
			__inf("ERROR! block %u doesn't store head of Boot1 copy.\n", i );
			continue;
		}

		bfh = (boot_file_head_t *) Boot1_buf;
        length =  bfh->length;
        if( ( length & ( NF_SECTOR_SIZE - 1 ) ) != 0 )     // length������NF_SECTOR_SIZE�����
        	continue;

		__inf("The size of Boot1 is %x.\n", length );

        if( length <=  NF_BLOCK_SIZE )
        {
        	/* ��һ����������Boot1�ı��� */
        	status = load_and_check_in_one_blk( i, (void *)Boot1_buf, length, NF_BLOCK_SIZE, BOOT1_MAGIC );
        	if( status == ADV_NF_OVERTIME_ERR )            // ��������
        		continue;
        	else if( status == ADV_NF_OK )
        	{
                __inf("Check is correct.\n");
                NF_close( );                        // �ر�nand flash

                return 0;
            }
        }
        else
        {
        	/* �Ӷ����������һ��Boot1�ı��� */
        	status = load_in_many_blks( i, BOOT1_LAST_BLK_NUM, (void*)Boot1_buf,
        								length, NF_BLOCK_SIZE, &read_blks );
        	if( status == ADV_NF_LACK_BLKS )        // ��������
        	{
        		NF_close( );                        // �ر�nand flash

        		return -1;
        	}
        	else if( status == ADV_NF_OVERTIME_ERR )
        		continue;

            if( check_sum( (__u32 *)Boot1_buf, length ) == 0 )
            {
                __inf("The file stored in start block %u is perfect.\n", i );
                NF_close( );                        // �ر�nand flash

                return 0;
            }
        }
    }


	__inf("Can't find a good Boot1 copy in nand.\n");
    NF_close( );                        // �ر�nand flash
    __inf("Ready to quit \"load_Boot1_from_nand\".\n");

    return -1;

}








