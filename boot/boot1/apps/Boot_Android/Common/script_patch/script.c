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
#include "boot_img.h"
#include "script.h"

extern  boot1_private_head_t  boot1_priv_para;

int parser_script_os_sel(void *data_pbuf, int script_len, void *srcipt_buf);
int parser_script_os_img(void *data_pbuf, int script_len, void *srcipt_buf);
//------------------------------------------------------------------------------------------------------------
//
// ����˵��
//
//
// ����˵��
//
//
// ����ֵ
//
//
// ����
//    ��
//
//------------------------------------------------------------------------------------------------------------
__s32 script_patch(char *script_name, void *script_buf, __s32 boot_script_type)
{
	H_FILE script_file = NULL;
	int    ret = -1;
	char   *script_addr = NULL;
	int    script_len;

	//�򿪽ű��ļ����򲻿���ʧ��
	if(!script_name)
	{
	    __inf("the input script is empty\n");

	    return -1;
	}
	script_file = wBoot_fopen(script_name, "rb");
	if(!script_file)
	{
		__inf("unable to open script file %s\n", script_name);

		return -1;
	}
	//���Ȼ�ȡ�ű��ĳ���
	script_len = wBoot_flen(script_file);
	//�����ű����е�����
	script_addr = (char *)wBoot_malloc(script_len);
	if(!script_addr)
	{
	    __inf("unable to malloc memory for script\n");

	    goto _err_out;
	}
	wBoot_fread(script_addr, 1, script_len, script_file);
	wBoot_fclose(script_file);
	script_file = NULL;
	//�����ű������ݽű�����������
	if(!boot_script_type)
	{
	    ret = parser_script_os_sel(script_addr, script_len, script_buf);
	}
	else
	{
	    ret = parser_script_os_img(script_addr, script_len, script_buf);
	}
	if(ret)
	{
		__inf("script is invalid\n");
	}

_err_out:
	//�˳�ʱ��Ĵ���
	if(script_addr)
	{
		wBoot_free(script_addr);
	}
	if(script_file)
	{
		wBoot_fclose(script_file);
		script_file = NULL;
	}

	return ret;
}
#define  THIS_LINE_NULL        (0)
#define  THIS_LINE_MAINKEY     (1)
#define  THIS_LINE_SUBKEY      (2)
#define  THIS_LINE_ERROR       (-1)
//�˺������ص�ǰ�еĳ��ȣ���ָ�뷵�ص�ǰ������
/********************************************
* flag = 0      //��ǰ��ע���У������
*      = 1      //��ǰ���ֶ���
*      = 2      //��ǰ�������У������ֶε���һ��
*      = -1     //��ǰ�в����Ϲ淶������
*********************************************/
static  int  _get_line_status(char  *daddr,  int  *flag, int last_len)
{
    char  *src;
    int    len;
	char   ch;

    src = daddr;
	ch  = *src++;
	last_len --;
	switch(ch)
	{
		case ';':     //ע����
		case 0x0D:    //�س���
		{
			*flag = THIS_LINE_NULL;
		}
		break;

		case '[':    //������
		{
			*flag = THIS_LINE_MAINKEY;
			break;
		}

		default:     //�Ӽ���
		{
			*flag = THIS_LINE_SUBKEY;
			break;
		}
	}

    len = 1;
	ch = *src++;
    while((ch != 0x0A) && (last_len >  len))     //ֻҪ���ǻ��з��ţ���������
    {
		ch = *src++;
		len ++;
        if(len >= 512)
        {
            *flag = THIS_LINE_ERROR;

            return 0;
        }
    }

    return len + 1;
}

//���ҳ��������ַ�������
static  int _fill_line_mainkey(char *pbuf, char *item_name)
{
	char *src;
	char  ch, i;

	i = 0;
	src = pbuf + 1;        //���� ��
	ch  = *src++;

	while(']' != ch)
	{
		item_name[i] = ch;
		i++;
		ch = *src++;
		if(i >= ITEM_MAIN_NAME_MAX)
		{
			item_name[i-1] = 0;

			break;
		}
	}

	return 0;

}

