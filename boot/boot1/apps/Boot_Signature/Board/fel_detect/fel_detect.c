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
#include "common_res.h"


#define   BAT_VOL_ACTIVE     (10)

int    power_ops_int_status = 0;
__u32  pic_layer_para;

static int show_battery_full(int *status);
static int shut_battery_full(void);
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
__s32 check_file_to_fel(char *name)
{
    H_FILE  hfile = NULL;

    hfile = wBoot_fopen(name, "r");
    if(hfile)
    {
        wBoot_fclose(hfile);
        wBoot_rmfile(name);
        hfile = NULL;
    	hfile = wBoot_fopen(name, "r");
    	if(!hfile)
    	{
    		__inf("dest file is not exist\n");
    	}
    	else
    	{
    		wBoot_fclose(hfile);
    	}

    	return 0;
    }

    return -1;
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
*    ˵��    �������������ɣ�ֱ�ӹػ�
*
*
************************************************************************************************************
*/
__s32 check_natch_time(char *file_name, __u32 work_mode)
{
    H_FILE  hfile = NULL;
    __s32   ret = -1;

    hfile = wBoot_fopen(file_name, "r");
    if(hfile)
    {
        if(work_mode == WORK_MODE_PRODUCT)
        {
            ret = 0;
        }
        wBoot_fclose(hfile);
        wBoot_rmfile(file_name);
    }

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
*    ����ֵ  �� 0:��������
*			   -1:�ػ�
*
*    ˵��    �������������ɣ�ֱ�ӹػ�
*
*
************************************************************************************************************
*/
__s32 check_power_status(void)
{
	__s32 status;
	__s32 power_start;
    
#ifdef CONFIG_AW_HOMELET_PRODUCT
    return 0;
#endif

	status = wBoot_power_get_level();
	if(status == BATTERY_RATIO_TOO_LOW_WITHOUT_DCIN)						//�͵�״̬�£����ⲿ��Դ��ֱ�ӹػ�
	{
		__inf("battery low power with no dc or ac, should set power off\n");
		ShowPictureEx("c:\\os_show\\low_pwr.bmp", 0);
		wBoot_timer_delay(3000);

		return -1;
	}
	power_start = 0;
   // 0: ��������ţֱ�ӿ���������ͨ���жϣ�����������������ֱ�ӿ���������power������ǰ����ϵͳ״̬�������ص������ͣ���������
   // 1: ����״̬�£�������ţֱ�ӿ�����ͬʱҪ���ص����㹻��
   // 2: ��������ţֱ�ӿ���������ͨ���жϣ�����������������ֱ�ӿ���������power������ǰ����ϵͳ״̬����Ҫ���ص���
   // 3: ����״̬�£�������ţֱ�ӿ�������Ҫ���ص���
	if(wBoot_script_parser_fetch("target", "power_start", &power_start, 1))
	{
      power_start=0;
	}
    __debug("status=%d\n",status);
    switch(status)
    {
        case BATTERY_RATIO_ENOUGH:
            __inf("battery enough\n"); break;
        case BATTERY_RATIO_TOO_LOW_WITH_DCIN:
            __inf("battery too low with dc\n"); break;
        case BATTERY_RATIO_TOO_LOW_WITHOUT_DCIN:
            __inf("battery too low without dc\n"); break;
        default: break;
    }
    __inf("power_start=%x\n", power_start);
	if(status == BATTERY_RATIO_TOO_LOW_WITH_DCIN)					//�͵磬ͬʱ���ⲿ��Դ״̬��
	{
		if(!(power_start & 0x02))	//��Ҫ�жϵ�ǰ��ص�����Ҫ��power_start�ĵ�1bit��ֵΪ0
		{							//��������£�ֱ�ӹػ�
			__inf("battery low power with dc or ac, should charge longer\n");
			ShowPictureEx("c:\\os_show\\bempty.bmp", 0);
			wBoot_timer_delay(3000);

			return -1;
		}
		else
		{
			if(power_start == 3)	//����Ҫ�жϵ�ǰ��ص��������Ϊ3�������ϵͳ�����Ϊ0������к����ж�
			{
				return 0;
			}
		}
	}
	else							//��ص����㹻�����
	{
		if(power_start & 0x01)		//�����0bit��ֵΪ1�������ϵͳ
		{
			return 0;
		}
	}								//��������£���������ж�

	status = -1;
	status = wBoot_power_check_startup();
#ifdef FORCE_BOOT_STANDBY
	status = 0;
#endif
	if(status)
	{
		return 0;
	}
	{
      
		__u32 dcin, bat_exist;
		__s32 bat_cal, this_bat_cal;
		__u32 bat_show_hd = NULL;
		int   i, j;
		int   bat_full_status = 0;
		//��ǰ����ȷ���ǻ�ţ�����������Ƿ񿪻�����ȷ������Ҫȷ�ϵ���Ƿ����
		WaitForDeInitFinish();//�� LCD init������release hard timer.
		power_int_reg();
		usb_detect_enter();
		bat_show_hd = ShowBatteryCharge_init(0);

		//wBoot_timer_delay(1500);
		dcin = 0;
		bat_exist = 0;
		wBoot_power_get_dcin_battery_exist(&dcin, &bat_exist);
		if(!bat_exist)
		{
			__inf("no battery exist\n");
			ShowBatteryCharge_exit(bat_show_hd);
			power_int_rel();
			usb_detect_exit();

			return 0;
		}
		wlibc_int_disable();
		this_bat_cal = wBoot_power_get_cal();
		wlibc_int_enable();
		__inf("base bat_cal = %d\n", this_bat_cal);
		if(this_bat_cal > 95)
		{
			this_bat_cal = 100;
		}
		if(this_bat_cal == 100)
		{
			ShowBatteryCharge_exit(bat_show_hd);
			bat_show_hd = NULL;
			show_battery_full(&bat_full_status);
			for(i =0;i<12;i++)
			{
				if(power_ops_int_status & 0x02)	//�̰�
				{
					power_ops_int_status &= ~0x02;
					j = 0;
					__inf("short key\n");
				}
				else if(power_ops_int_status & 0x01)	//����
				{
					wlibc_int_disable();
					power_int_rel();
					usb_detect_exit();
					power_ops_int_status &= ~0x01;
					wlibc_int_enable();
					power_int_reg();
					__inf("long key\n");

					return 0;
				}
				wBoot_timer_delay(250);
			}
		}
		else
		{
			int one_delay;

			one_delay = 1000/(10 - (this_bat_cal/10));
			for(j=0;j<3;j++)
			{
				for(i=this_bat_cal;i<110;i+=10)
				{
					ShowBatteryCharge_rate(bat_show_hd, i);
					wBoot_timer_delay(one_delay);
					if(power_ops_int_status & 0x02)	//�̰�
					{
						power_ops_int_status &= ~0x02;
						j = 0;
						__inf("short key\n");
					}
					else if(power_ops_int_status & 0x01)	//����
					{
						ShowBatteryCharge_exit(bat_show_hd);
						wlibc_int_disable();
						power_int_rel();
						usb_detect_exit();
						power_ops_int_status &= ~0x01;
						wlibc_int_enable();
						power_int_reg();
						__inf("long key\n");

						return 0;
					}
				}
			}
			ShowBatteryCharge_rate(bat_show_hd, this_bat_cal);
			wBoot_timer_delay(1000);
		}
		wBoot_power_get_key();
		__inf("extenal power low go high startup\n");
/******************************************************************
*
*	standby ����ֵ˵��
*
*	   -1: ����standbyʧ��
*		1: ��ͨ��������
*		2: ��Դ�����̰�����
*		3: ��Դ������������
*		4: �ⲿ��Դ�Ƴ�����
*		5: ��س�����
*		6: �ڻ���״̬���ⲿ��Դ���Ƴ�
*		7: �ڻ���״̬�³�����
*
******************************************************************/
		do
		{
			
			if(power_ops_int_status & 0x04)
			{
				status = 8;
				power_ops_int_status &= ~0x04;
			}
			else
			{
				wlibc_int_disable();
				power_int_rel();
				usb_detect_exit();
				wlibc_int_enable();
				De_CloseLayer(board_res.layer_hd);
                wBoot_EnableInt(GIC_SRC_NMI);
                __inf("enter standby\n");
				status = wBoot_standby();
				__inf("exit standby by %d\n", status);
                wBoot_DisableInt(GIC_SRC_NMI);

				wlibc_int_disable();
				bat_cal = wBoot_power_get_cal();
				wlibc_int_enable();
				__inf("current bat_cal = %d\n", bat_cal);
				if(bat_cal > this_bat_cal)
				{
					this_bat_cal = bat_cal;
					if(this_bat_cal > 95)
					{
						this_bat_cal = 100;
					}
				}
			}
			switch(status)
			{
				case 2:		//�̰�power�������»���
				{
					power_int_reg();
					De_OpenLayer(board_res.layer_hd);
					if(this_bat_cal == 100)
					{
						if(bat_show_hd)
						{
							ShowBatteryCharge_exit(bat_show_hd);
							bat_show_hd = NULL;
						}
						show_battery_full(&bat_full_status);
						for(i =0;i<12;i++)
						{
							if(power_ops_int_status & 0x02)	//�̰�
							{
								power_ops_int_status &= ~0x02;
								i = 0;
								__msg("short key\n");
							}
							else if(power_ops_int_status & 0x01)	//����
							{
								ShowBatteryCharge_exit(bat_show_hd);
								wlibc_int_disable();
								power_int_rel();
								usb_detect_exit();
								power_ops_int_status &= ~0x01;
								wlibc_int_enable();
								power_int_reg();
								__inf("long key\n");

								return 0;
							}
							wBoot_timer_delay(250);
						}
					}
					else
					{
						int one_delay;

						one_delay = 1000/(10 - (this_bat_cal/10));
						for(j=0;j<3;j++)
						{
							for(i=this_bat_cal;i<110;i+=10)
							{
								ShowBatteryCharge_rate(bat_show_hd, i);
								wBoot_timer_delay(one_delay);
								if(power_ops_int_status & 0x02)	//�̰�
								{
									power_ops_int_status &= ~0x02;
									j = 0;
									__msg("short key\n");
								}
								else if(power_ops_int_status & 0x01)	//����
								{
									ShowBatteryCharge_exit(bat_show_hd);
									wlibc_int_disable();
									power_int_rel();
									usb_detect_exit();
									power_ops_int_status &= ~0x01;
									wlibc_int_enable();
									power_int_reg();
									__inf("long key\n");

									return 0;
								}
							}
						}
						ShowBatteryCharge_rate(bat_show_hd, this_bat_cal);
						wBoot_timer_delay(1000);
					}
				}
				break;

				case 3:		//������Դ����֮�󣬹رյ��ͼ�꣬����ϵͳ
				{
					ShowBatteryCharge_exit(bat_show_hd);
					power_int_reg();

					return 0;
				}

				case 4:		//���Ƴ��ⲿ��Դʱ��������ʾ��ǰ���ͼ���3���ػ�
				case 5:		//����س����ɵ�ʱ����Ҫ�ػ�
					De_OpenLayer(board_res.layer_hd);
					ShowBatteryCharge_rate(bat_show_hd, this_bat_cal);
				case 6:
				case 7:
				{
					power_int_reg();
					if((status != 4) && (status != 5))
					{
						De_OpenLayer(board_res.layer_hd);
						ShowBatteryCharge_rate(bat_show_hd, this_bat_cal);
					}
					wBoot_timer_delay(500);
					if(bat_show_hd)
					{
						ShowBatteryCharge_degrade(bat_show_hd, 10);
						ShowBatteryCharge_exit(bat_show_hd);
					}
					else
					{
						shut_battery_full();
					}

					return -1;
				}
				case 8:		//standby�����м�⵽vbus����
				{
					usb_detect_enter();
					wBoot_timer_delay(600);
					usb_detect_exit();
				}
				break;
				case 9:		//standby�����м�⵽vbus�Ƴ���ͬʱ������ͨdc
				{
					power_set_usbpc();
				}
				break;

				default:
					break;
			}
		}
		while(1);
	}
}

static int show_battery_full(int *status)
{
	if(!*status)
	{
		*status = 1;
		pic_layer_para = ShowPictureEx("c:\\os_show\\bat10.bmp", 0);
	}
	else
	{
		De_OpenLayer(board_res.layer_hd);
	}

	return 0;
}

static int shut_battery_full(void)
{
	int  alpha_step, i;
	int  aplha, delay_time;
	display_layer_info_t *layer_para;

	if(!pic_layer_para)
    {
        return -1;
    }
    layer_para = (display_layer_info_t *)pic_layer_para;
	alpha_step = 5;
	delay_time = 50;
	aplha = layer_para->alpha_val;

	for(i=0xff;i>0;i -= alpha_step)
	{
		layer_para->alpha_en = 1;
		aplha -= alpha_step;
		if(aplha > 0)
		{
			De_SetLayerPara(board_res.layer_hd, layer_para);
			wBoot_timer_delay(delay_time);
			layer_para->alpha_val = aplha;
		}
		else
		{
			break;
		}
	}

	return 0;
}

