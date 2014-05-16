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
#include <string.h>
#include <stdarg.h>
#include  "include.h"
#include  "ui.h"
#include  "format/format.h"
#include  "../sfte/FontEngine.h"


extern  display_info_set_t  display_info;


static  __u32 change_line = 0;
static  __u32 color = 0x00ff00;            //Ĭ����ɫ

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
void  _change_to_new_line(void)
{
    __u32  tmp_size;

    wlibc_int_disable();
    display_info.rest_screen_width   = display_info.screen_width;   //��Ϊ�µ�һ�еĳ���,���ص�λ

    display_info.rest_screen_height  -= 1;           //ʣ�����Ļ�߶�, ����
    display_info.rest_display_height -= 1;           //ʣ�����ʾ�߶ȣ�����
    display_info.current_height      += 1;           //��ǰ�߶ȱ��һ��
    display_info.x                    = 0;           //
    display_info.y                    = display_info.current_height * WORD_SIZE;

    if(display_info.rest_display_height <= 0)               //ʣ���Ȳ����ˣ���Ҫ�л��ص���һ����Ļ
    {
        tmp_size = (4 * display_info.screen_width) * (display_info.screen_height - WORD_SIZE);

        display_info.rest_screen_height = display_info.total_height - (display_info.screen_height/WORD_SIZE);          //ʣ��߶�����
        memcpy(display_info.screen_buf, display_info.fb_addr + 4 * display_info.screen_width * WORD_SIZE,  tmp_size);
        display_info.crt_addr = display_info.screen_buf + tmp_size;
        memset(display_info.screen_buf + tmp_size,  0,  4 * 2 * display_info.screen_width * WORD_SIZE);
        display_info.current_height = display_info.screen_height/WORD_SIZE - 2;
        display_info.rest_display_height  = 1;
        display_info.x                    = 0;           //
        display_info.y                    = display_info.current_height * WORD_SIZE;

        display_info.fb_addr = display_info.screen_buf;    //��Ļ��ʾbuffer��ַ
        LCD_Change_frame_buffer(display_info.fb_addr);
    }
    else if(display_info.rest_display_height <= 0)                         //��ǰ������ʾ���£���Ҫ���»���
    {
        tmp_size = 4 * display_info.screen_width * display_info.current_height * WORD_SIZE;

        memset(display_info.screen_buf + tmp_size,  0,  8 * display_info.screen_width * WORD_SIZE);       //�������һ�е�����
        display_info.crt_addr = display_info.screen_buf + tmp_size;                           //��ǰ��ʾ���ݵ�ַ�任
        display_info.rest_display_height = 1;                                               //��¼��ʾ��ʣ��߶ȣ�����

        display_info.fb_addr += 4 * display_info.screen_width * WORD_SIZE;                              //�л�����һ��
        LCD_Change_frame_buffer(display_info.fb_addr);
    }
    else
    {
        tmp_size = 4 * display_info.screen_width * display_info.y;
        memset(display_info.screen_buf + tmp_size,  0,  4 * display_info.screen_width * WORD_SIZE);       //�������һ�е�����
        display_info.crt_addr = display_info.screen_buf + tmp_size;                           //��ǰ��ʾ���ݵ�ַ�任
    }

    wlibc_int_enable();

    return;
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
__s32 _debug_display_putchar(__u8 ch)
{
    __s32 ret, width;

    wlibc_int_disable();

    ret = check_change_line(display_info.x, ch);
    if(ret == -1)             //����ʧ�ܣ���ǰ�ַ�������
    {
        wlibc_int_enable();
        return -1;
    }
    else if(ret == 0)        //���ʳɹ�����ǰ�ַ��������ǲ���Ҫ����
    {
        ;
    }
    else if(ret == 1)        //���ʳɹ�����ǰ�ַ�������Ҫ����
    {
        _change_to_new_line( );
    }

    width = draw_bmp_ulc(display_info.x, display_info.y, color);    //��ʾ�ַ�,���ص�ǰ��ʾ�ַ��Ŀ�ȣ����ص�λ
    display_info.x += width;
    display_info.rest_screen_width -= width;                        //��¼��ǰ��ʣ�������
    wlibc_int_enable();
    //���ô�ӡ����

	return 0;
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
void debug_display_putstr(const char * str)
{
	while( *str != '\0' )
	{
		if(*str == '\n')
		{
            //��Ҫ���е�ʱ���Զ��л�����һ�п�ʼ������ʾ
            //���û��к���
            __wrn("here\n");
            _change_to_new_line();
		}
		else
		{
			_debug_display_putchar(*str);
		}
		str++;
	}
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
__s32 debug_setdisplay_clear(void)
{
    memset((__u8 *)display_info.screen_buf, 0x0, display_info.screen_width * display_info.screen_height * 4 * 2);//ȫ����ʼ���ɱ���ɫ(0x0 ��ɫ)
    display_info.crt_addr = display_info.screen_buf;

    display_info.rest_screen_height  = display_info.total_height - 1;            //��¼��Ļ��ʣ��߶ȣ�����, ʣ��1�в���
    display_info.rest_display_height = display_info.screen_height/WORD_SIZE - 1; //��¼��ʾ��ʣ��߶ȣ�������ʣ��1�в���
    display_info.rest_screen_width   = display_info.screen_width;                //ʣ���ȵ�����ʾ���, ���ص�λ
    display_info.current_height = 0;
    display_info.fb_addr        = display_info.screen_buf;
    display_info.x              = 0;
    display_info.y              = 0;

    change_line    = 0;
    wBoot_timer_delay( 20 );

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
__s32 debug_setdisplay_color(__u32 user_set_color)
{
    color = user_set_color;

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
void debug_printf( const char * str, ...)
{
	char string[13];
	char *p;
	__s32 hex_flag ;
	va_list argp;

	va_start( argp, str );

    if(change_line == 1)
    {
        _change_to_new_line();
        wBoot_timer_delay(30);
        change_line = 0;
    }

	while( *str )
	{
		if( *str == '%' )
		{
			++str;
			p = string;
			hex_flag = HEX_X;
			switch( *str )
			{
			    case 'u':
				case 'd': int_to_string_dec( va_arg( argp,  __s32 ), string );
                          debug_display_putstr( p );
						  ++str;
						  break;
				case 'x': hex_flag = HEX_x;	         // jump to " case 'X' "
				case 'p':
				case 'X': int_to_string_hex( va_arg( argp,  __s32 ), string, hex_flag );
						  debug_display_putstr( p );
                          ++str;
						  break;
				case 'c': _debug_display_putchar( va_arg( argp,  __s32 ) );
						  ++str;
						  break;
				case 's': debug_display_putstr( va_arg( argp, char * ) );
						  ++str;
						  break;
				case 'l':
					{
						__u32 high;
						__u32 low;

						if( str[1] == 'l' && ( str[2] == 'x' || str[2] == 'X' ) )
						{
							low = va_arg( argp, __u32 );
							high = va_arg( argp, __u32 );

							if( str[2] == 'x' )
								hex_flag = 'x';
							else
								hex_flag = 'X';
							int_to_string_hex( high, string, hex_flag );
							debug_display_putstr( p );
							int_to_string_hex( low, string, hex_flag );
							debug_display_putstr( p+2 );
							str += 3;
						  	break;
						}
						else
						{
							int_to_string_dec( va_arg( argp,  __s32 ), string );
                          	debug_display_putstr( p );
						  	++str;
						  	break;
						}
					}
				default : _debug_display_putchar( '%' );          // if current character is not Conversion Specifiers 'dxpXucs',
						  _debug_display_putchar( *str );         // output directly '%' and current character, and then
						  ++str;                        // let 'str' point to next character.
			}
		}

		else
		{
			if( *str == '\n' )                      // if current character is '\n', insert and output '\r'
				change_line = 1;
				//_change_to_new_line();

			_debug_display_putchar( *str++ );
		}
	}

	va_end( argp );
}



