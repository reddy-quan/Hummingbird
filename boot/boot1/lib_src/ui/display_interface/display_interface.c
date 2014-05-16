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
#include  "ui_main_i.h"
/*
*******************************************************************************
*                     De_SetBlkColor
*
* Description:
*    ���ñ�����ɫ
*
* Parameters:
*    Color  :  input. ��ɫ
*
* Return value:
*    0  :  �ɹ�
*   !0  :  ʧ��
*
* note:
*    void
*
*******************************************************************************
*/
__s32 De_SetBlkColor(__disp_color_t *Color)
{
    __s32 ret;
    __u32 arg[3];

    if(ui_source.disp_hd == NULL)
    {
        __inf("ERR: display driver not open yet\n");

        return -1;
    }

    arg[0] = (__u32)Color;
    arg[1] = 0;
    arg[2] = 0;
    ret = wBoot_driver_ioctl(ui_source.disp_hd, DISP_CMD_SET_BKCOLOR, 0, (void*)arg);
    if(ret)
    {
        __inf("ERR: wBoot_driver_ioctl DISP_CMD_SET_BKCOLOR failed\n");
    }

    return ret;
}
/*
*******************************************************************************
*                     De_OpenDevice
*
* Description:
*    ��TV
*
* Parameters:
*    void
*
* Return value:
*    0  :  �ɹ�
*   !0  :  ʧ��
*
* note:
*    void
*
*******************************************************************************
*/
__s32 De_OpenDevice(__s32 display_source, __s32 tv_mode, __s32 display_mode)
{
    __s32 ret = 0;
    __s32 aux;
    __u32 arg[3];

    if(ui_source.disp_hd == NULL)
    {
        __inf("ERR: display driver not open yet\n");

        return -1;
    }
    arg[0] = 0;
    arg[1] = 0;
    arg[2] = 0;

    if(DISP_OUTPUT_TYPE_LCD == display_source)   //TV���
    {
        ret = wBoot_driver_ioctl(ui_source.disp_hd, DISP_CMD_LCD_ON, 0, (void*)arg);
    }
    else if(DISP_OUTPUT_TYPE_TV == display_source)
    {
        if(DISP_TV_CVBS == tv_mode)
        {
            aux = DISP_TV_MOD_720P_50HZ;
        }
        else if(DISP_TV_YPBPR == tv_mode)
        {
            aux = DISP_TV_MOD_PAL;
        }
        else
        {
            return -1;
        }
        arg[0] = aux;
        wBoot_driver_ioctl(ui_source.disp_hd, DISP_CMD_TV_SET_MODE, 0, arg);
        ret = wBoot_driver_ioctl(ui_source.disp_hd, DISP_CMD_TV_ON, 0, 0);
    }
    else if(DISP_OUTPUT_TYPE_HDMI == display_source)
    {
    	arg[0] = display_mode;
        wBoot_driver_ioctl(ui_source.disp_hd, DISP_CMD_HDMI_SET_MODE, 0, arg);
        ret = wBoot_driver_ioctl(ui_source.disp_hd, DISP_CMD_HDMI_ON, 0, 0);;
    }
    else
    {
        return -1;
    }

    if(ret)
    {
        __inf("ERR: wBoot_driver_ioctl DISP_CMD_HDMI_ON failed\n");
    }

    return ret;
}

