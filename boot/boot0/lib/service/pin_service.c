/*
**********************************************************************************************************************
*											        eGon
*						           the Embedded GO-ON Bootloader System
*									       eGON arm boot sub-system
*
*						  Copyright(C), 2006-2010, SoftWinners Microelectronic Co., Ltd.
*                                           All Rights Reserved
*
* File    : pin_ops.c
*
* By      : Jerry
*
* Version : V2.00
*
* Date	  :
*
* Descript:
**********************************************************************************************************************
*/
#include "..\\lib_i.h"

#define GPIO_REG_READ(reg)              (*(volatile unsigned int *)(reg))
#define GPIO_REG_WRITE(reg, value)      ((*(volatile unsigned int *)(reg)) = (value))


extern const boot0_file_head_t  BT0_head;
/*
************************************************************************************************************
*
*                                             normal_gpio_cfg
*
*    �������ƣ�
*
*    �����б�
*
*
*
*    ����ֵ  ��
*
*    ˵��    ��
*
*
************************************************************************************************************
*/
__s32 boot_set_gpio(void  *user_gpio_list, __u32 group_count_max, __s32 set_gpio)
{
#if 1
	normal_gpio_cfg    *tmp_user_gpio_data, *gpio_list;
	__u32				first_port;                      //����������Ч��GPIO�ĸ���
	__u32               tmp_group_func_data;
	__u32               tmp_group_pull_data;
	__u32               tmp_group_dlevel_data;
	__u32               tmp_group_data_data;
	__u32               data_change = 0;
	__u32			   *tmp_group_port_addr;
	__u32     		   *tmp_group_func_addr,   *tmp_group_pull_addr;
	__u32     		   *tmp_group_dlevel_addr, *tmp_group_data_addr;
	__u32  				port, port_num, port_num_func, port_num_pull;
	__u32  				pre_port, pre_port_num_func;
	__u32  				pre_port_num_pull;
	__s32               i, tmp_val;

    //׼����һ��GPIO����
    if(BOOT_STORAGE_CODE_BASE == ((__u32)user_gpio_list & BOOT_STORAGE_CODE_BASE))
    {
    	__s32 tmp = (__u32)user_gpio_list & 0x03;

    	tmp <<= 3;
    	gpio_list = (normal_gpio_cfg *)&BT0_head.prvt_head.storage_gpio[tmp];
    }
    else
    {
    	gpio_list = user_gpio_list;
    }

    for(first_port = 0; first_port < group_count_max; first_port++)
    {
        tmp_user_gpio_data = gpio_list + first_port;
        port     = tmp_user_gpio_data->port;                         //�����˿���ֵ
	    port_num = tmp_user_gpio_data->port_num;                     //�����˿��е�ĳһ��GPIO
	    if(!port)
	    {
	    	continue;
	    }
	    port_num_func = (port_num >> 3);
        port_num_pull = (port_num >> 4);

		tmp_group_port_addr    = PIO_REG_BASE(port);

		tmp_group_func_addr    = tmp_group_port_addr + port_num_func;						  //���¹��ܼĴ�����ַ
		tmp_group_pull_addr    = tmp_group_port_addr + (PIOC_REG_o_PUL0>>2) + port_num_pull;  //����pull�Ĵ���
		tmp_group_dlevel_addr  = tmp_group_port_addr + (PIOC_REG_o_DRV0>>2) + port_num_pull;  //����driver level�Ĵ���
		tmp_group_data_addr    = tmp_group_port_addr + (PIOC_REG_o_DATA>>2); 				  //����data�Ĵ���

        tmp_group_func_data    = GPIO_REG_READ(tmp_group_func_addr);
        tmp_group_pull_data    = GPIO_REG_READ(tmp_group_pull_addr);
        tmp_group_dlevel_data  = GPIO_REG_READ(tmp_group_dlevel_addr);
        tmp_group_data_data    = GPIO_REG_READ(tmp_group_data_addr);

        pre_port          = port;
        pre_port_num_func = port_num_func;
        pre_port_num_pull = port_num_pull;
        //���¹��ܼĴ���
        tmp_val = (port_num - (port_num_func << 3)) << 2;
        tmp_group_func_data &= ~(                              0x07  << tmp_val);
        if(set_gpio)
        {
        	tmp_group_func_data |=  (tmp_user_gpio_data->mul_sel & 0x07) << tmp_val;
        }
        //����pull��ֵ�����Ƿ����pull�Ĵ���
        tmp_val              =  (port_num - (port_num_pull << 4)) << 1;
        if(tmp_user_gpio_data->pull >= 0)
        {
        	tmp_group_pull_data &= ~(                           0x03  << tmp_val);
        	tmp_group_pull_data |=  (tmp_user_gpio_data->pull & 0x03) << tmp_val;
        }
        //����driver level��ֵ�����Ƿ����driver level�Ĵ���
        if(tmp_user_gpio_data->drv_level >= 0)
        {
        	tmp_group_dlevel_data &= ~(                                0x03  << tmp_val);
        	tmp_group_dlevel_data |=  (tmp_user_gpio_data->drv_level & 0x03) << tmp_val;
        }
        //�����û����룬�Լ����ܷ�������Ƿ����data�Ĵ���
        if(tmp_user_gpio_data->mul_sel == 1)
        {
            if(tmp_user_gpio_data->data >= 0)
            {
            	tmp_val = tmp_user_gpio_data->data & 1;
                tmp_group_data_data &= ~(1 << port_num);
                tmp_group_data_data |= tmp_val << port_num;
                data_change = 1;
            }
        }

        break;
	}
	//����Ƿ������ݴ���
	if(first_port >= group_count_max)
	{
	    return -1;
	}
	//�����û�����
	for(i = first_port + 1; i < group_count_max; i++)
	{
		tmp_user_gpio_data = gpio_list + i;                 //gpio_set����ָ���û���ÿ��GPIO�����Ա
	    port     = tmp_user_gpio_data->port;                //�����˿���ֵ
	    port_num = tmp_user_gpio_data->port_num;            //�����˿��е�ĳһ��GPIO
	    if(!port)
	    {
	    	break;
	    }
        port_num_func = (port_num >> 3);
        port_num_pull = (port_num >> 4);

        if((port_num_pull != pre_port_num_pull) || (port != pre_port))    //������ֵ�ǰ���ŵĶ˿ڲ�һ�£��������ڵ�pull�Ĵ�����һ��
        {
            GPIO_REG_WRITE(tmp_group_func_addr, tmp_group_func_data);     //��д���ܼĴ���
            GPIO_REG_WRITE(tmp_group_pull_addr, tmp_group_pull_data);     //��дpull�Ĵ���
            GPIO_REG_WRITE(tmp_group_dlevel_addr, tmp_group_dlevel_data); //��дdriver level�Ĵ���
            if(data_change)
            {
                data_change = 0;
                GPIO_REG_WRITE(tmp_group_data_addr, tmp_group_data_data); //��дdata�Ĵ���
            }

			tmp_group_port_addr    = PIO_REG_BASE(port);

			tmp_group_func_addr    = tmp_group_port_addr + port_num_func;						  //���¹��ܼĴ�����ַ
			tmp_group_pull_addr    = tmp_group_port_addr + (PIOC_REG_o_PUL0>>2) + port_num_pull;  //����pull�Ĵ���
			tmp_group_dlevel_addr  = tmp_group_port_addr + (PIOC_REG_o_DRV0>>2) + port_num_pull;  //����driver level�Ĵ���
			tmp_group_data_addr    = tmp_group_port_addr + (PIOC_REG_o_DATA>>2); 				  //����data�Ĵ���

            tmp_group_func_data    = GPIO_REG_READ(tmp_group_func_addr);
            tmp_group_pull_data    = GPIO_REG_READ(tmp_group_pull_addr);
            tmp_group_dlevel_data  = GPIO_REG_READ(tmp_group_dlevel_addr);
            tmp_group_data_data    = GPIO_REG_READ(tmp_group_data_addr);
        }
        else if(pre_port_num_func != port_num_func)                       //������ֵ�ǰ���ŵĹ��ܼĴ�����һ��
        {
            GPIO_REG_WRITE(tmp_group_func_addr, tmp_group_func_data);    //��ֻ��д���ܼĴ���
            tmp_group_func_addr    = PIO_REG_CFG(port, port_num_func);   //���¹��ܼĴ�����ַ

            tmp_group_func_data    = GPIO_REG_READ(tmp_group_func_addr);
        }
		//���浱ǰӲ���Ĵ�������
        pre_port_num_pull = port_num_pull;                      //���õ�ǰGPIO��Ϊǰһ��GPIO
        pre_port_num_func = port_num_func;
        pre_port          = port;

        //���¹��ܼĴ���
        tmp_val = (port_num - (port_num_func << 3)) << 2;
        if(tmp_user_gpio_data->mul_sel)
        {
        	tmp_group_func_data &= ~(                              0x07  << tmp_val);
        	if(set_gpio)
        	{
        		tmp_group_func_data |=  (tmp_user_gpio_data->mul_sel & 0x07) << tmp_val;
        	}
        }
        //����pull��ֵ�����Ƿ����pull�Ĵ���
        tmp_val              =  (port_num - (port_num_pull << 4)) << 1;
        if(tmp_user_gpio_data->pull >= 0)
        {
        	tmp_group_pull_data &= ~(                           0x03  << tmp_val);
        	tmp_group_pull_data |=  (tmp_user_gpio_data->pull & 0x03) << tmp_val;
        }
        //����driver level��ֵ�����Ƿ����driver level�Ĵ���
        if(tmp_user_gpio_data->drv_level >= 0)
        {
        	tmp_group_dlevel_data &= ~(                                0x03  << tmp_val);
        	tmp_group_dlevel_data |=  (tmp_user_gpio_data->drv_level & 0x03) << tmp_val;
        }
        //�����û����룬�Լ����ܷ�������Ƿ����data�Ĵ���
        if(tmp_user_gpio_data->mul_sel == 1)
        {
            if(tmp_user_gpio_data->data >= 0)
            {
            	tmp_val = tmp_user_gpio_data->data & 1;
                tmp_group_data_data &= ~(1 << port_num);
                tmp_group_data_data |= tmp_val << port_num;
                data_change = 1;
            }
        }
    }
    //forѭ��������������ڻ�û�л�д�ļĴ���������д�ص�Ӳ������
    if(tmp_group_func_addr)                         //ֻҪ���¹��Ĵ�����ַ���Ϳ��Զ�Ӳ����ֵ
    {                                               //��ô�����е�ֵȫ����д��Ӳ���Ĵ���
        GPIO_REG_WRITE(tmp_group_func_addr,   tmp_group_func_data);   //��д���ܼĴ���
        GPIO_REG_WRITE(tmp_group_pull_addr,   tmp_group_pull_data);   //��дpull�Ĵ���
        GPIO_REG_WRITE(tmp_group_dlevel_addr, tmp_group_dlevel_data); //��дdriver level�Ĵ���
        if(data_change)
        {
            GPIO_REG_WRITE(tmp_group_data_addr, tmp_group_data_data); //��дdata�Ĵ���
        }
    }
#endif
    return 0;
}




