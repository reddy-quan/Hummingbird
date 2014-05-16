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
#include "string.h"

typedef struct
{
	int mul_sel;
	int pull;
	int drv_level;
	int data;
}
gpio_status_set_t;

typedef struct
{
    char    gpio_name[32];
    int port;
	int port_num;
    gpio_status_set_t user_gpio_status;
    gpio_status_set_t hardware_gpio_status;
}
system_gpio_set_t;
/*
****************************************************************************************************
*
*             CSP_PIN_init
*
*  Description:
*       init
*
*  Parameters:
*		p_sys_srv	:	��csp������ϵͳ����
*		p_cfg		:	pin������
*  Return value:
*		EBSP_TRUE/EBSP_FALSE
****************************************************************************************************
*/
__s32 eGon2_GPIO_init(void)          //GPIO��ʼ�������ӿ�
{
	return 0;
}
/*
****************************************************************************************************
*
*             CSP_PIN_exit
*
*  Description:
*       exit
*
*  Parameters:
*
*  Return value:
*		EBSP_TRUE/EBSP_FALSE
****************************************************************************************************
*/
__s32 eGon2_GPIO_exit(void)		    //GPIO�Ƴ������ӿ�
{
	return 0;
}
/*
************************************************************************************************************
*
*                                             CSP_PIN_DEV_req
*
*    �������ƣ�
*
*    �����б�gpio_list      ��������õ���GPIO���ݵ����飬GPIO��ֱ��ʹ���������
*
*			   group_count_max  ����ĳ�Ա������GPIO�趨��ʱ�򣬽�������GPIO��󲻳������ֵ
*
*    ����ֵ  ��
*
*    ˵��    ����ʱû������ͻ���
*
*
************************************************************************************************************
*/
//�û����������ݱ��뱣��
//����ķ��������������û������ʱ��ȥmalloc�����ڵ������ǿ��� malloc������Ƚ϶࣬���ܳ��������С���ݵ����
//�����ڳ�ʼ����ʱ�������GPIOȫ�������,��С��������IC�����п��Է����GPIO�ĸ��������ǱȽ����Կ��ơ������ڱ����ӦGPIO��Ӧ���ݵ�����
//
//������ʹ��malloc�ķ������Ͼ�������
__hdle eGon2_GPIO_Request(user_gpio_set_t *gpio_list, __u32 group_count_max)  //�豸����GPIO�����ӿ�
{
    char               *user_gpio_buf;                                        //����char��������
	system_gpio_set_t  *user_gpio_set, *tmp_sys_gpio_data;					  //user_gpio_set���������ڴ�ľ��
	user_gpio_set_t  *tmp_user_gpio_data;
	__u32				real_gpio_count = 0;                                  //����������Ч��GPIO�ĸ���
	__u32               tmp_group_func_data;
	__u32               tmp_group_pull_data;
	__u32               tmp_group_dlevel_data;
	__u32               tmp_group_data_data;
	__u32               pull_change = 0;
	__u32               dlevel_change = 0, data_change = 0;
	volatile __u32  *tmp_group_func_addr = NULL, *tmp_group_pull_addr = NULL;
	volatile __u32  *tmp_group_dlevel_addr = NULL, *tmp_group_data_addr = NULL;
	__u32  port, port_num, port_num_func, port_num_pull;
	__u32  pre_port = 0x7fffffff, pre_port_num_func = 0x7fffffff;
	__u32  pre_port_num_pull = 0x7fffffff;
	__s32               i, tmp_val;

	//����û����������ݵ���Ч��
	if((!gpio_list) || (!group_count_max))
	{
		return (__hdle)0;
	}
	//�鿴�û�����������Ч�ĸ���
	for(i = 0; i < group_count_max; i++)
	{
		tmp_user_gpio_data = gpio_list + i;                 //gpio_set����ָ��ÿ��GPIO�����Ա
	    if(!tmp_user_gpio_data->port)
	    {
	    	continue;
	    }
	    real_gpio_count ++;
	}
    //������Ч��GPIO��������ռ�
	user_gpio_buf = (char *)eGon2_malloc(16 + sizeof(system_gpio_set_t) * real_gpio_count);   //�����ڴ棬������16���ֽڣ����ڴ��GPIO��������Ϣ
	if(!user_gpio_buf)
	{
		return (__hdle)0;
	}
	memset(user_gpio_buf, 0, 16 + sizeof(system_gpio_set_t) * real_gpio_count);         //����ȫ������
	*(int *)user_gpio_buf = real_gpio_count;                                           //������Ч��GPIO����
	user_gpio_set = (system_gpio_set_t *)(user_gpio_buf + 16);                         //ָ���һ���ṹ��
	//�����û�����
	for(i = 0; i < group_count_max; i++)
	{
		tmp_sys_gpio_data  = user_gpio_set + i;             //tmp_sys_gpio_dataָ�������GPIO�ռ�
		tmp_user_gpio_data = gpio_list + i;                 //gpio_set����ָ���û���ÿ��GPIO�����Ա
	    port     = tmp_user_gpio_data->port;                //�����˿���ֵ
	    port_num = tmp_user_gpio_data->port_num;            //�����˿��е�ĳһ��GPIO
	    if(!port)
	    {
	    	continue;
	    }
	    //��ʼ�����û�����
	    strcpy(tmp_sys_gpio_data->gpio_name, tmp_user_gpio_data->gpio_name);
	    tmp_sys_gpio_data->port                       = port;
	    tmp_sys_gpio_data->port_num                   = port_num;
	    tmp_sys_gpio_data->user_gpio_status.mul_sel   = tmp_user_gpio_data->mul_sel;
	    tmp_sys_gpio_data->user_gpio_status.pull      = tmp_user_gpio_data->pull;
	    tmp_sys_gpio_data->user_gpio_status.drv_level = tmp_user_gpio_data->drv_level;
	    tmp_sys_gpio_data->user_gpio_status.data      = tmp_user_gpio_data->data;


        port_num_func = (port_num >> 3);
        port_num_pull = (port_num >> 4);

        if(tmp_group_func_addr)                             //������Ĵ��������Ƿ񱻸�ֵ
        {
            if((port_num_pull != pre_port_num_pull) || (port != pre_port))    //������ֵ�ǰ���ŵĶ˿ڲ�һ�£��������ڵ�pull�Ĵ�����һ��
            {
                *tmp_group_func_addr   = tmp_group_func_data;    //��д���ܼĴ���
                if(pull_change)
                {
                    pull_change = 0;
                    *tmp_group_pull_addr   = tmp_group_pull_data;    //��дpull�Ĵ���
                }
                if(dlevel_change)
                {
                    dlevel_change = 0;
                    *tmp_group_dlevel_addr = tmp_group_dlevel_data;  //��дdriver level�Ĵ���
                }
                if(data_change)
                {
                    data_change = 0;
                    *tmp_group_data_addr   = tmp_group_data_data;    //��д
                }

                tmp_group_func_addr    = PIO_REG_CFG(port, port_num_func);   //���¹��ܼĴ�����ַ
                tmp_group_pull_addr    = PIO_REG_PULL(port, port_num_pull);   //����pull�Ĵ���
                tmp_group_dlevel_addr  = PIO_REG_DLEVEL(port, port_num_pull); //����level�Ĵ���
                tmp_group_data_addr    = PIO_REG_DATA(port);                  //����data�Ĵ���

                tmp_group_func_data    = *tmp_group_func_addr;
                tmp_group_pull_data    = *tmp_group_pull_addr;
                tmp_group_dlevel_data  = *tmp_group_dlevel_addr;
                tmp_group_data_data    = *tmp_group_data_addr;
            }
            else if(pre_port_num_func != port_num_func)                       //������ֵ�ǰ���ŵĹ��ܼĴ�����һ��
            {
                *tmp_group_func_addr   = tmp_group_func_data;    //��ֻ��д���ܼĴ���
                tmp_group_func_addr    = PIO_REG_CFG(port, port_num_func);   //���¹��ܼĴ�����ַ

                tmp_group_func_data    = *tmp_group_func_addr;
            }
        }
        else
        {
            tmp_group_func_addr    = PIO_REG_CFG(port, port_num_func);   //���¹��ܼĴ�����ַ
            tmp_group_pull_addr    = PIO_REG_PULL(port, port_num_pull);   //����pull�Ĵ���
            tmp_group_dlevel_addr  = PIO_REG_DLEVEL(port, port_num_pull); //����level�Ĵ���
            tmp_group_data_addr    = PIO_REG_DATA(port);                  //����data�Ĵ���

            tmp_group_func_data    = *tmp_group_func_addr;
            tmp_group_pull_data    = *tmp_group_pull_addr;
            tmp_group_dlevel_data  = *tmp_group_dlevel_addr;
            tmp_group_data_data    = *tmp_group_data_addr;
        }
		//���浱ǰӲ���Ĵ�������
        pre_port_num_pull = port_num_pull;                      //���õ�ǰGPIO��Ϊǰһ��GPIO
        pre_port_num_func = port_num_func;
        pre_port          = port;

        //���¹��ܼĴ���
        tmp_val = (port_num - (port_num_func<<3)) << 2;
        tmp_sys_gpio_data->hardware_gpio_status.mul_sel = (tmp_group_func_data >> tmp_val) & 0x07;
        tmp_group_func_data &= ~(                              0x07  << tmp_val);
        tmp_group_func_data |=  (tmp_user_gpio_data->mul_sel & 0x07) << tmp_val;
        //����pull��ֵ�����Ƿ����pull�Ĵ���
        tmp_val              =  (port_num - (port_num_pull<<4)) << 1;
        tmp_sys_gpio_data->hardware_gpio_status.pull = (tmp_group_pull_data >> tmp_val) & 0x03;
        if(tmp_user_gpio_data->pull >= 0)
        {
            tmp_group_pull_data &= ~(                           0x03  << tmp_val);
            tmp_group_pull_data |=  (tmp_user_gpio_data->pull & 0x03) << tmp_val;
            pull_change = 1;
        }
        //����driver level��ֵ�����Ƿ����driver level�Ĵ���
        tmp_sys_gpio_data->hardware_gpio_status.drv_level = (tmp_group_dlevel_data >> tmp_val) & 0x03;
        if(tmp_user_gpio_data->drv_level >= 0)
        {
            tmp_group_dlevel_data &= ~(                                0x03  << tmp_val);
            tmp_group_dlevel_data |=  (tmp_user_gpio_data->drv_level & 0x03) << tmp_val;
            dlevel_change = 1;
        }
        //�����û����룬�Լ����ܷ�������Ƿ����data�Ĵ���
        if(tmp_user_gpio_data->mul_sel == 1)
        {
            if(tmp_val >= 0)
            {
            	tmp_val = tmp_user_gpio_data->data;
            	tmp_val &= 1;
                tmp_group_data_data &= ~(1 << port_num);
                tmp_group_data_data |= tmp_val << port_num;
                data_change = 1;
            }
        }
    }
    //forѭ��������������ڻ�û�л�д�ļĴ���������д�ص�Ӳ������
    if(tmp_group_func_addr)                         //ֻҪ���¹��Ĵ�����ַ���Ϳ��Զ�Ӳ����ֵ
    {                                               //��ô�����е�ֵȫ����д��Ӳ���Ĵ���
        *tmp_group_func_addr   = tmp_group_func_data;       //��д���ܼĴ���
        if(pull_change)
        {
            *tmp_group_pull_addr   = tmp_group_pull_data;    //��дpull�Ĵ���
        }
        if(dlevel_change)
        {
            *tmp_group_dlevel_addr = tmp_group_dlevel_data;  //��дdriver level�Ĵ���
        }
        if(data_change)
        {
            *tmp_group_data_addr   = tmp_group_data_data;    //��дdata�Ĵ���
        }
    }

    return (__hdle)user_gpio_buf;
}

