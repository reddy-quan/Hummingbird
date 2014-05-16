/*
************************************************************************************************************************
*                                                         eGON
*                                         the Embedded GO-ON Bootloader System
*
*                             Copyright(C), 2006-2008, SoftWinners Microelectronic Co., Ltd.
*											       All Rights Reserved
*
* File Name : check.c
*
* Author : Gary.Wang
*
* Version : 1.1.0
*
* Date : 2007.10.12
*
* Description : This file provides a function to check Boot0 and Boot1.
*
* Others : None at present.
*
*
* History :
*
*  <Author>        <time>       <version>      <description>
*
* Gary.Wang       2007.10.12      1.1.0        build the file
*
************************************************************************************************************************
*/
#include "lib_i.h"



//#pragma arm section  code="check_magic"
/********************************************************************************
*��������: check_magic
*����ԭ��: __s32 check_magic( __u32 *mem_base, const char *magic )
*��������: ʹ�á������͡���У���ڴ��е�һ������
*��ڲ���: mem_base       Boot�ļ����ڴ��е���ʼ��ַ
*          magic          Boot��magic
*�� �� ֵ: CHECK_IS_CORRECT      У����ȷ
*          CHECK_IS_WRONG        У�����
*��    ע:
********************************************************************************/
__s32 check_magic( __u32 *mem_base, const char *magic )
{
	__u32 i;
	boot_file_head_t *bfh;
	__u32 sz;
	unsigned char *p;


	bfh = (boot_file_head_t *)mem_base;
	p = bfh->magic;
	for( i = 0, sz = sizeof( bfh->magic );  i < sz;  i++ )
	{
		if( *p++ != *magic++ )
			return CHECK_IS_WRONG;
	}


	return CHECK_IS_CORRECT;
}

//#pragma arm section




//#pragma arm section  code="check_sum"
/********************************************************************************
*��������: check_sum
*����ԭ��: __s32 check_sum( __u32 *mem_base, __u32 size, const char *magic )
*��������: ʹ�á������͡���У���ڴ��е�һ������
*��ڲ���: mem_base           ��У����������ڴ��е���ʼ��ַ��������4�ֽڶ���ģ�
*          size               ��У������ݵĸ��������ֽ�Ϊ��λ��������4�ֽڶ���ģ�
*�� �� ֵ: CHECK_IS_CORRECT   У����ȷ
*          CHECK_IS_WRONG     У�����
*��    ע:
********************************************************************************/
__s32 check_sum( __u32 *mem_base, __u32 size )
{
	__u32 *buf;
	__u32 count;
	__u32 src_sum;
	__u32 sum;
	boot_file_head_t  *bfh;


	bfh = (boot_file_head_t *)mem_base;

	/* ����У��� */
	src_sum = bfh->check_sum;                  // ��Boot_file_head�еġ�check_sum���ֶ�ȡ��У���
	bfh->check_sum = STAMP_VALUE;              // ��STAMP_VALUEд��Boot_file_head�еġ�check_sum���ֶ�

	count = size >> 2;                         // �� �֣�4bytes��Ϊ��λ����
	sum = 0;
	buf = (__u32 *)mem_base;
	do
	{
		sum += *buf++;                         // �����ۼӣ����У���
		sum += *buf++;                         // �����ۼӣ����У���
		sum += *buf++;                         // �����ۼӣ����У���
		sum += *buf++;                         // �����ۼӣ����У���
	}while( ( count -= 4 ) > (4-1) );

	while( count-- > 0 )
		sum += *buf++;

	bfh->check_sum = src_sum;                  // �ָ�Boot_file_head�еġ�check_sum���ֶε�ֵ

	if( sum == src_sum )
		return CHECK_IS_CORRECT;               // У��ɹ�
	else
		return CHECK_IS_WRONG;                 // У��ʧ��
}

//#pragma arm section



//#pragma arm section  code="check_file"
/********************************************************************************
*��������: check_file
*����ԭ��: __s32 check_file( __u32 *mem_base, __u32 size, const char *magic )
*��������: ʹ�á������͡���У���ڴ��е�һ������
*��ڲ���: mem_base       ��У����������ڴ��е���ʼ��ַ��������4�ֽڶ���ģ�
*          size           ��У������ݵĸ��������ֽ�Ϊ��λ��������4�ֽڶ���ģ�
*          magic          magic number, ��У���ļ��ı�ʶ��
*�� �� ֵ: CHECK_IS_CORRECT       У����ȷ
*          CHECK_IS_WRONG         У�����
*��    ע:
********************************************************************************/
__s32 check_file( __u32 *mem_base, __u32 size, const char *magic )
{
	if( check_magic( mem_base, magic ) == CHECK_IS_CORRECT
        &&check_sum( mem_base, size  ) == CHECK_IS_CORRECT )
        return CHECK_IS_CORRECT;
    else
    	return CHECK_IS_WRONG;
}

//#pragma arm section