static  int _get_item_value(char *pbuf, char *name, char *value)
{
    char  *src, *dstname, *dstvalue;
    int   len;

    src = pbuf;
    dstname = name;
    dstvalue = value;

    len = 0;
    while(*src != '=')
    {
        if((*src != 0x20) && (*src != 0x09))
        {
            *dstname++ = *src;
            len ++;
        }
        src ++;
        if(len >= 31)
        {
			*dstname = '\0';
			break;
        }
    }
    *dstname = '\0';
    //src ++;      //���� '='
	while(*src != '=')
	{
		src ++;
	}

	src++;
    len = 0;
    while(*src != 0x0D)
    {
        if((*src != 0x20) && (*src != 0x09))
        {
            *dstvalue++ = *src;
            len ++;
        }
        src ++;
        if(len >= 127)
        {
			return -1;
        }
    }
    *dstvalue = '\0';

    return 0;
}
//�˺���ת���ַ����ݳ�Ϊ�������ݣ�����10���ƺ�16����
//ת�����������value�У�����ֵ��־ת���ɹ�����ʧ��
static  int  _get_str2int(char *saddr, int value[] )
{
    char  *src;
    char   off;
    unsigned int  tmp_value = 0;
	int    data_count;
	int    sign = 1;

	data_count = 2;
    src = saddr;
    off = 0;         //0����10���ƣ�1����16����

    if(src[0] == '-')
    {
        sign = -1;
        if((src[1] == '0') && ((src[2] == 'x') || (src[2] == 'X')))      //ʮ������
        {
            src += 3;
            off  = 2;
        }
        else if((src[1] >= '0') && (src[1] <= '9'))         //����10����
        {
            src += 1;
		    off  = 1;
        }
    }
    else
    {
        if((src[0] == '0') && ((src[1] == 'x') || (src[1] == 'X')))      //ʮ������
        {
            src += 2;
            off  = 2;
        }
    	else if((src[0] >= '0') && (src[0] <= '9'))                     //ʮ����
    	{
    		off = 1;
    	}
    }

	//��ʾ���ַ���
	if(off == 0)
	{
		data_count = 0;
		while(src[data_count] != '\0')
		{
			data_count ++;
			if(data_count > 127)
			{
				break;
			}
		}
		if(data_count & 0x03)       //Ҫ�����ֽڶ���
		{
			data_count = (data_count & (~0x03)) + 4;
		}
		value[0] = data_count>>2;


		return DATA_TYPE_STRING;
	}
    else if(off == 1)
    {
        while(*src != '\0')
        {
            if((*src >= '0') && (*src <= '9'))
            {
                tmp_value = tmp_value * 10 + (*src - '0');
                src ++;
            }
            else if((*src == '+') || (*src == '-'))
            {
                break;
            }
            else
            {
                return -1;
            }
        }
        value[0] = tmp_value;
		value[1] = sign;
		value[2] = 1;
        if(*src == '-')
        {
            value[2] = -1;
        }

		return DATA_TYPE_SINGLE_WORD;
    }
    else if(off == 2)
    {
        while(*src != '\0')
        {
			if((*src >= '0') && (*src <= '9'))
            {
                tmp_value = tmp_value * 16 + (*src - '0');
                src ++;
            }
            else if((*src >= 'A') && (*src <= 'F'))
            {
                tmp_value = tmp_value * 16 + (*src - 'A' + 10);
                src ++;
            }
            else if((*src >= 'a') && (*src <= 'f'))
            {
                tmp_value = tmp_value * 16 + (*src - 'a' + 10);
                src ++;
            }
            else if((*src == '+') || (*src == '-'))
            {
                break;
            }
            else
            {
                return -1;
            }
        }
		value[0] = tmp_value;
		value[1] = sign;
		value[2] = 1;
		if(*src == '-')
        {
            value[2] = -1;
        }

		return DATA_TYPE_SINGLE_WORD;
    }
	else
	{
		return -1;
	}
}
//�㷨��һ��һ�еĲ���
//      ���ҵ�ð�ţ�ֱ����Ϊ��ע���У�Ȼ���ҵ��س����з��ţ��ҵ���һ��ȥ    (�﷨����)
//      ���ҵ�'['(��������)����Ϊ��һ������(��ΪA����)���ҳ��������м���ַ������Լ���������
//             �ҵ�������ַ��Լ����ݣ����δ����һ���ڴ���
//      ֱ���ҵ����������(B����)��Ȼ����ΪA�����������
//      ��A�����µ��������ݣ����մӼ�������ֵ��˳�����У��������һ���ڴ����
//      ��¼A�����ĳ���(�Ӽ��ĸ���)����¼A�����µ����ݴ�����ڴ��е�ƫ����

#define TIEM_MAIN_MAX       16

