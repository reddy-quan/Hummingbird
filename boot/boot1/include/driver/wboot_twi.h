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
#ifndef	_WBOOT_TWI_H_
#define	_WBOOT_TWI_H_

typedef enum  wboot_twi_cmd_set
{
    WBOOT_TWI_SETFREQUENCY,                                  //����TWI����Ƶ��
    WBOOT_TWI_
}
wboot_twi_cmd_set_t;

//eGon2������twi������������������紫������ʹ�õ����ݽṹ
typedef struct  _twi_init_arg
{
	__u32   twi_control_index;      //TWI��������ţ�0��Ӧ������0,1��Ӧ������1����������
	__u32   twi_run_clock;          //TWI���е�ʱ��Ƶ�ʣ��� Hz Ϊ��λ
}
_twi_init_arg_t;

typedef struct  _twi_arg
{
    __u32    slave_addr;             //���豸��ַ
    __u32    slave_addr_flag;        //�����õ�ַ���  0��7bit�� 1��10bit
	__u8    *byte_addr;              //��Ҫ��д�������ڴ��豸�еĵ�ַ
	__u32    byte_addr_width;        //�����õ�ַ���  0��1��8 bit; ���������ִ����ֽ���
	__u32    byte_count;             //һ�ζ�дҪ��ɵ��ֽ���
	__u32    if_restart;             //�Ƿ���Ҫ����restart����
	__u8    *data;                   //���ݵĵ�ַ
}
_twi_arg_t;




#endif	//_WBOOT_TWI_H_

/* end of _WBOOT_TWI_H_ */