//����ͨIO�������DEVICE��IO��������һ�𣬼����ⲿ�ӿ�
/*
****************************************************************************************************
*
*             CSP_PIN_DEV_release
*
*  Description:
*       �ͷ�ĳ�߼��豸��pin
*
*  Parameters:
* 		p_handler	:	handler
*
*  Return value:
*		EBSP_TRUE/EBSP_FALSE
****************************************************************************************************
*/
__s32 eGon2_GPIO_Release(__hdle p_handler, __s32 if_release_to_default_status)
{
    char               *tmp_buf;                                        //ת����char����
    __u32               group_count_max;                                //���GPIO����
    system_gpio_set_t  *user_gpio_set, *tmp_sys_gpio_data;
    __u32               tmp_group_func_data;
    volatile __u32  *tmp_group_func_addr = NULL;
    __u32               port, port_num, port_num_func;
    __u32               pre_port = 0x7fffffff, pre_port_num_func = 0x7fffffff;
    __u32               i;

    //��鴫���ľ������Ч��
    if(!p_handler)
    {
        return EGON2_FAIL;
    }
    tmp_buf = (char *)p_handler;
    group_count_max = *(int *)tmp_buf;
    if(!group_count_max)
    {
        return EGON2_FAIL;
    }
    if(if_release_to_default_status == 2)
    {
        eGon2_free((char *)p_handler);

        return EGON2_OK;
    }
    user_gpio_set = (system_gpio_set_t *)(tmp_buf + 16);
	//��ȡ�û�����
	for(i = 0; i < group_count_max; i++)
	{
		tmp_sys_gpio_data  = user_gpio_set + i;             //tmp_sys_gpio_dataָ�������GPIO�ռ�
	    port     = tmp_sys_gpio_data->port;                 //�����˿���ֵ
	    port_num = tmp_sys_gpio_data->port_num;             //�����˿��е�ĳһ��GPIO

        port_num_func = (port_num >> 3);

        if(tmp_group_func_addr)                             //������Ĵ��������Ƿ񱻸�ֵ
        {
            if((pre_port_num_func != port_num_func) || (port != pre_port)) //������ֵ�ǰ���ŵĶ˿ڲ�һ�£��������ڵ�pull�Ĵ�����һ��
            {
                *tmp_group_func_addr = tmp_group_func_data;                //��д���ܼĴ���
                tmp_group_func_addr  = PIO_REG_CFG(port, port_num_func);   //���¹��ܼĴ�����ַ

                tmp_group_func_data = *tmp_group_func_addr;
            }
        }
        else
        {
            tmp_group_func_addr = PIO_REG_CFG(port, port_num_func);       //���¹��ܼĴ�����ַ

            tmp_group_func_data = *tmp_group_func_addr;
        }

        pre_port_num_func = port_num_func;
        pre_port          = port;
        //���¹��ܼĴ���
        tmp_group_func_data &= ~(0x07 << ((port_num - (port_num_func<<3)) << 2));
        //����pull��ֵ�����Ƿ����pull�Ĵ���
    }
    if(tmp_group_func_addr)                              //ֻҪ���¹��Ĵ�����ַ���Ϳ��Զ�Ӳ����ֵ
    {                                                    //��ô�����е�ֵȫ����д��Ӳ���Ĵ���
        *tmp_group_func_addr   = tmp_group_func_data;    //��д���ܼĴ���
    }

    //free(p_handler);                                //�ͷ��ڴ�
	eGon2_free((char *)p_handler);

    return EGON2_OK;
}
/*
**********************************************************************************************************************
*                                               CSP_PIN_Get_All_Gpio_Status
*
* Description:
*				��ȡ�û������������GPIO��״̬
* Arguments  :
*		p_handler	:	handler
*		gpio_status	:	�����û����ݵ�����
*		gpio_count_max	:	��������������������Խ��
*       if_get_user_set_flag   :   ��ȡ��־����ʾ��ȡ�û��趨���ݻ�����ʵ������
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
__s32  eGon2_GPIO_Get_All_PIN_Status(__hdle p_handler, user_gpio_set_t *gpio_status, __u32 gpio_count_max, __u32 if_get_from_hardware)
{
    char               *tmp_buf;                                        //ת����char����
    __u32               group_count_max;                                //���GPIO����
    system_gpio_set_t  *user_gpio_set, *tmp_sys_gpio_data;
    user_gpio_set_t  *script_gpio;
    __u32               port_num_func, port_num_pull;
    volatile __u32     *tmp_group_func_addr = NULL, *tmp_group_pull_addr;
    volatile __u32     *tmp_group_data_addr, *tmp_group_dlevel_addr;
    __u32               port, port_num;
    __u32               pre_port = 0x7fffffff, pre_port_num_func = 0x7fffffff, pre_port_num_pull = 0x7fffffff;
    __u32               i;

    //��鴫���ľ������Ч��
    if((!p_handler) || (!gpio_status))
    {
        return EGON2_FAIL;
    }
    if(gpio_count_max <= 0)
    {
        return EGON2_FAIL;
    }
    tmp_buf = (char *)p_handler;
    group_count_max = *(int *)tmp_buf;
    if(group_count_max <= 0)
    {
        return EGON2_FAIL;
    }
    user_gpio_set = (system_gpio_set_t *)(tmp_buf + 16);
    if(group_count_max > gpio_count_max)
    {
        group_count_max = gpio_count_max;
    }
	//��ȡ�û�����
	//��ʾ��ȡ�û�����������
	if(!if_get_from_hardware)
	{
	    for(i = 0; i < group_count_max; i++)
	    {
    	    tmp_sys_gpio_data = user_gpio_set + i;             //tmp_sys_gpio_dataָ�������GPIO�ռ�
    	    script_gpio       = gpio_status + i;               //script_gpioָ���û������Ŀռ�

    	    script_gpio->port      = tmp_sys_gpio_data->port;                       //����port����
    	    script_gpio->port_num  = tmp_sys_gpio_data->port_num;                   //����port_num����
    	    script_gpio->pull      = tmp_sys_gpio_data->user_gpio_status.pull;      //����pull����
    	    script_gpio->mul_sel   = tmp_sys_gpio_data->user_gpio_status.mul_sel;   //������������
    	    script_gpio->drv_level = tmp_sys_gpio_data->user_gpio_status.drv_level; //����������������
    	    script_gpio->data      = tmp_sys_gpio_data->user_gpio_status.data;      //����data����
    	    strcpy(script_gpio->gpio_name, tmp_sys_gpio_data->gpio_name);
	    }
	}
	else
	{
		for(i = 0; i < group_count_max; i++)
    	{
    		tmp_sys_gpio_data = user_gpio_set + i;             //tmp_sys_gpio_dataָ�������GPIO�ռ�
    		script_gpio       = gpio_status + i;               //script_gpioָ���û������Ŀռ�

    	    port     = tmp_sys_gpio_data->port;                //�����˿���ֵ
    	    port_num = tmp_sys_gpio_data->port_num;            //�����˿��е�ĳһ��GPIO

            script_gpio->port = port;                          //����port����
            script_gpio->port_num  = port_num;                 //����port_num����
            strcpy(script_gpio->gpio_name, tmp_sys_gpio_data->gpio_name);

            port_num_func = (port_num >> 3);
            port_num_pull = (port_num >> 4);

            if(tmp_group_func_addr)                             //������Ĵ��������Ƿ񱻸�ֵ
            {
                if((port_num_pull != pre_port_num_pull) || (port != pre_port))    //������ֵ�ǰ���ŵĶ˿ڲ�һ�£��������ڵ�pull�Ĵ�����һ��
                {
                    tmp_group_func_addr    = PIO_REG_CFG(port, port_num_func);   //���¹��ܼĴ�����ַ
                    tmp_group_pull_addr    = PIO_REG_PULL(port, port_num_pull);   //����pull�Ĵ���
                    tmp_group_dlevel_addr  = PIO_REG_DLEVEL(port, port_num_pull); //����level�Ĵ���
                    tmp_group_data_addr    = PIO_REG_DATA(port);                  //����data�Ĵ���
                }
                else if(pre_port_num_func != port_num_func)                       //������ֵ�ǰ���ŵĹ��ܼĴ�����һ��
                {
                    tmp_group_func_addr    = PIO_REG_CFG(port, port_num_func);   //���¹��ܼĴ�����ַ
                }
            }
            else
            {
                tmp_group_func_addr    = PIO_REG_CFG(port, port_num_func);   //���¹��ܼĴ�����ַ
                tmp_group_pull_addr    = PIO_REG_PULL(port, port_num_pull);   //����pull�Ĵ���
                tmp_group_dlevel_addr  = PIO_REG_DLEVEL(port, port_num_pull); //����level�Ĵ���
                tmp_group_data_addr    = PIO_REG_DATA(port);                  //����data�Ĵ���
            }

            pre_port_num_pull = port_num_pull;
            pre_port_num_func = port_num_func;
            pre_port          = port;
            //���û��ؼ���ֵ
            script_gpio->pull      = (*tmp_group_pull_addr   >> ((port_num - (port_num_pull<<4))<<1)) & 0x03;    //����pull����
    	    script_gpio->drv_level = (*tmp_group_dlevel_addr >> ((port_num - (port_num_pull<<4))<<1)) & 0x03;    //������������
    	    script_gpio->mul_sel   = (*tmp_group_func_addr   >> ((port_num - (port_num_pull<<3))<<2)) & 0x07;    //����������������
    	    if(script_gpio->mul_sel <= 1)
    	    {
    	    	script_gpio->data  = (*tmp_group_data_addr   >>   port_num) & 0x01;                              //����data����
        	}
        	else
        	{
        		script_gpio->data = -1;
        	}
        }
	}

	return EGON2_OK;
}
/*
**********************************************************************************************************************
*                                               CSP_PIN_Get_All_Gpio_Status
*
* Description:
*				��ȡ�û������������GPIO��״̬
* Arguments  :
*		p_handler	:	handler
*		gpio_status	:	�����û����ݵ�����
*		gpio_name	:	Ҫ������GPIO������
*       if_get_user_set_flag   :   ��ȡ��־����ʾ��ȡ�û��趨���ݻ�����ʵ������
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
__s32  eGon2_GPIO_Get_One_PIN_Status(__hdle p_handler, user_gpio_set_t *gpio_status, const char *gpio_name, __u32 if_get_from_hardware)
{
    char               *tmp_buf;                                        //ת����char����
    __u32               group_count_max;                                //���GPIO����
    system_gpio_set_t  *user_gpio_set, *tmp_sys_gpio_data;
    __u32               port_num_func, port_num_pull;
    __u32               port, port_num;
    __u32               i, tmp_val1, tmp_val2;

    //��鴫���ľ������Ч��
    if((!p_handler) || (!gpio_status) || (!gpio_name))
    {
        return EGON2_FAIL;
    }
    tmp_buf = (char *)p_handler;
    group_count_max = *(int *)tmp_buf;
    if(group_count_max <= 0)
    {
        return EGON2_FAIL;
    }
    user_gpio_set = (system_gpio_set_t *)(tmp_buf + 16);
	//��ȡ�û�����
	//��ʾ��ȡ�û�����������

    for(i = 0; i < group_count_max; i++)
    {
        tmp_sys_gpio_data = user_gpio_set + i;             //tmp_sys_gpio_dataָ�������GPIO�ռ�
        if(strcmp(gpio_name, tmp_sys_gpio_data->gpio_name))
        {
            continue;
        }
        strcpy(gpio_status->gpio_name, tmp_sys_gpio_data->gpio_name);
        port                   = tmp_sys_gpio_data->port;
        port_num               = tmp_sys_gpio_data->port_num;
        gpio_status->port      = port;                                              //����port����
	    gpio_status->port_num  = port_num;                                          //����port_num����

        if(!if_get_from_hardware)                                                    //��ǰҪ������û���Ƶ�����
        {
        	gpio_status->mul_sel   = tmp_sys_gpio_data->user_gpio_status.mul_sel;   //���û����������ж�����������
            gpio_status->pull      = tmp_sys_gpio_data->user_gpio_status.pull;      //���û����������ж���pull����
            gpio_status->drv_level = tmp_sys_gpio_data->user_gpio_status.drv_level; //���û����������ж���������������
	        gpio_status->data      = tmp_sys_gpio_data->user_gpio_status.data;      //���û����������ж���data����
        }
        else                                                                        //��ǰ�����Ĵ���ʵ�ʵĲ���
        {
            port_num_func = (port_num >> 3);
            port_num_pull = (port_num >> 4);

            tmp_val1 = (port_num - (port_num_func<<3)<<2);
            tmp_val2 = (port_num - (port_num_pull<<4)<<1);
            gpio_status->mul_sel   = (PIO_REG_CFG_VALUE(port, port_num_func)>>tmp_val1) & 0x07;       //��Ӳ���ж������ܼĴ���
            gpio_status->pull      = (PIO_REG_PULL_VALUE(port, port_num_pull)>>tmp_val2) & 0x03;      //��Ӳ���ж���pull�Ĵ���
            gpio_status->drv_level = (PIO_REG_DLEVEL_VALUE(port, port_num_pull)>>tmp_val2) & 0x03;    //��Ӳ���ж���level�Ĵ���
            if(gpio_status->mul_sel <= 1)
            {
                gpio_status->data = (PIO_REG_DATA_VALUE(port) >> port_num) & 0x01;                     //��Ӳ���ж���data�Ĵ���
            }
            else
            {
                gpio_status->data = -1;
            }
        }

        break;
    }

    return EGON2_OK;
}
/*
**********************************************************************************************************************
*                                               CSP_PIN_Set_One_Gpio_Status
*
* Description:
*				��ȡ�û��������GPIO��ĳһ����״̬
* Arguments  :
*		p_handler	:	handler
*		gpio_status	:	�����û����ݵ�����
*		gpio_name	:	Ҫ������GPIO������
*       if_get_user_set_flag   :   ��ȡ��־����ʾ��ȡ�û��趨���ݻ�����ʵ������
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
__s32  eGon2_GPIO_Set_One_PIN_Status(__hdle p_handler, user_gpio_set_t *gpio_status, const char *gpio_name, __u32 if_set_to_current_input_status)
{
    char               *tmp_buf;                                        //ת����char����
    __u32               group_count_max;                                //���GPIO����
    system_gpio_set_t  *user_gpio_set, *tmp_sys_gpio_data;
    user_gpio_set_t     script_gpio;
    volatile __u32     *tmp_addr;
    __u32               port_num_func, port_num_pull;
    __u32               port, port_num;
    __u32               i, reg_val, tmp_val;

    //��鴫���ľ������Ч��
    if((!p_handler) || (!gpio_name))
    {
        return EGON2_FAIL;
    }
    if((if_set_to_current_input_status) && (!gpio_status))
    {
        return EGON2_FAIL;
    }
    tmp_buf = (char *)p_handler;
    group_count_max = *(int *)tmp_buf;
    if(group_count_max <= 0)
    {
        return EGON2_FAIL;
    }
    user_gpio_set = (system_gpio_set_t *)(tmp_buf + 16);
	//��ȡ�û�����
	//��ʾ��ȡ�û�����������
    for(i = 0; i < group_count_max; i++)
    {
        tmp_sys_gpio_data = user_gpio_set + i;             //tmp_sys_gpio_dataָ�������GPIO�ռ�
        if(strcmp(gpio_name, tmp_sys_gpio_data->gpio_name))
        {
            continue;
        }

        port          = tmp_sys_gpio_data->port;                           //����port����
        port_num      = tmp_sys_gpio_data->port_num;                       //����port_num����
        port_num_func = (port_num >> 3);
        port_num_pull = (port_num >> 4);

        if(if_set_to_current_input_status)                                 //���ݵ�ǰ�û��趨����
        {
            //�޸�FUCN�Ĵ���
            script_gpio.mul_sel   = gpio_status->mul_sel;
            script_gpio.pull      = gpio_status->pull;
            script_gpio.drv_level = gpio_status->drv_level;
            script_gpio.data      = gpio_status->data;
        }
        else
        {
            script_gpio.mul_sel   = tmp_sys_gpio_data->user_gpio_status.mul_sel;
            script_gpio.pull      = tmp_sys_gpio_data->user_gpio_status.pull;
            script_gpio.drv_level = tmp_sys_gpio_data->user_gpio_status.drv_level;
            script_gpio.data      = tmp_sys_gpio_data->user_gpio_status.data;
        }

        if(script_gpio.mul_sel >= 0)
        {
        	tmp_addr = PIO_REG_CFG(port, port_num_func);
            reg_val = *tmp_addr;                      								 //�޸�FUNC�Ĵ���
            tmp_val = (port_num - (port_num_func<<3))<<2;
            reg_val &= ~(0x07 << tmp_val);
            reg_val |=  (script_gpio.mul_sel) << tmp_val;
            *tmp_addr = reg_val;
        }
        //�޸�PULL�Ĵ���
        if(script_gpio.pull >= 0)
        {
        	tmp_addr = PIO_REG_PULL(port, port_num_pull);
            reg_val = *tmp_addr;								                     //�޸�FUNC�Ĵ���
            tmp_val = (port_num - (port_num_pull<<4))<<1;
            reg_val &= ~(0x07 << tmp_val);
            reg_val |=  (script_gpio.pull) << tmp_val;
            *tmp_addr = reg_val;
        }
        //�޸�DLEVEL�Ĵ���
        if(script_gpio.drv_level >= 0)
        {
        	tmp_addr = PIO_REG_DLEVEL(port, port_num_pull);
            reg_val = *tmp_addr;                       								  //�޸�FUNC�Ĵ���
            tmp_val = (port_num - (port_num_pull<<4))<<1;
            reg_val &= ~(0x07 << tmp_val);
            reg_val |=  (script_gpio.drv_level) << tmp_val;
            *tmp_addr = reg_val;
        }
        //�޸�data�Ĵ���
        if(script_gpio.mul_sel == 1)
        {
            if(script_gpio.data >= 0)
            {
            	tmp_addr = PIO_REG_DATA(port);
                reg_val = *tmp_addr;                                   				   //�޸�DATA�Ĵ���
                reg_val &= ~(0x01 << port_num);
                reg_val |=  (script_gpio.data & 0x01) << port_num;
                *tmp_addr = reg_val;
            }
        }

        break;
    }

    return EGON2_OK;
}

__s32  eGon2_GPIO_Set_One_PIN_IO_Status(__hdle p_handler, __u32 if_set_to_output_status, const char *gpio_name)
{
    char               *tmp_buf;                                        //ת����char����
    __u32               group_count_max;                                //���GPIO����
    system_gpio_set_t  *user_gpio_set = NULL, *tmp_sys_gpio_data;
    volatile __u32      *tmp_group_func_addr = NULL;
    __u32               port, port_num, port_num_func;
    __u32				i, reg_val;

    //��鴫���ľ������Ч��
    if(!p_handler)
    {
        return EGON2_FAIL;
    }
    if(if_set_to_output_status > 1)
    {
        return EGON2_FAIL;
    }
    tmp_buf = (char *)p_handler;
    group_count_max = *(int *)tmp_buf;
    tmp_sys_gpio_data = (system_gpio_set_t *)(tmp_buf + 16);
    if(group_count_max == 0)
    {
        return EGON2_FAIL;
    }
    else if(group_count_max == 1)
	{
		user_gpio_set = tmp_sys_gpio_data;
	}
	else if(gpio_name)
	{
		for(i=0; i<group_count_max; i++)
		{
			if(strcmp(gpio_name, tmp_sys_gpio_data->gpio_name))
			{
			    tmp_sys_gpio_data ++;
				continue;
			}
			user_gpio_set = tmp_sys_gpio_data;
			break;
		}
	}
	if(!user_gpio_set)
	{
		return EGON2_FAIL;
	}

    port     = user_gpio_set->port;
    port_num = user_gpio_set->port_num;
    port_num_func = port_num >> 3;

    tmp_group_func_addr = PIO_REG_CFG(port, port_num_func);
    reg_val = *tmp_group_func_addr;
    reg_val &= ~(0x07 << ((port_num - (port_num_func<<3)<<2)));
    reg_val |=  (if_set_to_output_status << ((port_num - (port_num_func<<3)<<2)));
    *tmp_group_func_addr = reg_val;

    return EGON2_OK;
}
__s32  eGon2_GPIO_Set_One_PIN_Pull(__hdle p_handler, __u32 set_pull_status, const char *gpio_name)
{
    char               *tmp_buf;                                        //ת����char����
    __u32               group_count_max;                                //���GPIO����
    system_gpio_set_t  *user_gpio_set, *tmp_sys_gpio_data;
    volatile __u32      *tmp_group_func_addr = NULL;
    __u32               port, port_num, port_num_pull;
	__u32				i, reg_val;
    //��鴫���ľ������Ч��
    if(!p_handler)
    {
        return EGON2_FAIL;
    }
    if(set_pull_status >= 4)
    {
        return EGON2_FAIL;
    }
    tmp_buf = (char *)p_handler;
    group_count_max = *(int *)tmp_buf;
    tmp_sys_gpio_data = (system_gpio_set_t *)(tmp_buf + 16);
    if(group_count_max == 0)
    {
        return EGON2_FAIL;
    }
    else if(group_count_max == 1)
	{
		user_gpio_set = tmp_sys_gpio_data;
	}
	else if(gpio_name)
	{
		for(i=0; i<group_count_max; i++)
		{
			if(strcmp(gpio_name, tmp_sys_gpio_data->gpio_name))
			{
			    tmp_sys_gpio_data ++;
				continue;
			}
			user_gpio_set = tmp_sys_gpio_data;
			break;
		}
	}
	if(!user_gpio_set)
	{
		return EGON2_FAIL;
	}

    port     = user_gpio_set->port;
    port_num = user_gpio_set->port_num;
    port_num_pull = port_num >> 4;

    tmp_group_func_addr = PIO_REG_PULL(port, port_num_pull);
    reg_val = *tmp_group_func_addr;
    reg_val &= ~(0x03 << ((port_num - (port_num_pull<<4)<<1)));
    reg_val |=  (set_pull_status << ((port_num - (port_num_pull<<4)<<1)));
    *tmp_group_func_addr = reg_val;

    return EGON2_OK;
}
__s32  eGon2_GPIO_Set_One_PIN_driver_level(__hdle p_handler, __u32 set_driver_level, const char *gpio_name)
{
    char               *tmp_buf;                                        //ת����char����
    __u32               group_count_max;                                //���GPIO����
    system_gpio_set_t  *user_gpio_set, *tmp_sys_gpio_data;
    volatile __u32      *tmp_group_func_addr = NULL;
    __u32               port, port_num, port_num_dlevel;
	__u32				i, reg_val;
    //��鴫���ľ������Ч��
    if(!p_handler)
    {
        return EGON2_FAIL;
    }
    if(set_driver_level >= 4)
    {
        return EGON2_FAIL;
    }
    tmp_buf = (char *)p_handler;
    group_count_max = *(int *)tmp_buf;
    tmp_sys_gpio_data = (system_gpio_set_t *)(tmp_buf + 16);

    if(group_count_max == 0)
    {
        return EGON2_FAIL;
    }
    else if(group_count_max == 1)
	{
		user_gpio_set = tmp_sys_gpio_data;
	}
	else if(gpio_name)
	{
		for(i=0; i<group_count_max; i++)
		{
			if(strcmp(gpio_name, tmp_sys_gpio_data->gpio_name))
			{
			    tmp_sys_gpio_data ++;
				continue;
			}
			user_gpio_set = tmp_sys_gpio_data;
			break;
		}
	}
	if(!user_gpio_set)
	{
		return EGON2_FAIL;
	}

    port     = user_gpio_set->port;
    port_num = user_gpio_set->port_num;
    port_num_dlevel = port_num >> 4;

    tmp_group_func_addr = PIO_REG_DLEVEL(port, port_num_dlevel);
    reg_val = *tmp_group_func_addr;
    reg_val &= ~(0x03 << ((port_num - (port_num_dlevel<<4)<<1)));
    reg_val |=  (set_driver_level << ((port_num - (port_num_dlevel<<4)<<1)));
    *tmp_group_func_addr = reg_val;

    return EGON2_OK;
}
__s32  eGon2_GPIO_Read_One_PIN_Value(__hdle p_handler, const char *gpio_name)
{
    char               *tmp_buf;                                        //ת����char����
    __u32               group_count_max;                                //���GPIO����
    system_gpio_set_t  *user_gpio_set = NULL, *tmp_sys_gpio_data;
    __u32               port, port_num, port_num_func, func_val;
	__u32				i, reg_val;
    //��鴫���ľ������Ч��
    if(!p_handler)
    {
        return EGON2_FAIL;
    }
    tmp_buf = (char *)p_handler;
    group_count_max = *(int *)tmp_buf;
    tmp_sys_gpio_data = (system_gpio_set_t *)(tmp_buf + 16);

    if(group_count_max == 0)
    {
        return EGON2_FAIL;
    }
    else if(group_count_max == 1)
	{
		user_gpio_set = tmp_sys_gpio_data;
	}
	else if(gpio_name)
	{
		for(i=0; i<group_count_max; i++)
		{
			if(strcmp(gpio_name, tmp_sys_gpio_data->gpio_name))
			{
			    tmp_sys_gpio_data ++;
				continue;
			}
			user_gpio_set = tmp_sys_gpio_data;
			break;
		}
	}
	if(!user_gpio_set)
	{
		return EGON2_FAIL;
	}

    port     = user_gpio_set->port;
    port_num = user_gpio_set->port_num;
    port_num_func = port_num >> 3;

    reg_val  = PIO_REG_CFG_VALUE(port, port_num_func);
    func_val = (reg_val >> (port_num - (port_num_func<<3)<<2)) & 0x07;
    if(func_val == 0)
    {
        reg_val = (PIO_REG_DATA_VALUE(port) >> port_num) & 0x01;

        return reg_val;
    }

    return EGON2_FAIL;
}
__s32  eGon2_GPIO_Write_One_PIN_Value(__hdle p_handler, __u32 value_to_gpio, const char *gpio_name)
{
    char               *tmp_buf;                                        //ת����char����
    __u32               group_count_max;                                //���GPIO����
    system_gpio_set_t  *user_gpio_set = NULL, *tmp_sys_gpio_data;
    volatile __u32     *tmp_group_data_addr = NULL;
    __u32               port, port_num, port_num_func, func_val;
	__u32				i, reg_val;
    //��鴫���ľ������Ч��
    if(!p_handler)
    {
        return EGON2_FAIL;
    }
    if(value_to_gpio >= 2)
    {
        return EGON2_FAIL;
    }
    tmp_buf = (char *)p_handler;
    group_count_max = *(int *)tmp_buf;
    tmp_sys_gpio_data = (system_gpio_set_t *)(tmp_buf + 16);

    if(group_count_max == 0)
    {
        return EGON2_FAIL;
    }
    else if(group_count_max == 1)
	{
		user_gpio_set = tmp_sys_gpio_data;
	}
	else if(gpio_name)
	{
		for(i=0; i<group_count_max; i++)
		{
			if(strcmp(gpio_name, tmp_sys_gpio_data->gpio_name))
			{
                tmp_sys_gpio_data ++;
				continue;
			}
			user_gpio_set = tmp_sys_gpio_data;
			break;
		}
	}
	if(!user_gpio_set)
	{
		return EGON2_FAIL;
	}

    port     = user_gpio_set->port;
    port_num = user_gpio_set->port_num;
    port_num_func = port_num >> 3;

    reg_val  = PIO_REG_CFG_VALUE(port, port_num_func);
    func_val = (reg_val >> (port_num - (port_num_func<<3)<<2)) & 0x07;
    if(func_val == 1)
    {
        tmp_group_data_addr = PIO_REG_DATA(port);
        reg_val = *tmp_group_data_addr;
        reg_val &= ~(1 << port_num);
        reg_val |=  (value_to_gpio << port_num);
        *tmp_group_data_addr = reg_val;

        return EGON2_OK;
    }

    return EGON2_FAIL;
}


