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
#include <stdarg.h>
#include  "ui_main_i.h"
#include  "ui_char_i.h"
#include  "sfte/FontEngine.h"


_ui_char_info_t  ui_char_info;

static  __u32 change_line = 0;
/*
**********************************************************************************************************************
*                                               _change_to_new_line
*
* Description:
*
* Arguments  :
*
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
int boot_ui_char_init(int char_size)
{
	char  font_name[] = "c:\\font24.sft";

	memset(&ui_char_info, 0, sizeof(_ui_char_info_t));

	ui_char_info.word_size = 24;
	if(char_size == 32)
	{
		font_name[7] = '3';
    	font_name[8] = '2';
    	ui_char_info.word_size = 32;
	}
	else if(char_size == 24)
	{
		;
	}
	else
	{
		if(ui_source.screen_width < 400)
		{
			font_name[7] = '3';
    		font_name[8] = '2';
    		ui_char_info.word_size = 32;
		}
	}
    if(open_font((const char *)font_name, ui_char_info.word_size, ui_source.screen_width,  (__u8 *)ui_source.screen_buf))   //���ֿ�
    {
    	__inf("boot_ui_char: open font failed\n");
    	return -1;
    }

    ui_char_info.crt_addr 	  = ui_source.screen_buf;
    ui_char_info.total_height = ((ui_source.screen_size / 4) / (ui_source.screen_width)) / (ui_char_info.word_size);   //�ܵĸ߶ȣ�������ʾ������

    ui_char_info.rest_screen_height  = ui_source.screen_height/(ui_char_info.word_size);  	//��¼��Ļ��ʣ��߶ȣ�����, ʣ��1�в���
    ui_char_info.rest_display_height = ui_char_info.total_height;    						//��¼��ʾ��ʣ��߶ȣ�������ʣ��1�в���
    ui_char_info.rest_screen_width   = ui_source.screen_width;                        		//ʣ���ȵ�����ʾ���, ���ص�λ
    ui_char_info.current_height 	 = 0;
    ui_char_info.x              	 = 0;
    ui_char_info.y              	 = 0;

    return 0;
}
/*
**********************************************************************************************************************
*                                               _change_to_new_line
*
* Description:
*
* Arguments  :
*
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
static int uichar_change_newline(void)
{
	int ret = 0;

    if(ui_char_info.rest_display_height <= 0)               //ʣ���Ȳ����ˣ���Ҫ�л��ص���һ����Ļ
    {
		__inf("boot ui char: not enough space to printf\n");

		ret = -1;
	}
	else
	{
//	    __u32  tmp_size;

	    ui_char_info.rest_screen_width   = ui_source.screen_width;   //��Ϊ�µ�һ�еĳ���,���ص�λ

	    ui_char_info.rest_screen_height  -= 1;           //ʣ�����Ļ�߶�, ����
	    ui_char_info.rest_display_height -= 1;           //ʣ�����ʾ�߶ȣ�����
	    ui_char_info.current_height      += 1;           //��ǰ�߶ȱ��һ��
	    ui_char_info.x                    = 0;           //
	    ui_char_info.y                    = ui_char_info.current_height * ui_char_info.word_size;

//	    tmp_size = 4 * display_info.screen_width * display_info.y;
//        memset(display_info.screen_buf + tmp_size,  0,  4 * display_info.screen_width * WORD_SIZE);       //�������һ�е�����
//        display_info.crt_addr = display_info.screen_buf + tmp_size;                           //��ǰ��ʾ���ݵ�ַ�任
	}
//        tmp_size = (4 * display_info.screen_width) * (display_info.screen_height - WORD_SIZE);
//
//        display_info.rest_screen_height = display_info.total_height - (display_info.screen_height/WORD_SIZE);          //ʣ��߶�����
//        memcpy(display_info.screen_buf, display_info.fb_addr + 4 * display_info.screen_width * WORD_SIZE,  tmp_size);
//        display_info.crt_addr = display_info.screen_buf + tmp_size;
//        memset(display_info.screen_buf + tmp_size,  0,  4 * 2 * display_info.screen_width * WORD_SIZE);
//        display_info.current_height = display_info.screen_height/WORD_SIZE - 2;
//        display_info.rest_display_height  = 1;
//        display_info.x                    = 0;           //
//        display_info.y                    = display_info.current_height * WORD_SIZE;
//
//        display_info.fb_addr = display_info.screen_buf;    //��Ļ��ʾbuffer��ַ
//        LCD_Change_frame_buffer(display_info.fb_addr);
//    }
//    else if(display_info.rest_display_height <= 0)                         //��ǰ������ʾ���£���Ҫ���»���
//    {
//        tmp_size = 4 * display_info.screen_width * display_info.current_height * WORD_SIZE;
//
//        memset(display_info.screen_buf + tmp_size,  0,  8 * display_info.screen_width * WORD_SIZE);       //�������һ�е�����
//        display_info.crt_addr = display_info.screen_buf + tmp_size;                           //��ǰ��ʾ���ݵ�ַ�任
//        display_info.rest_display_height = 1;                                               //��¼��ʾ��ʣ��߶ȣ�����
//
//        display_info.fb_addr += 4 * display_info.screen_width * WORD_SIZE;                              //�л�����һ��
//        LCD_Change_frame_buffer(display_info.fb_addr);
//    }
//    else
//    {
//        tmp_size = 4 * display_info.screen_width * display_info.y;
//        memset(display_info.screen_buf + tmp_size,  0,  4 * display_info.screen_width * WORD_SIZE);       //�������һ�е�����
//        display_info.crt_addr = display_info.screen_buf + tmp_size;                           //��ǰ��ʾ���ݵ�ַ�任
//    }

    return ret;
}

/*
**********************************************************************************************************************
*                                               _debug_display_putchar
*
* Description:
*
* Arguments  :  ch         :  ��Ҫ��ӡ���ַ�
*               rest_width :  ��ǰ��ʣ��Ŀ��
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
static int uichar_putchar(__u8 ch)
{
    __s32 ret, width;

    ret = check_change_line(ui_char_info.x, ch);
    if(ret == -1)             //����ʧ�ܣ���ǰ�ַ�������
    {
        return 0;
    }
    else if(ret == 0)        //���ʳɹ�����ǰ�ַ��������ǲ���Ҫ����
    {
        ;
    }
    else if(ret == 1)        //���ʳɹ�����ǰ�ַ�������Ҫ����
    {
        if(uichar_change_newline( ))
        {
        	ret = -1;
        }
    }

    width = draw_bmp_ulc(ui_char_info.x, ui_char_info.y, ui_source.color);    //��ʾ�ַ�,���ص�ǰ��ʾ�ַ��Ŀ�ȣ����ص�λ
    ui_char_info.x += width;
    ui_char_info.rest_screen_width -= width;                        //��¼��ǰ��ʣ�������
    //���ô�ӡ����

	return ret;
}
/*
**********************************************************************************************************************
*                                               debug_display_putstr
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
static int uichar_putstr(const char * str, int length)
{
	int count = 0;

	while( *str != '\0' )
	{
		if(*str == '\n')
		{
            //��Ҫ���е�ʱ���Զ��л�����һ�п�ʼ������ʾ
            //���û��к���
            if(uichar_change_newline( ))
	        {
	        	return -1;
	        }
		}
		else
		{
			if(uichar_putchar(*str))
			{
				return -1;
			}
		}
		str++;
		count ++;
		if(count >= length)
		{
			if(uichar_change_newline( ))
	        {
	        	return -1;
	        }
		}
	}

	return 0;
}
/*
******************************************************************************************************************
*
*Function Name : UART_printf
*
*Description : This function is to formatedly output through UART, similar to ANSI C function printf().
*                This function can support and only support the following Conversion Specifiers:
*              %d		Signed decimal integer.
*              %u		Unsigned decimal integer.
*              %x		Unsigned hexadecimal integer, using hex digits 0f.
*              %X		Unsigned hexadecimal integer, using hex digits 0F.
*              %c		Single character.
*              %s		Character string.
*              %p		A pointer.
*
*Input : refer to ANSI C function printf().
*
*Output : void, different form ANSI C function printf().
*
*call for : void int_to_string_dec( __s32 input, char * str ), defined in format_transformed.c.
*           void int_to_string_hex( __s32 input, char * str );  defined in format_transformed.c.
*           void Uint_to_string_dec( __u32 input, char * str );  defined in format_transformed.c.
*           void UART_putchar( __s32 ch); defined in boot loader.
*           void UART_puts( const char * string ); defined in boot loader.
*
*Others : None at present.
*
*******************************************************************************************************************
*/
void  boot_ui_printf_null(const char * str, ...)
{
	return ;
}