int parser_script_os_sel(void *data_pbuf, int script_len, void *srcipt_buf)
{
	int   ret = -1, i;
	int   os_img_count[8];
	int   next_subkey_flag;
	int   os_index;
	int   line_len, line_status;
	char  sub_name[128], sub_value[128];
	int   value[8];
	char  *src;
    boot_global_info_t  *os_info = (boot_global_info_t *)srcipt_buf;

	src = (char *)data_pbuf;
	os_index = -1;
	memset(os_img_count, -1, 8 * sizeof(int));
    next_subkey_flag = -1;

	while(script_len)
	{
		line_len = _get_line_status(src, &line_status, script_len);//�жϵ�ǰ�е�״̬
		script_len -= line_len;

		switch(line_status)
		{
			case THIS_LINE_NULL:                                //ע����
			{
				break;
			}

			case THIS_LINE_MAINKEY:                             //������
			{
			    memset(sub_name, 0, 128);
				if(_fill_line_mainkey(src, sub_name))  //��ȡһ�����������̱��������������Ϣ
				{
					goto _err;
				}
				next_subkey_flag = -1;
				if(!strcmp(sub_name, "system"))
				{
				    next_subkey_flag = 1;
				}
				else
				{
				    next_subkey_flag = 0;
				    os_index ++;
				    strcpy(os_info->os_name[os_index], sub_name);
				    if(os_index >= 8)
                    {
                        return -1;
                    }
    			}

				break;
			}

			case THIS_LINE_SUBKEY:                              //�Ӽ��У���ǰ���������Ӧ
			{
				if(_get_item_value(src, sub_name, sub_value))   //ת�����ѵȺ��󷽵Ĵ��ڵ�sub_nam���Ⱥ��ҷ��Ĵ�ŵ�sub_value�������ַ�����ʽ
				{
					goto _err;
				}
                //��ȡ����
                ret = _get_str2int(sub_value, value);
				if(ret == -1)           //���ݴ���
				{
					goto _err;
				}

                if(next_subkey_flag == 1)       //����OS����
                {
                    if(!strcmp(sub_name, "start_os_name"))
				    {
				        strncpy(os_info->start_os_name, sub_value, value[0]<<2);
				    }
//				    else if(!strcmp(sub_name, "last_os_name"))
//				    {
//				        tmp_addr = &os_info->last_os_name;
//				    }
				    else if(!strcmp(sub_name, "timeout"))
				    {
				        os_info->user_set_timeout = value[0] * value[1];
				    }
				    else if(!strcmp(sub_name, "display_device"))
				    {
				        os_info->display_device = value[0] * value[1];
				    }
				    else if(!strcmp(sub_name, "display_mode"))
				    {
				        os_info->display_mode = value[0] * value[1];
				    }
                }
                else if(!next_subkey_flag)
                {
                    char tmp_str[] = "os_show[0]";
                    //��ͨ�����ж�
                    for(i=0;i<4;i++)
                    {
                        if(!strcmp(sub_name, tmp_str))
    				    {
    				        if(value[0])
    				        {
    				            strncpy(os_info->os_pic[os_index].name[i], sub_value, value[0]<<2);
    				            os_info->os_pic[os_index].valid_num ++;
    				            break;
    				        }
    				    }
    				    tmp_str[8] ++;
                    }
				}

                break;
			}

			default:
			{
				goto _err;
			}
		}
		src += line_len;
	}

    os_info->os_count = os_index + 1;
    for(i=0;i<8;i++)
    {
        if(!strcmp(os_info->start_os_name, os_info->os_name[i]))
        {
            os_info->start_os_index = i;
            break;
        }
    }

_err:

	ret = ((ret >= 0) ? 0: -1);

	return ret;
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
int parser_script_os_img(void *data_pbuf, int script_len, void *srcipt_buf)
{
	int   ret = -1, fix_flag;
	int   next_subkey_flag, segment_index;
	int   line_len, line_status;
	char  sub_name[128], sub_value[128];
	int   value[8];
	char  *src;
    boot_sys_img_set_t  *os_img = (boot_sys_img_set_t *)srcipt_buf;

	src = (char *)data_pbuf;
	segment_index = -1;

	while(script_len)
	{
		line_len = _get_line_status(src, &line_status, script_len);//�жϵ�ǰ�е�״̬
		script_len -= line_len;

		switch(line_status)
		{
			case THIS_LINE_NULL:                                //ע����
			{
				break;
			}

			case THIS_LINE_MAINKEY:                             //������
			{
			    memset(sub_name, 0, 128);
				if(_fill_line_mainkey(src, sub_name))  //��ȡһ�����������̱��������������Ϣ
				{
					goto _err;
				}
                if(!strcmp(sub_name, "segment"))
				{
				    next_subkey_flag = 1;
				    segment_index ++;
				}
				else if(!strcmp(sub_name, "script_info"))
				{
				    next_subkey_flag = 2;
				}
				else if(!strcmp(sub_name, "logo_info"))
				{
				    next_subkey_flag = 3;
				}
				else if(!strcmp(sub_name, "global"))
				{
				    next_subkey_flag = 4;
				}
				fix_flag = 0;

				break;
			}

			case THIS_LINE_SUBKEY:                              //�Ӽ��У���ǰ���������Ӧ
			{
				if(_get_item_value(src, sub_name, sub_value))   //ת�����ѵȺ��󷽵Ĵ��ڵ�sub_nam���Ⱥ��ҷ��Ĵ�ŵ�sub_value�������ַ�����ʽ
				{
					goto _err;
				}
                //��ȡ����
                ret = _get_str2int(sub_value, value);
				if(ret == -1)           //���ݴ���
				{
					goto _err;
				}

                switch(next_subkey_flag)
                {
                    case 1:
                    {
                        if(!strcmp(sub_name, "img_name"))
    				    {
    				        strncpy(os_img->img_set[segment_index].img_name, sub_value, value[0]<<2);
    				    }
    				    else if(!strcmp(sub_name, "img_size"))
    				    {
    				        os_img->img_set[segment_index].img_size = value[0];
    				        if(fix_flag)
    				        {
    				            os_img->img_set[segment_index].img_base -= value[0];
    				            fix_flag = 0;
    				        }
    				    }
    				    else if(!strcmp(sub_name, "img_base"))
    				    {
    				        if(value[1] < 0)
    				        {
                                value[0] = boot1_priv_para.dram_para.dram_baseaddr + (boot1_priv_para.dram_para.dram_size<<20) - value[0];
    				        }
    				        if(value[2] < 0)
    				        {
    				            if(!os_img->img_set[segment_index].img_size)
    				            {
    				                fix_flag = 1;
    				            }
    				            else
    				            {
    				                value[0] -= os_img->img_set[segment_index].img_size;
    				            }
    				        }
    				        os_img->img_set[segment_index].img_base = value[0];
    				    }
    				    break;
                    }
                    case 2:
                    {
                        if(!strcmp(sub_name, "script_base"))
    				    {
    				        if(value[1] < 0)
    				        {
                                value[0] = boot1_priv_para.dram_para.dram_baseaddr + (boot1_priv_para.dram_para.dram_size<<20) - value[0];
    				        }
    				        if(value[2] < 0)
    				        {
    				            if(!os_img->script_info.img_size)
    				            {
    				                fix_flag = 1;
    				            }
    				            else
    				            {
    				                value[0] -= os_img->script_info.img_size;
    				            }
    				        }
    				        os_img->script_info.img_base = value[0];
    				    }
    				    else if(!strcmp(sub_name, "script_size"))
    				    {
    				        os_img->script_info.img_size = value[0];
    				        if(fix_flag)
    				        {
    				            os_img->script_info.img_base -= os_img->script_info.img_size;
    				            fix_flag = 0;
    				        }
    				    }
    				    break;
                    }
                    case 3:
                    {
                        if(!strcmp(sub_name, "logo_name"))
    				    {
    				        strncpy(os_img->logo_name, sub_value, value[0]<<2);
    				    }
    				    else if(!strcmp(sub_name, "logo_show"))
    				    {
    				        os_img->logo_show = value[0];
    				    }
    				    else if(!strcmp(sub_name, "logo_address"))
				        {
				            if(value[1] < 0)
    				        {
                                value[0] = boot1_priv_para.dram_para.dram_baseaddr + (boot1_priv_para.dram_para.dram_size<<20) - value[0];
		                    }
		                    os_img->logo_address = value[0];
				        }
				        else if(!strcmp(sub_name, "logo_off"))
				        {
				        	os_img->logo_off = value[0];
				        }
    				    break;
                    }
                    case 4:
                    {
                        if(!strcmp(sub_name, "start_mode"))
                        {
                            os_img->start_mode = value[0];
                        }
                        break;
                    }
                    default:
                        break;
                }
                break;
			}
			default:
			{
				goto _err;
			}
		}
		src += line_len;
	}

    os_img->img_count = segment_index + 1;

_err:

	ret = ((ret >= 0) ? 0: -1);

	return ret;
}


