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

#include  "include.h"
#include  "ui.h"
#include  "common_res.h"
#include  "../sfte/FontEngine.h"

display_info_set_t  display_info;

__u32  WORD_SIZE;
/*
*******************************************************************************
*                     WaitForDeInitFinish
*
* Description:
*   �ȴ�de��ʼ�����
*
* Parameters:
*   void
*
* Return value:
*    0  :  �ɹ�
*   !0  :  ʧ��
*
* note:
*   void
*
*******************************************************************************
*/
__s32 WaitForDeInitFinish(void)
{
	__s32 ret;
	__s32 timedly = 2000;
	__s32 check_time = timedly/50;

	do
	{
		ret = De_IsLCDOpen();
		if(ret == 1)
		{
			break;
		}
		else if(ret == -1)
		{
			return -1;
		}
		wBoot_timer_delay(50);
		check_time --;
		if(check_time <= 0)
		{
			return -1;
		}
	}
	while(1);

	return 0;

}
/*
*******************************************************************************
*                     ShowLayer
*
* Description:
*    ��ʾͼ��
*
* Parameters:
*    layer_hdl   :  input. ͼ����
*    layer_para  :  input. ͼ�����
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
__s32 ShowLayer(__hdle layer_hdl, display_layer_info_t *layer_para)
{
	__s32 ret = 0;

	WaitForDeInitFinish();
	ret = De_SetLayerPara(layer_hdl, layer_para);
	if(ret != 0)
	{
		DMSG_PANIC("ERR: De_SetLayerPara failed\n");
		return -1;
	}
	/* ��ʾ */
	De_OpenLayer(layer_hdl);

	return 0;
}
/*
*******************************************************************************
*                     ui_AllocLayerPara
*
* Description:
*    ��ʼ��һ��ͼ�����
*
* Parameters:
*    PictureInfo  :  input. ͼƬ��Ϣ
*
* Return value:
*    ����ͼ�����
*
* note:
*    void
*
*******************************************************************************
*/
display_layer_info_t *LCD_DisplayAllocLayerPara(void)
{
	display_layer_info_t *layer_para = NULL;
	__u32 screen_width = 0;
	__u32 screen_height = 0;
    __u32 buffer_size;

	layer_para = (display_layer_info_t *)wBoot_malloc(sizeof(display_layer_info_t));
	if(layer_para == NULL)
	{
		DMSG_PANIC("ERR: wBoot_malloc failed\n");
		return NULL;
	}
	memset(layer_para, 0, sizeof(display_layer_info_t));
    /* ��ʼ��ͼ�β��� */
	screen_width  = De_GetSceenWidth();
	screen_height = De_GetSceenHeight();
	//������Ļ�Ĵ�С������framebuffer�Ĵ�С
	display_info.screen_width  = screen_width;
	display_info.screen_height = screen_height;
	buffer_size                = screen_width * screen_height * 4 * 2;

    display_info.screen_buf = wBoot_malloc(buffer_size);
    if(!display_info.screen_buf)
    {
        __wrn("LCD_DisplayAllocLayerPara: malloc memory for display buffer failed\n");

        wBoot_free(layer_para);

        return NULL;
    }

    memset(display_info.screen_buf, 0, buffer_size);

    layer_para->fb.size.width 		= screen_width;
    layer_para->fb.size.height 		= screen_height;
    layer_para->fb.addr[0] 			= (__u32)display_info.screen_buf;
    layer_para->fb.mode = DISP_MOD_INTERLEAVED;
    layer_para->fb.format = DISP_FORMAT_ARGB8888;
    layer_para->fb.br_swap = 0;
    layer_para->fb.seq = DISP_SEQ_ARGB;
    layer_para->ck_enable        = 0;
	layer_para->mode           = DISP_LAYER_WORK_MODE_NORMAL;
	layer_para->alpha_en       = 0;
	layer_para->alpha_val      = 0xff;
	layer_para->pipe           = 0;
	layer_para->src_win.x      = 0;
	layer_para->src_win.y      = 0;
	layer_para->src_win.width  = screen_width;
	layer_para->src_win.height = screen_height;
	layer_para->scn_win.x      = 0;
	layer_para->scn_win.y      = 0;
	layer_para->scn_win.width  = screen_width;
	layer_para->scn_win.height = screen_height;

    display_info.crt_addr = display_info.screen_buf;

    display_info.total_height = ((buffer_size / 4) / (screen_width)) / (WORD_SIZE);   //�ܵĸ߶ȣ�������ʾ������

    display_info.rest_screen_height  = display_info.total_height - 1;          //��¼��Ļ��ʣ��߶ȣ�����, ʣ��1�в���
    display_info.rest_display_height = screen_height/WORD_SIZE - 1;    //��¼��ʾ��ʣ��߶ȣ�������ʣ��1�в���
    display_info.rest_screen_width   = screen_width;                        //ʣ���ȵ�����ʾ���, ���ص�λ
    display_info.current_height = 0;
    display_info.fb_addr        = display_info.screen_buf;
    display_info.x              = 0;
    display_info.y              = 0;

    return layer_para;
}
/*
*******************************************************************************
*                     ShowPicture
*
* Description:
*   ��ͼƬ�Ĵ洢·������ʾһ��ͼƬ
*
* Parameters:
*   Path    :  ��Ҫ��ʾ��ͼƬ·����ֻ֧��BMP��
*   picture_group : ͼƬ���������ͬʱ��8��
*   show_way:  ��ʾ��ʽ       ������������ʱ��֧��
*              Ҫ������ͼƬ�Ĵ�Сһ��
* Return value:
*    0  :  �ɹ�
*   !0  :  ʧ��
*
* note:
*   void
*
*******************************************************************************
*/
__s32 LCD_DisplayInit(void)
{
    __u32 	   screen_width = 0;
    char       font_name[16];

    display_layer_info_t *layer_para = NULL;
    /* ������ʼ�� */
    memset(&display_info, 0, sizeof(display_info_set_t));
    memset(font_name, 0, 16);
	//��ȡ�����С
	screen_width = De_GetSceenWidth();
	if(screen_width > 480)
	{
		WORD_SIZE = 32;
		strcpy(font_name, "c:\\font32.sft");
	}
	else
	{
		WORD_SIZE = 24;
		strcpy(font_name, "c:\\font24.sft");
    }
    //����һ��ͼ��������ʾ������ȡdisplay buffer�ĵ�ַ
    layer_para = LCD_DisplayAllocLayerPara();
    if(!layer_para)
    {
        return -1;
    }
    open_font(font_name, WORD_SIZE, display_info.screen_width,  display_info.screen_buf);   //���ֿ�
    /* ��ʾͼƬ */
	ShowLayer(board_res.layer_hd, layer_para);
    wBoot_timer_delay(50);

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
__s32 LCD_Change_frame_buffer(__u8 *addr)
{
    __disp_fb_t  fb_buffer;

    De_GetFrameBuffer(board_res.layer_hd, &fb_buffer);
    fb_buffer.addr[0] = (__u32)addr;
    De_SetFrameBuffer(board_res.layer_hd, &fb_buffer);

    return 0;
}





