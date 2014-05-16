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
#include "include.h"

#define   KEY_DELAY_MAX          (8)
#define   KEY_MAX_COUNT_GO_ON    ((KEY_DELAY_MAX * 1000)/40)
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
//static __s32 check_vol_to_fel(__u32 vol_threshold)
//{
//    __u32 battery_vol;
//    __u32 bat_exist, dcin_exist;
//
//    bat_exist = 0;
//    dcin_exist = 0;
//    if(!eGon2_power_get_dcin_battery_exist(&dcin_exist, &bat_exist))
//    {
//        if(!dcin_exist)
//        {
//            if(!eGon2_power_get_battery_vol(&battery_vol))
//            {
//                eGon2_printf("bat vol = %d\n", battery_vol);
//                if(battery_vol < vol_threshold)
//                {
//                    return 0;
//                }
//            }
//        }
//    }
//
//    return -1;
//}
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
static __s32 check_key_to_fel(void)
{
    __s32 ret, count, time_tick;
    __s32 value_old, value_new, value_cnt;
    __s32 new_key, new_key_flag;

	eGon2_printf("key\n");
    eGon2_key_get_status();
    eGon2_timer_delay(10);

    time_tick = 0;
    count = 0;
    value_cnt = 0;
    new_key = 0;
    new_key_flag = 0;
    ret = eGon2_key_get_value();  			//��ȡ������Ϣ
    if(ret < 0)             				//û�а�������
    {
        eGon2_printf("no key found\n");
        return -1;
    }
    else
    {
    	value_old = ret;
    }

    while(1)
    {
        time_tick ++;
        ret = eGon2_power_get_short_key();  //��ȡpower������Ϣ
        if(ret > 0)              	  		//û��POWER��������
        {
            count ++;
        }
        eGon2_timer_delay(40);
        ret = eGon2_key_get_value();  		//��ȡ������Ϣ
        if(ret < 0)             			//û�а�������
        {
            eGon2_printf("key not pressed anymore\n");
            if(count == 1)
            {
            	if(new_key >= 2)
            	{
            		eGon2_printf("force to debug mode\n");

            		return -2;
            	}
            }
        	if(value_cnt >= 2)
        	{
        		return value_old;
        	}
        	else
        	{
        		return -1;
        	}
        }
        else
        {
        	value_new = ret;
        	if(value_old == value_new)
        	{
        		value_cnt ++;
        		if(new_key_flag == 1)
        		{
        			new_key ++;
        			new_key_flag ++;
        		}
        		else if(!new_key_flag)
        		{
        			new_key_flag ++;
        		}
        	}
        	else
        	{
        		new_key_flag = 0;
        		value_old = value_new;
        	}
        }

        if(count == 3)
        {
        	eGon2_printf("you can release the key to update now\n");
            return 0;
        }

        if((!count) && (time_tick >= KEY_MAX_COUNT_GO_ON))
        {
            eGon2_printf("LRADC key timeout without power key\n");
            return value_old;
        }
    }
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
__u32 eGon2_boot_detect(void)
{
    //������޵�ѹ
//    if(!check_vol_to_fel(BT1_head.prvt_head.core_para.vol_threshold))
//    {
//        return 1;
//    }
    //��鰴��  ��ͨ��������������������3��power����
    return check_key_to_fel();
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
int eGon2_force_to_debug(void)
{
	int					ret, i;
	int 				pre_uart_no, dbg_uart_no;
	user_gpio_set_t		pre_uart_gpio[2];
	user_gpio_set_t		dbg_uart_gpio[2];

	ret = eGon2_script_parser_fetch("uart_para", "uart_debug_port", &pre_uart_no, 1);
	if(ret)
	{
		return -1;
	}
	ret = eGon2_script_parser_mainkey_get_gpio_cfg("uart_para", pre_uart_gpio, 2);
	if(ret)
	{
		return -2;
	}
	ret = eGon2_script_parser_fetch("uart_force_debug", "uart_debug_port", &dbg_uart_no, 1);
	if(ret)
	{
		return -3;
	}
	ret = eGon2_script_parser_mainkey_get_gpio_cfg("uart_force_debug", dbg_uart_gpio, 2);
	if(ret)
	{
		return -4;
	}
	//�ر�ԭ��uart������ʱ�Ӻ�GPIO
	//�ر�ʱ��
	serial_exit(pre_uart_no);
	//�ر�GPIO
	{
		__u32 *reg, value;
		__u32  tmp;

		for(i=0;i<2;i++)
		{
			reg = (__u32 *)(0x1c20800 + (pre_uart_gpio[i].port - 1) * 0x24);

			reg += pre_uart_gpio[i].port_num>>3;
			value = *reg;
			tmp  = (pre_uart_gpio[i].port_num - ((pre_uart_gpio[i].port_num>>3)<<3))<<2;
			value &= ~(0x07 << tmp);
			*reg   = value;
		}
	}
	//������uart������ʱ�Ӻ�GPIO
	{
		normal_gpio_cfg new_uart_gpio[2];

		for(i=0;i<2;i++)
		{
			new_uart_gpio[i].port 	   = dbg_uart_gpio[i].port;
			new_uart_gpio[i].port_num  = dbg_uart_gpio[i].port_num;
			new_uart_gpio[i].mul_sel   = dbg_uart_gpio[i].mul_sel;
			new_uart_gpio[i].pull 	   = dbg_uart_gpio[i].pull;
			new_uart_gpio[i].drv_level = dbg_uart_gpio[i].drv_level;
		}
		serial_init(dbg_uart_no, (normal_gpio_cfg *)new_uart_gpio, 115200, 24000000);
	}
	//�޸������е�����
	eGon2_script_parser_patch("uart_para0", "uart_used", 1);
	eGon2_script_parser_patch("uart_para0", "uart_port", dbg_uart_no);
	eGon2_script_parser_patch_str("uart_para0", "uart_tx", (void *)&dbg_uart_gpio[0], sizeof(user_gpio_set_t));
	eGon2_script_parser_patch_str("uart_para0", "uart_rx", (void *)&dbg_uart_gpio[1], sizeof(user_gpio_set_t));
	eGon2_script_parser_patch("mmc0_para", "sdc_used", 	0);
	//�޸����

	return 0;
}