void boot_ui_printf( const char * str, ...)
{
	int  base_color;
	char string[32];
	char *p;
	__s32 hex_flag ;
	va_list argp;

	va_start( argp, str );

	base_color = boot_ui_get_color();
	base_color &= 0xffffff;
	boot_ui_set_color(base_color);
    if(change_line == 1)
    {
        if(uichar_change_newline())
        {
        	__inf("boot ui char: unable to change to one new line\n");

        	goto boot_ui_print_err;
        }
        wBoot_timer_delay(10);
        change_line = 0;
    }

	while( *str )
	{
		if( *str == '%' )
		{
			++str;
			p = string;
			hex_flag = 'X';
			switch( *str )
			{
			    case 'u':
				case 'd':
				{
					int_to_string_dec( va_arg( argp,  __s32 ), string );
                    if(uichar_putstr( p, 32 ))
                    {
                      	goto boot_ui_print_err;
                    }
					++str;
					break;
				}
				case 'x': hex_flag = 'x';	         // jump to " case 'X' "
				case 'p':
				case 'X':
				{
					int_to_string_hex( va_arg( argp,  __s32 ), string, hex_flag );
					if(uichar_putstr( p , 32))
					{
						goto boot_ui_print_err;
					}
                    ++str;
					break;
				}
				case 'c':
				{
					if(uichar_putchar( va_arg( argp,  __s32 ) ))
					{
						goto boot_ui_print_err;
					}
					++str;
					break;
				}
				case 's':
				{
					if(uichar_putstr( va_arg( argp, char * ), 32 ))
					{
						goto boot_ui_print_err;
					}
					++str;
					break;
				}
				default :
				{
					if(uichar_putchar( '%' ))          // if current character is not Conversion Specifiers 'dxpXucs',
					{
						goto boot_ui_print_err;
					}
					if(uichar_putchar( *str ))         // output directly '%' and current character, and then
					{
						goto boot_ui_print_err;
					}
					++str;                        // let 'str' point to next character.
				}
			}
		}
		else
		{
			if( *str == '\n' )                      // if current character is '\n', insert and output '\r'
				change_line = 1;

			if(uichar_putchar( *str++ ))
			{
				goto boot_ui_print_err;
			}
		}
	}

	va_end( argp );

boot_ui_print_err:
	base_color |= 0xff000000;
	boot_ui_set_color(base_color);

	return;
}



