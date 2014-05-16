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
#include "standby.h"


static int eGon2_enter_standby(void);
static int eGon2_exit_standby(void);
static int eGon2_standby_detect(void);
static int eGon2_mod_enter_standby(void);
static int eGon2_mod_exit_standby(void);

static int standby_flag = 0;
static int status;
static boot_dram_para_t standby_dram_para;

void standby_get_dram_para(void)
{
  //     __u32 *addr = (__u32 *)&BT1_head.prvt_head.dram_para;
    memcpy((void *)&standby_dram_para, (void *)&BT1_head.prvt_head.dram_para, sizeof(boot_dram_para_t));
    #if 0
    __inf("standby_dram_para.type=%x\n",standby_dram_para.dram_type);
    __inf("standby_dram_para.dram_clk=%x\n",standby_dram_para.dram_clk);
    __inf("standby_dram_para.dram_rank_num=%x\n",standby_dram_para.dram_rank_num);
    __inf("standby_dram_para.dram_chip_density=%x\n",standby_dram_para.dram_chip_density);
    __inf("standby_dram_para.dram_io_width=%x\n",standby_dram_para.dram_io_width);
    __inf("standby_dram_para.dram_cas=%x\n",standby_dram_para.dram_cas);
    __inf("standby_dram_para.dram_zq=%x\n",standby_dram_para.dram_zq);
    __inf("standby_dram_para.dram_odt_en=%x\n",standby_dram_para.dram_odt_en);
    __inf("standby_dram_para.dram_size=%x\n",standby_dram_para.dram_size);
    __inf("standby_dram_para.dram_baseaddr=%x\n",standby_dram_para.dram_baseaddr);
    __inf("standby_dram_para.dram_tpr0=%x\n",standby_dram_para.dram_tpr0);
    __inf("standby_dram_para.dram_tpr1=%x\n",standby_dram_para.dram_tpr1);
    __inf("standby_dram_para.dram_tpr2=%x\n",standby_dram_para.dram_tpr2);
    __inf("standby_dram_para.dram_tpr3=%x\n",standby_dram_para.dram_tpr3);
    __inf("standby_dram_para.dram_tpr4=%x\n",standby_dram_para.dram_tpr4);
    __inf("standby_dram_para.dram_tpr5=%x\n",standby_dram_para.dram_tpr5);
    __inf("standby_dram_para.dram_emr1=%x\n",standby_dram_para.dram_tpr1);
    __inf("standby_dram_para.dram_emr2=%x\n",standby_dram_para.dram_tpr2);
    __inf("standby_dram_para.dram_emr3=%x\n",standby_dram_para.dram_tpr3);
    #endif
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
int eGon2_standby_mode(void)
{
	__u32 dcin_exist, battery_exist;
	__s32 key_status;
	status = -1;
	//����Ƿ��а�������
	__debug("at the start of %s\n",__FUNCTION__);
	key_status = eGon2_power_get_key();
	if(key_status & 0x01)			//������������£����ܵ�Դ�Ƿ��Ƴ���ֱ�ӽ���ϵͳ
	{
		//�˳����е�����ģ��
		if(standby_flag)
		{
			eGon2_mod_exit_standby();
		}
		return 3;
	}
	//����ⲿ��Դ�Ƿ����
	dcin_exist = 100;
	battery_exist = 100;
	eGon2_power_get_dcin_battery_exist(&dcin_exist, &battery_exist);
	if(!dcin_exist)							//�ⲿ��Դ�Ƴ�
	{
		//�˳����е�����ģ��
		if(standby_flag)
		{
			eGon2_mod_exit_standby();
		}
		return 6;
	}
//	charge_status = eGon2_power_battery_charge_status();		//������
//	if((charge_status > 0) && (battery_exist == 1))
//	{
//		//�˳����е�����ģ��
//		if(standby_flag)
//		{
//			eGon2_mod_exit_standby();
//		}
//		return 7;
//	}
	if(key_status & 0x02)			//���ⲿ��Դ���ڣ�����standby
	{
		//�˳����е�����ģ��
		if(standby_flag)
		{
			eGon2_mod_exit_standby();
		}
		return 2;
	}
	//����ԭ����SP
	eGon2_store_sp();
	//�����µ�SP�������SRAM��
	eGon2_set_sp();

	if(!standby_flag)
	{
		eGon2_mod_enter_standby();
	}
	eGon2_enter_standby();
	//���ڣ�clock������32k��
	do
	{
		//��ʼѭ������Ƿ�ʼ����
		eGon2_halt();
		status = eGon2_standby_detect();
	}
	while(status <= 0);
	//������Ҫ���ѣ��˳�standby
	eGon2_exit_standby();
	//�˳�ģ���standby
	if((status != 8) && (status != 9))
	{
		eGon2_mod_exit_standby();
		standby_flag = 0;
	}
	else
	{
		standby_flag = 1;
	}    
  
	eGon2_restore_sp();

	return status;
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
#define   DCDC2_STANDBY_VOL      (1250)
#define   DCDC3_STANDBY_VOL		 (1250)

//#define STANDBY_CHECK_CRC
#ifdef STANDBY_CHECK_CRC
static unsigned int crc_before_standby[8];
static unsigned int test_rang_begin=0x40000000;
static unsigned int test_rang_last =0x43000000;
void standby_before_check_crc(void)
{
    int i, j;
    int *tmp = (int *)test_rang_begin;
    int crc = 0;

  for(i = 0; i < 8; i++)
  {
        crc = 0;
        for(j = 0; j < 128 * 1024 * 1024; j+=32)
        {
            crc += *tmp;
            tmp += 8;
        }
        crc_before_standby[i]=crc;
   //     if (crc != standby_get_from_rtc(8 + i))
      //  {
    //        eGon2_printf("%d M dram crc err!\n", i*128);
    //    }
    //    else
    //    {
    //        eGon2_printf("%d M dram crc ok!\n", i*128);
    //    }
  }
}


void standby_after_check_crc(void)
{
    int i, j;
    int *tmp = (int *)test_rang_begin;
    int crc = 0;
    int result =1;
    //eGon2_printf("check crc!\n");
   eGon2_printf("test_rang_begin=%x\n",test_rang_begin);
  for(i = 0; i < 8; i++)
  {
        crc = 0;
        for(j = 0; j < 128 * 1024 * 1024; j+=32)
        {
            crc += *tmp;
            tmp += 8;
        }
        if (crc != crc_before_standby[i])
        {   
            
            eGon2_printf("%d M dram crc err!\n", i*128);
            
       //    result =0;
        }
        else
        {
            eGon2_printf("%d M dram crc ok!\n", i*128);
           // result =1;
        }
        
        
  }
//  if(result)
//  {
//       test_rang_begin = ((test_rang_begin-0x40000000)>>1);
//  }else
//  {
//       test_rang_begin +=((test_rang_last-test_rang_begin)>>1);
//  }
}
#endif

static int eGon2_enter_standby(void)
{
	volatile int i;
//	__u8  power_int_status[5];
	//����standby������
	standby_get_dram_para();
	eGon2_config_charge_current(1);
	//����VBUS��粻����
//	eGon2_power_vbus_cur_limit();
//	//���power���ж�pending
//	eGon2_power_int_query(power_int_status);
	//�����ж�
	//__debug("standby int init\n");
	standby_int_init();
	//����clock
	standby_clock_store();
    #ifdef STANDBY_CHECK_CRC
    standby_before_check_crc();
    #endif
    #ifndef CONFIG_AW_FPGA_PLATFORM
	//����dram��֮�������ٷ���dram
	dram_power_save_process(&standby_dram_para);
	//��ֹdrampll���ʹ��
	standby_clock_drampll_ouput(0);
    #endif
	//�л���24M
	standby_clock_to_source(24000000);
    //__debug("after standby_clock_to_source\n");
	//�ر�����pll���
	standby_clock_plldisable();
   // __debug("after standby_clock_plldisable\n");
    
	//���͵�Դ��ѹ���
	eGon2_power_set_dcdc2(DCDC2_STANDBY_VOL);
    //__debug("after standby_clock_plldisable\n");
	//eGon2_power_set_dcdc3(DCDC3_STANDBY_VOL);
	//ʹ�ܵ�Դ�жϣ��ȴ�����
	eGon2_power_int_enable();
	//�л���Ƶ��ȫΪ0
	standby_clock_divsetto0();
    //__debug("after standby_clock_divsetto0 fail\n");
	//�л�apb1��32k
	standby_clock_apb1_to_source(32000);
	//�л�ʱ�ӣ��ر�ʱ��
	for(i=0;i<2000;i++);
	standby_clock_to_source(32000);
	//�ر�24M����
	standby_clock_24m_op(0);
	return 0;
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
static int eGon2_exit_standby(void)
{
	int i;

	//�رյ�Դ�ж�
	eGon2_power_int_disable();
	//���õ�ѹ
	eGon2_power_set_dcdc2(-1);
//	eGon2_power_set_dcdc3(-1);
	//��ԭ����pll��ԭ���򿪵���򿪣�ԭ���رյĲ�����
	
	standby_clock_restore();
	for(i=0;i<80000;i++);//0x100000
	//�л�ʱ�ӵ�PLL1
	standby_clock_to_source(0);
    #ifndef CONFIG_AW_FPGA_PLATFORM
	//��dram���ʹ��
	standby_clock_drampll_ouput(1);
	//����dram
	standby_tmr_enable_watchdog();
	dram_power_up_process(&standby_dram_para);
	standby_tmr_disable_watchdog();
    #endif
    #ifdef STANDBY_CHECK_CRC
    standby_after_check_crc();
    #endif
	//��ԭ�ж�״̬
	standby_int_exit();
	//��ԭ������
	eGon2_config_charge_current(0);
	//��ԭ���е�����ģ��
	//eGon2_timer_delay(50);

	return 0;
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
static int eGon2_standby_detect(void)
{
	int   i;
	__u32 dcin_exist, battery_exist;
	__u8  power_int_status[5];

	//����24M����
	standby_clock_24m_op(1);
	//�л�apb1��24M
	standby_clock_apb1_to_source(24000000);
	//����Ƶ�ʵ�24M
	standby_clock_to_source(24000000);
	standby_clock_divsetback();
	for(i=0;i<2000;i++);
	//����жϴ���
	eGon2_power_int_query(power_int_status);
	//����жϿ�������pending
//	standby_int_query();
//	if(eGon2_key_get_status() == 1)			//��ͨADC��������
//	{
//		return 1;
//	}
//	eGon2_power_vbus_unlimit();
	if(power_int_status[2] & 0x02)			//��Դ�����̰�
	{
		return 2;
	}
	if(power_int_status[2] & 0x01)			//��Դ��������
	{
		return 3;
	}
	dcin_exist = 100;
	battery_exist = 100;
	eGon2_power_get_dcin_battery_exist(&dcin_exist, &battery_exist);
	if(!dcin_exist)							//�ⲿ��Դ�Ƴ�
	{
		return 4;
	}
//	if((power_int_status[1] & 0x04) && (battery_exist==1))			//������
//	{
//		return 5;
//	}
	if(power_int_status[0] & 0x08)			//usb��ţ����
	{
		return 8;
	}
	if(power_int_status[0] & 0x04)			//usb��ţ�Ƴ�
	{
		return 9;
	}

	//��ԭ��32K
	standby_clock_divsetto0();
	standby_clock_apb1_to_source(32000);
	standby_clock_to_source(32000);
	standby_clock_24m_op(0);

	return 0;
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
static int eGon2_mod_enter_standby(void)
{
	int i;

	for(i=0;i<EMOD_COUNT_MAX;i++)
	{
		eGon2_driver_standby(i, EGON2_MOD_ENTER_STANDBY, 0);
	}

	return 0;
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
static int eGon2_mod_exit_standby(void)
{
	int i;

	for(i=0;i<EMOD_COUNT_MAX;i++)
	{
		eGon2_driver_standby(i, EGON2_MOD_EXIT_STANDBY, 0);
	}

	return 0;
}