/*
*******************************************************************************
*                     De_CloseDevice
*
* Description:
*    �ر�TV
*
* Parameters:
*    void
*
* Return value:
*    0  :  �ɹ�
*   !0  :  ʧ��
*
* note:
*    void
*
*******************************************************************************
*/
__s32 De_CloseDevice(__s32 display_source)
{
    __s32 ret = 0;
    __u32 arg[3], cmd;
    __s32 high_flag;

    if(ui_source.disp_hd == NULL)
    {
        __inf("ERR: display driver not open yet\n");

        return -1;
    }
    arg[0] = 0;
    arg[1] = 0;
    arg[2] = 0;
    high_flag = (display_source >> 16) & 0xffff;

    if(DISP_OUTPUT_TYPE_LCD == high_flag)   //TV���
    {
        cmd = DISP_CMD_LCD_OFF;
    }
    else if(DISP_OUTPUT_TYPE_TV == high_flag)
    {
        cmd = DISP_CMD_TV_OFF;
    }
    else if(DISP_OUTPUT_TYPE_HDMI == high_flag)
    {
        cmd = DISP_CMD_HDMI_OFF;
    }
    else
    {
        return -1;
    }

    ret = wBoot_driver_ioctl(ui_source.disp_hd, cmd, 0, (void*)arg);
    if(ret)
    {
        __inf("ERR: display driver turn off display device failed\n");
    }

    return ret;
}
/*
*******************************************************************************
*                     De_IsLCDOpen
*
* Description:
*    LCD�Ƿ�� ��
*
* Parameters:
*    void
*
* Return value:
*    <0	:  �ɹ�
*    0 	:  LCD���ڳ�ʼ��
*    >0	:  ������ʧ��
*
* note:
*    void
*
*******************************************************************************
*/
__s32 De_IsLCDOpen(void)
{
    __s32 ret;
    __u32 arg[3];

    if(ui_source.disp_hd == NULL)
    {
        __inf("ERR: display driver not open yet\n");

        return -1;
    }

    arg[0] = 0;
    arg[1] = 0;
    arg[2] = 0;
    ret = wBoot_driver_ioctl(ui_source.disp_hd, DISP_CMD_LCD_CHECK_OPEN_FINISH, 0, (void*)arg);

    return ret;
}

/*
*******************************************************************************
*                     De_IsLCDClose
*
* Description:
*    LCD�Ƿ�ر� ��
*
* Parameters:
*    void
*
* Return value:
*    0  :  �ɹ�
*   !0  :  ʧ��
*
* note:
*    void
*
*******************************************************************************
*/
__s32 De_IsLCDClose(void)
{
    __s32 ret;
    __u32 arg[3];

    if(ui_source.disp_hd == NULL)
    {
        __inf("ERR: display driver not open yet\n");

        return -1;
    }

    arg[0] = 0;
    arg[1] = 0;
    arg[2] = 0;
    ret = wBoot_driver_ioctl(ui_source.disp_hd, DISP_CMD_LCD_CHECK_CLOSE_FINISH, 0, (void*)arg);
    if(ret)
    {
        __inf("ERR: LCD not close\n");
    }

    return ret;
}
/*
*******************************************************************************
*                     De_RequestLayer
*
* Description:
*    ����ͼ��
*
* Parameters:
*    layer_para  :  input.  ͼ�����
*    Layer_hd    :  output. ͼ����
*
* Return value:
*    0  :  �ɹ�
*   !0  :  ʧ��
*
* note:
*    void
*
*******************************************************************************
*/
__hdle De_RequestLayer(__disp_layer_work_mode_t mode)
{
    __hdle Layer_hd = NULL;
    __u32  arg[3];

    if(ui_source.disp_hd == NULL)
    {
        __inf("ERR: display driver not open yet\n");

        return (__hdle)0;
    }

    arg[0] = mode;
    arg[1] = 0;
    arg[2] = 0;
    Layer_hd = wBoot_driver_ioctl(ui_source.disp_hd, DISP_CMD_LAYER_REQUEST, 0, (void*)arg);
    if(Layer_hd == NULL)
    {
        __inf("ERR: wBoot_driver_ioctl DISP_CMD_LAYER_REQUEST failed\n");
    }

    return Layer_hd;
}


/*
*******************************************************************************
*                     De_ReleaseLayer
*
* Description:
*    �ͷ�ͼ��
*
* Parameters:
*    Layer_hd    :  input. ͼ����
*
* Return value:
*    0  :  �ɹ�
*   !0  :  ʧ��
*
* note:
*    void
*
*******************************************************************************
*/
__s32 De_ReleaseLayer(__s32 layer_hdl)
{
    __s32 ret = 0;
    __u32 arg[3];

    if(ui_source.disp_hd == NULL)
    {
        __inf("ERR: display driver not open yet\n");

        return -1;
    }

    arg[0] = (__u32)layer_hdl;
    arg[1] = 0;
    arg[2] = 0;
    ret = wBoot_driver_ioctl(ui_source.disp_hd, DISP_CMD_LAYER_RELEASE, 0, (void*)arg);
    if(ret)
    {
        __inf("ERR: wBoot_driver_ioctl DISP_CMD_LAYER_RELEASE failed\n");
    }

    return ret;
}


