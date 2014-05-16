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
#include "../hdmi_hal.h"
#include "hdmi_interface.h"
#include "hdmi_core.h"

volatile __u32 HDMI_BASE = 0;

extern __s32            hdmi_state;
extern __bool           video_enable;
extern __s32            video_mode;
extern HDMI_AUDIO_INFO  audio_info;

void Hdmi_set_reg_base(__u32 base)
{
    HDMI_BASE = base;
}

__s32 Hdmi_hal_video_enable(__bool enable)
{
	if((video_enable != enable) && (hdmi_state >= HDMI_State_Video_config) )
	{
		hdmi_state 			= HDMI_State_Video_config;
	}
    video_enable = enable;
    
    return 0;
}

__s32 Hdmi_hal_set_display_mode(__u32 hdmi_mode)
{
	if(hdmi_mode != video_mode)
	{
		if(hdmi_state >= HDMI_State_Video_config)
		{
			hdmi_state = HDMI_State_Video_config;
		}
		video_mode = hdmi_mode;
	}
    return 0;
}


__s32 Hdmi_hal_audio_enable(__u8 mode, __u8 channel)
{
	/////////????????????????????????
	if(hdmi_state >= HDMI_State_Audio_config)
	{
		hdmi_state 			= HDMI_State_Audio_config;
	}
	
	audio_info.audio_en     = (channel == 0)?0:1;

    return 0;
}

#ifndef __BOOT_OSAL__
__s32 Hdmi_hal_set_audio_para(hdmi_audio_t * audio_para)
{
    if(!audio_para)
    {
        return -1;
    }

    if(audio_para->sample_rate != audio_info.sample_rate)
    {
    	if(hdmi_state >= HDMI_State_Audio_config)
    		hdmi_state 				= HDMI_State_Audio_config;
    	audio_info.sample_rate 	= audio_para->sample_rate;
    	//audio_info.channel_num  = 2;

    	__inf("sample_rate:%d in Hdmi_hal_set_audio_para\n", audio_info.sample_rate);
    }
    if(audio_para->channel_num != audio_info.channel_num)
    {
    	if(hdmi_state >= HDMI_State_Audio_config)
    		hdmi_state 				= HDMI_State_Audio_config;
    	audio_info.channel_num 	= audio_para->channel_num;

    	__inf("channel_num:%d in Hdmi_hal_set_audio_para\n", audio_info.channel_num);
    }

    return 0;
}
#endif

__s32 Hdmi_hal_mode_support(__u32 mode)
{
    if(Hpd_Check() == 0)
    {
        return 0;
    }
    else
    {
        while(hdmi_state < HDMI_State_Wait_Video_config)
        {
	        hdmi_delay_ms(1);
	    }
	    return Device_Support_VIC[mode];
	}
}
//0:rgb, 1:yuv
__s32 Hmdi_hal_get_input_csc(void)
{
    if(!YCbCr444_Support)
    	return 0;
    else
    	return 1;	
}
__s32 Hdmi_hal_get_HPD(void)
{
	return Hpd_Check();
}

__s32 Hdmi_hal_get_state(void)
{
    return hdmi_state;
}

__s32 Hdmi_hal_set_pll(__u32 pll, __u32 clk)
{
    hdmi_pll = pll;
    hdmi_clk = clk;
    return 0;
}

__s32 Hdmi_hal_main_task(void)
{
    hdmi_main_task_loop();
    return 0;
}

__s32 Hdmi_hal_init(void)
{	
    //hdmi_audio_t audio_para;
    
	hdmi_core_initial();
    audio_info.channel_num  = 2;
#ifdef __BOOT_OSAL__
{
    __u32 loop_count;
    
    loop_count = 3;
    while(loop_count--)
    {
        hdmi_main_task_loop();
    }
}
#endif
//for audio test
#if 0
    audio_para.ch0_en = 1;
    audio_para.sample_rate = 44100;
	Hdmi_hal_set_audio_para(&audio_para);

	Hdmi_hal_audio_enable(0, 1);
#endif

    return 0;
}

__s32 Hdmi_hal_exit(void)
{
    return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
__s32 Hdmi_hal_video_enable_sync(__bool enable)
{
    __u32 loop_count;
    
    if((video_enable != enable) && (hdmi_state >= HDMI_State_Video_config) )
	{
		hdmi_state 			= HDMI_State_Video_config;
	}
    video_enable = enable;

    loop_count = 3;
    while(loop_count--)
    {
        hdmi_main_task_loop();
        if(hdmi_state == HDMI_State_Playback)
        {
            return 0;
        }
        hdmi_delay_ms(1);
    }
    return -1;
}
