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

/*
*******************************************************************************
*                     BoardInit_Display
*
* Description:
*    ���� display ����
*
* Parameters:
*    void
*
* Return value:
*    0 : �ɹ�
*   !0 : ʧ��
*
* note:
*    void
*
*******************************************************************************
*/
__s32 BoardInit_Display(__s32 source, __s32 mode)
{
    __s32 ret = 0;
    __s32 source_type, tv_mode, dislpay_device;

    board_res.display_source = source;
    if((source < 0) || (mode < 0))
    {
        board_res.display_source = -1;
        return 0;
    }
    #ifndef CONFIG_AW_HOMELET_PRODUCT
    ret = wBoot_driver_install("c:\\drv_de.drv");
    #else
    ret = wBoot_driver_install("c:\\drv_hdmi.drv");
    #endif
    if(ret != 0)
    {
        DMSG_PANIC("ERR: wBoot_driver_install display driver failed\n");
        return -1;
    }
	//��ǰ����ʾ�豸
	board_res.disp_hd = wBoot_driver_open(EGON2_EMOD_TYPE_DRV_DISP, NULL);
    if(board_res.disp_hd == NULL)
    {
        DMSG_PANIC("ERR: open display driver failed\n");
        return -1;
    }
    source_type = 0;
    tv_mode     = 0;
    switch(source)
    {
        case 1:                                 //CVBS
            source_type = DISP_OUTPUT_TYPE_TV;
            tv_mode = DISP_TV_CVBS;
            break;
        case 2:                                 //YPBPR
            source_type = DISP_OUTPUT_TYPE_TV;
            tv_mode = DISP_TV_YPBPR;
            break;
//        case 3:                                 //SVIDEO
//            source_type = DISP_OUTPUT_TYPE_TV;
//            tv_mode = DISP_TV_SVIDEO;
//            break;
        case 3:                                 //HDMI
            source_type = DISP_OUTPUT_TYPE_HDMI;
            break;
        case 4:                                 //VGA
            source_type = DISP_OUTPUT_TYPE_VGA;
            break;
        default:
            source_type = DISP_OUTPUT_TYPE_LCD;
            break;
    }
    dislpay_device = (source_type << 16) | (tv_mode << 8) | ((mode << 0));
    //����ʾ��Ļ
    ret = De_OpenDevice(dislpay_device);
    if(ret != 0)
    {
        DMSG_PANIC("ERR: De Open LCD or TV failed\n");
        return -1;
    }
	board_res.layer_hd = De_RequestLayer(DISP_LAYER_WORK_MODE_NORMAL);
	if(board_res.layer_hd == NULL)
	{
        DMSG_PANIC("ERR: De_RequestLayer failed\n");
        return -1;
	}

    return 0;
}

/*
*******************************************************************************
*                     BoardExit_Display
*
* Description:
*    ж�� display ����
*
* Parameters:
*    void
*
* Return value:
*    0 : �ɹ�
*   !0 : ʧ��
*
* note:
*    void
*
*******************************************************************************
*/
__s32 BoardExit_Display(int logo_status, int boot_status)
{
    if(board_res.display_source < 0)
    {
        return 0;
    }
    if(board_res.disp_hd == NULL)
    {
        DMSG_PANIC("ERR: display driver not open yet\n");
    }
	else if(boot_status)
	{
		wBoot_driver_ioctl(board_res.disp_hd, DISP_CMD_LCD_OFF, 0, 0);
		{
			int i;

			for(i=0;i<200000;i++);
		}
	}
	else
	{
	    if(!logo_status)
	    {
	    	wBoot_driver_ioctl(board_res.disp_hd, DISP_CMD_SET_EXIT_MODE, DISP_EXIT_MODE_CLEAN_PARTLY, 0);
	    	//�˳���ʾ������ֻ�ر��жϣ����ǲ��ر���Ļ
	    }
	    else
	    {
	    	wBoot_driver_ioctl(board_res.disp_hd, DISP_CMD_LCD_OFF, 0, 0);
	    	//�ر���Ļ
	    }
	}

    return wBoot_driver_uninstall(EGON2_EMOD_TYPE_DRV_DISP);
}

/*
*******************************************************************************
*                     BoardInit
*
* Description:
*    Ӳ����ʼ����
*
* Parameters:
*    :  input.  Boot�׶εĲ�����
*
* Return value:
*    ����Ӳ����ʼ���Ľ�����ɹ�����ʧ�ܡ�
*
* note:
*    ��
*
*******************************************************************************
*/
__s32 BoardInit(void)
{
    /* ��ʼ�� WatchDog */

    /* ��ʼ�� PIO */

    /* ��ʼ�� Jtag */

    /* ��ʼ�� Uart */
    /* ��ʼ�� Key */
    /* ��ʼ�� IR */

    /* ��ʼ�� FM */

    /* ��ʼ�� ��ʾ��� */
    //BoardInit_Display();

    return 0;
}

/*
*******************************************************************************
*                     BoardExit
*
* Description:
*    �ر����е�Ӳ����Դ��
*
* Parameters:
*    :  input.  Boot�׶εĲ�����
*
* Return value:
*    ���سɹ�����ʧ�ܡ�
*
* note:
*    ��
*
*******************************************************************************
*/
__s32 BoardExit(int logo_status, int boot_status)
{
    /* �ر� ��ʾ��� */
    BoardExit_Display(logo_status, boot_status);

    /* �ر� FM */

    /* �ر� IR */

    /* �ر� Key */
    /* �ر� Uart */

    /* �ر� Jtag */

    /* �ر� PIO */

    /* �ر� WatchDog */
    return 0;
}


