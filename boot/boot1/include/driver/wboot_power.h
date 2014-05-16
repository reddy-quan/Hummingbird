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
#ifndef	_WBOOT_POWER_H_
#define	_WBOOT_POWER_H_

#define   POWER_KEY_EXIST                                1
#define   POWER_KEY_NOTEXIST                             0

#define   BATTERY_RATIO_DEFAULT                          0
#define   BATTERY_RATIO_TOO_LOW_WITHOUT_DCIN             1
#define   BATTERY_RATIO_TOO_LOW_WITH_DCIN                2
#define   BATTERY_RATIO_ENOUGH                           3

#define   AXP_POWER_ON_BY_POWER_KEY       				 0
#define   AXP_POWER_ON_BY_POWER_TRIGGER   				 1

typedef enum
{
    WBOOT_POWER_DCIN_BAT_STATUS,                         //��ȡpower���״̬���Ƿ����dcin���Ƿ���ڵ������, pbuffer = {power_dcin_bat_status_t}
    WBOOT_POWER_GET_VOL        ,                         //��ȡ��ص�ѹ, pbuffer = vol (ֻ��ȡDCDC2)
    WBOOT_POWER_OFF            ,                         //�ػ����޲���
    WBOOT_POWER_GET_KEY        ,                         //��ȡPOWER������Ϣ���Ƿ���ڰ���״̬, �а�������1��û�з���0
    WBOOT_POWER_STARTUP_CHECK  ,                         //����Ƿ���Ҫ�������������ţ��ʱ�򲻿����������¿�����������, �޲���
    WBOOT_POWER_SW1_CONTROL                              //����SW1�Ŀ���״̬, aux = {wboot_power_sw1_status}

}
wboot_power_cmd_set_t;

typedef struct __power_dcin_bat_status_t
{
    __u32 dcin_exist;                                    //�ж��ⲿ��Դ�Ƿ����
    __u32 bat_exist;                                     //�жϵ���Ƿ����
}
power_dcin_bat_status_t;

typedef enum
{
    WBOOT_POWER_SW1_OPEN            ,                          //����SW1��״̬Ϊ��
    WBOOT_POWER_SW1_CLOSE           ,                          //����SW1��״̬Ϊ�ر�
    WBOOT_POWER_SW1_NOCHANGE                                   //����SW1��״̬Ϊ����
}
wboot_power_sw1_status;

#endif	//_WBOOT_POWER_H_

/* end of _WBOOT_POWER_H_ */