/*
*******************************************************************************
*                     De_OpenLayer
*
* Description:
*    ��ͼ��
*
* Parameters:
*    Layer_hd    :  input. ͼ����
*
* Return value:
*    0  :  �ɹ�
*   !0  :  ʧ��
*
* note:
*    void
*
*******************************************************************************
*/
__s32 De_OpenLayer(__s32 layer_hdl)
{
    __s32 ret = 0;
    __u32 arg[3];

    if(ui_source.disp_hd == NULL)
    {
        __inf("ERR: display driver not open yet\n");

        return -1;
    }

    arg[0] = (__u32)layer_hdl;
    arg[1] = 0;
    arg[2] = 0;
    ret = wBoot_driver_ioctl(ui_source.disp_hd, DISP_CMD_LAYER_OPEN, 0, (void*)arg);
    if(ret)
    {
        __inf("ERR: wBoot_driver_ioctl DISP_CMD_LAYER_OPEN failed\n");
    }

    return ret;
}


/*
*******************************************************************************
*                     De_CloseLayer
*
* Description:
*    �ر�ͼ��
*
* Parameters:
*    Layer_hd    :  input. ͼ����
*
* Return value:
*    0  :  �ɹ�
*   !0  :  ʧ��
*
* note:
*    void
*
*******************************************************************************
*/
__s32 De_CloseLayer(__s32 layer_hdl)
{
    __s32 ret = 0;
    __u32 arg[3];

    if(ui_source.disp_hd == NULL)
    {
        __inf("ERR: display driver not open yet\n");

        return -1;
    }

    arg[0] = (__u32)layer_hdl;
    arg[1] = 0;
    arg[2] = 0;
    ret = wBoot_driver_ioctl(ui_source.disp_hd, DISP_CMD_LAYER_CLOSE, 0, (void*)arg);
    if(ret)
    {
        __inf("ERR: wBoot_driver_ioctl DISP_CMD_LAYER_CLOSE failed\n");
    }

    return ret;
}

/*
*******************************************************************************
*                     De_GetFrameBuffer
*
* Description:
*    ��� FrameBuffer
*
* Parameters:
*    Layer_hd    :  input.  ͼ����
*    FrameBuffer :  output. ͼ�㻺����
*
* Return value:
*    0  :  �ɹ�
*   !0  :  ʧ��
*
* note:
*    void
*
*******************************************************************************
*/
__s32 De_GetFrameBuffer(__s32 layer_hdl, __disp_fb_t *FrameBuffer)
{
    __s32 ret = 0;
    __u32 arg[3];

    if(ui_source.disp_hd == NULL)
    {
        __inf("ERR: display driver not open yet\n");

        return -1;
    }

    arg[0] = (__u32)layer_hdl;
    arg[1] = (__u32)FrameBuffer;
    arg[2] = 0;
    ret = wBoot_driver_ioctl(ui_source.disp_hd, DISP_CMD_LAYER_GET_FB, 0, (void*)arg);
    if(ret)
    {
        __inf("ERR: wBoot_driver_ioctl DISP_CMD_LAYER_GET_FB failed\n");
    }

    return ret;
}

/*
*******************************************************************************
*                     De_SetFrameBuffer
*
* Description:
*    ����FrameBuffer
*
* Parameters:
*    Layer_hd    :  input.  ͼ����
*    FrameBuffer :  input.  ͼ�㻺����
*
* Return value:
*    0  :  �ɹ�
*   !0  :  ʧ��
*
* note:
*    void
*
*******************************************************************************
*/
__s32 De_SetFrameBuffer(__s32 layer_hdl, __disp_fb_t *FrameBuffer)
{
    __s32 ret = 0;
    __u32 arg[3];

    if(ui_source.disp_hd == NULL)
    {
        __inf("ERR: display driver not open yet\n");

        return -1;
    }

    arg[0] = (__u32)layer_hdl;
    arg[1] = (__u32)FrameBuffer;
    arg[2] = 0;

    ret = wBoot_driver_ioctl(ui_source.disp_hd, DISP_CMD_LAYER_SET_FB, 0, (void*)arg);
    if(ret)
    {
        __inf("ERR: wBoot_driver_ioctl DISP_CMD_LAYER_SET_FB failed\n");
    }

    return ret;
}

/*
*******************************************************************************
*                     De_GetSceenWidth
*
* Description:
*    �����Ļ�Ŀ��
*
* Parameters:
*    void
*
* Return value:
*    0  :  �ɹ�
*   !0  :  ʧ��
*
* note:
*    void
*
*******************************************************************************
*/
__u32 De_GetSceenWidth(void)
{
    __u32 arg[3];

    if(ui_source.disp_hd == NULL)
    {
        __inf("ERR: display driver not open yet\n");

        return 0;
    }

    arg[0] = 0;
    arg[1] = 0;
    arg[2] = 0;

    return wBoot_driver_ioctl(ui_source.disp_hd, DISP_CMD_SCN_GET_WIDTH, 0, (void*)arg);
}



/*
*******************************************************************************
*                     De_GetSceenWidth
*
* Description:
*    �����Ļ�ĸ߶�
*
* Parameters:
*    void
*
* Return value:
*    0  :  �ɹ�
*   !0  :  ʧ��
*
* note:
*    void
*
*******************************************************************************
*/
__u32 De_GetSceenHeight(void)
{
    __u32 arg[3];

    if(ui_source.disp_hd == NULL)
    {
        __inf("ERR: display driver not open yet\n");

        return 0;
    }

    arg[0] = 0;
    arg[1] = 0;
    arg[2] = 0;

    return wBoot_driver_ioctl(ui_source.disp_hd, DISP_CMD_SCN_GET_HEIGHT, 0, (void*)arg);
}

/*
*******************************************************************************
*                     De_GetLayerPara
*
* Description:
*    ���ͼ�����
*
* Parameters:
*    void
*
* Return value:
*    0  :  �ɹ�
*   !0  :  ʧ��
*
* note:
*    void
*
*******************************************************************************
*/
__s32 De_GetLayerPara(__s32 layer_hdl, __disp_layer_info_t * layer_para)
{
    __s32 ret = 0;
    __u32 arg[3];

    if(ui_source.disp_hd == NULL)
    {
        __inf("ERR: display driver not open yet\n");

        return -1;
    }

    arg[0] = (__u32)layer_hdl;
    arg[1] = (__u32)layer_para;
    arg[2] = 0;
    ret = wBoot_driver_ioctl(ui_source.disp_hd, DISP_CMD_LAYER_GET_PARA, 0, (void*)arg);

    if(ret)
    {
        __inf("ERR: wBoot_driver_ioctl DISP_CMD_LAYER_GET_PARA failed\n");
    }

    return ret;
}

/*
*******************************************************************************
*                     De_SetLayerPara
*
* Description:
*    ����ͼ�����
*
* Parameters:
*    void
*
* Return value:
*    0  :  �ɹ�
*   !0  :  ʧ��
*
* note:
*    void
*
*******************************************************************************
*/
__s32 De_SetLayerPara(__s32 layer_hdl, __disp_layer_info_t * layer_para)
{
    __s32 ret = 0;
    __u32 arg[3];

    if(ui_source.disp_hd == NULL)
    {
        __inf("ERR: display driver not open yet\n");

        return -1;
    }

    arg[0] = (__u32)layer_hdl;
    arg[1] = (__u32)layer_para;
    arg[2] = 0;

    ret = wBoot_driver_ioctl(ui_source.disp_hd, DISP_CMD_LAYER_SET_PARA, 0, (void*)arg);
    if(ret)
    {
        __inf("ERR: wBoot_driver_ioctl DISP_CMD_LAYER_SET_PARA failed\n");
    }

    return ret;
}

