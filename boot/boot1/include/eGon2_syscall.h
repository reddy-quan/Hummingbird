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
#ifndef __SYSCALL_H_
#define __SYSCALL_H_	1

#include "eGon2_syscall_define.h"
//--------------------------------------------------------------------
//  ����������
//--------------------------------------------------------------------

//__swi (EGON2_SWI_FOR_TEST) 			    int 		swi_for_test          (void );                             //swi����֮��
//__swi (EGON2_SWI_GET_PARA)              int 		wBoot_get_para        (unsigned int  /* para_name */, void * /* para address */);  //���ݲ���
//__swi (EGON2_SWI_STANDBY)				int 		wBoot_standby		  (void);
//
//
//__swi (EGON2_SWI_DRV_REG)				int          wBoot_driver_install  (void *        /* class_header */                                                                  );
//__swi (EGON2_SWI_DRV_UNREG)			    int          wBoot_driver_uninstall(unsigned int  /* mod_id       */                                                                  );
//__swi (EGON2_SWI_DRV_OPEN)				unsigned int wBoot_driver_open     (unsigned int  /* mod_id  */, void * /*open_arg */                                                 );
//__swi (EGON2_SWI_DRV_CLOSE)			    int          wBoot_driver_close    (unsigned int  /* hd      */                                                                       );
//__swi (EGON2_SWI_DRV_READ)				int          wBoot_driver_read     (void *        /* pBuffer */, unsigned int /* blk */, unsigned int /*   n */, unsigned int /* hd */);
//__swi (EGON2_SWI_DRV_WRITE)			    int          wBoot_driver_write    (void *        /* pBuffer */, unsigned int /* blk */, unsigned int /*   n */, unsigned int /* hd */);
//__swi (EGON2_SWI_DRV_IOCTL)			    int          wBoot_driver_ioctl    (unsigned int  /* hd      */, unsigned int /* cmd */, int          /* aux */, void *  /* pBuffer */);
//
//__swi (EGON2_SWI_RUN_APP)               int          wBoot_run_app         (int           /* argc    */, char **      /* argv */                                              );
//
//__swi (EGON2_SWI_BLOCK_DEV_INIT)        int          wBoot_block_init      (void           /* arg     */                                                                       );
//__swi (EGON2_SWI_BLOCK_DEV_READ)        int          wBoot_block_read      (unsigned int  /* blk start */,unsigned int /* nblock */,                     void */* pBuffer */  );
//__swi (EGON2_SWI_BLOCK_DEV_WRITE)       int          wBoot_block_write     (unsigned int  /* blk start */,unsigned int /* nblock */,                     void */* pBuffer */  );
//__swi (EGON2_SWI_BLOCK_DEV_EXIT)        int          wBoot_block_exit      (void           /* arg     */                                                                       );
//
//__swi (EGON2_SWI_BLOCK_SIZE)			int			 wBoot_block_size	   (void);
//__swi (EGON2_SWI_BLOCK_DDEV_READ)       int          wBoot_block_dread     (unsigned int  /* blk start */,unsigned int /* nblock */,                     void */* pBuffer */  );
//__swi (EGON2_SWI_BLOCK_DDEV_WRITE)      int          wBoot_block_dwrite    (unsigned int  /* blk start */,unsigned int /* nblock */,                     void */* pBuffer */  );
///*
//     �жϲ��� ϵͳ����
//   1.ע���жϷ������
//   2.ע���жϷ������
//   3.ʹ���ж�
//   4.��ֹ�ж�
//*/
//__swi (EGON2_SWI_IRQ_REG)			    int wBoot_InsINT_Func     (unsigned int /* irq_no */, int * /* func_addr */, void * /* p_arg */);
//__swi (EGON2_SWI_IRQ_UNREG)		        int wBoot_UnsInt_Func     (unsigned int /* irq_no */                                          );
//__swi (EGON2_SWI_ENABLE_IRQ)            int wBoot_EnableInt       (unsigned int /* irq_no */                                          );
//__swi (EGON2_SWI_DISABLE_IRQ)           int wBoot_DisableInt      (unsigned int /* irq_no */                                          );
//
///*
//    �ļ����� ϵͳ����
//    �ļ�������ԱȽ϶࣬����Ч�ʵĿ��ǣ����ṩ��Ŀ¼������ֻ���ļ�����������ķ�����·��б�
//    ���еĲ������Ǳ�׼C�ӿ�
//    1.�ļ��򿪣���Ҫ����·������
//    2.�ļ���
//    3.�ļ�д
//    4.�ļ���λ���ļ�ָ������ļ�ͷ��ƫ��������TELL����
//    5.�ļ�����
//    6.�ļ�ָ��ƫ�ƣ���SEEK����
//    7.�ļ�������    ��eof����
//    8.�ļ��ر�
//    9.�ļ�ɾ������Ҫ����·��
//*/
//__swi (EGON2_SWI_FS_OPEN)               void *        wBoot_fopen (const char * /* filename */, char * /* openmode   */                                              );
//__swi (EGON2_SWI_FS_READ)               int           wBoot_fread (void * /* buffer   */, unsigned int /* block_size */, unsigned int /* count */, void * /* hfile */);
//__swi (EGON2_SWI_FS_WRITE)              int           wBoot_write (void * /* buffer   */, unsigned int /* block_size */, unsigned int /* count */, void * /* hfile */);
//__swi (EGON2_SWI_FS_TELL)               unsigned int  wBoot_ftell (void * /* hfile    */                                                                             );
//__swi (EGON2_SWI_FS_LEN)                unsigned int  wBoot_flen  (void * /* hfile    */                                                                             );
//__swi (EGON2_SWI_FS_SEEK)               int           wBoot_fseek (void * /* hfile    */, int          /* offset     */, int         /* whence */                    );
//__swi (EGON2_SWI_FS_EOF)                int           wBoot_feof  (void * /* hfile    */                                                                             );
//__swi (EGON2_SWI_FS_CLOSE)              int           wBoot_fclose(void * /* hfile    */                                                                             );
//__swi (EGON2_SWI_FS_RM)                 int           wBoot_rmfile(char * /* filename */                                                                             );
//
//__swi (EGON2_SWI_PART_INFO)             int           wBoot_part_info(const char * /* class_name */,          const char * /* name */, part_info_t * /* part_info */ );
//__swi (EGON2_SWI_PART_MOUNT  )          int           wBoot_fsmount  (char         /* disk       */                                                                  );
//__swi (EGON2_SWI_PART_UNMOUNT)          int           wBoot_fsunmount(char         /* disk       */                                                                  );
//__swi (EGON2_SWI_PART_COUNT)			int			  wBoot_part_count(int);
//__swi (EGON2_SWI_PART_START)            int			  wBoot_part_start(int         /* part index */);
//__swi (EGON2_SWI_PART_CAPACITY)			int			  wBoot_part_capacity(int      /* part index */);
///*
//    Ŀ¼���� ϵͳ����
//    �������ڱ�׼C�ӿ�
//    1.Ŀ¼�򿪲���
//    2.Ŀ¼�رղ���
//    3.Ŀ¼����������·��
//    4.Ŀ¼ɾ��������·��
//    5.��ȡһ��Ŀ¼
//*/
//__swi (EGON2_SWI_DIR_OPEN)              void *       wBoot_opendir(const char * /* dir name */);
//__swi (EGON2_SWI_DIR_CLOSE)             int          wBoot_closedir( void * /* dir handle */);
//__swi (EGON2_SWI_DIR_MAKE)              int          wBoot_mkdir(char * /* dir name */);
//__swi (EGON2_SWI_DIR_ERASE)             int          wBoot_rmdir(char * /* dir name */);
//__swi (EGON2_SWI_DIR_READ)              int          wBoot_readdir(void * /* dir handle */, ENTRY * /* dir struct */);
//
///*
//     �ڴ���� ϵͳ����
//    1.�����ڴ棬�ֽڵ�λ����ʼ����δ֪
//    2.�����ڴ棬�ֽڵ�λ����ʼ����ȫ��0
//    3.����ǰһ��������ڴ棬��β��׷���ڴ棬����δ֪
//    4.�ͷ�������ڴ�
//*/
//__swi(EGON2_SWI_MALLOC)                 void *wBoot_malloc          (unsigned int /* num_bytes */                              );
//__swi(EGON2_SWI_CALLOC)                 void *wBoot_calloc          (unsigned int /* size      */, unsigned int /* num_bytes */);
//__swi(EGON2_SWI_REALLOC)                void *wBoot_realloc         (void *       /* p         */, unsigned int /* num_bytes */);
//__swi(EGON2_SWI_FREE)                   void  wBoot_free            (void *       /* p         */                              );
//
//
///*
//    ���õ�ԴоƬ�������
//*/
//__swi(EGON2_SWI_POWER_GET_SOURCE)       int   wBoot_power_get_dcin_battery_exist(unsigned int * /*dcin_exist*/, unsigned int */*battery_exist*/);
//__swi(EGON2_SWI_POWER_GET_BAT_VOL)      int   wBoot_power_get_battery_vol       (unsigned int * /*battery_vol*/);
//__swi(EGON2_SWI_POWER_GET_KEY)          int   wBoot_power_get_key               (void);
//__swi(EGON2_SWI_POWER_CHECK_STARTUP)    int   wBoot_power_check_startup         (void);
//__swi(EGON2_SWI_POWER_SET_SW1)          int   wBoot_power_set_sw1               (unsigned int on_off);
//__swi(EGON2_SWI_POWER_SET_OFF)          int   wBoot_power_set_off               (void);
//__swi(EGON2_SWI_POWER_GET_LEVEL_STATUS) int   wBoot_power_get_level				(void);
//__swi(EGON2_SWI_POWER_SET_OFF_VOL)		int	  wBoot_power_set_off_vol			(void);
//__swi(EGON2_SWI_POWER_BATTERY_CAL)		int   wBoot_power_get_cal				(void);
//__swi(EGON2_SWI_POWER_CUR_LIMIT)		int	  wBoot_power_cur_limit				(int current);
//__swi(EGON2_SWI_POWER_VOL_LIMIT)		int   wBoot_power_vol_limit				(int vol);
//__swi(EGON2_SWI_POWER_TYPE)				int   wBoot_power_type					(void);
//
//__swi(EGON2_SWI_TWI_READ   )            int   wBoot_twi_read   (void  *      /* twi_arg   */);
//__swi(EGON2_SWI_TWI_WRITE  )            int   wBoot_twi_write  (void  *      /* twi_arg   */);
////__swi(EGON2_SWI_TWI_INIT   )            int   wBoot_twi_init   (unsigned int /* twi_index */, unsigned int /* clock  */);
////__swi(EGON2_SWI_TWI_EXIT   )            int   wBoot_twi_exit   (void                        );
////__swi(EGON2_SWI_TWI_SETFREQ)            int   wBoot_twi_setfreq(unsigned int /* twi run clock*/);
///*
//    ��ʱ��ʹ�ã�ϵͳ����
//    1.����һ����ʱ������Ҫ����һ��������Ϊ�ص�����ʱ���ж�������ʱ�򣬽������������
//    2.�ͷŶ�ʱ��
//    3.������ʱ������Ҫ����ϵͳ������Ķ�ʱ���������ʱʱ�䣬�Ƿ���Ҫ�Զ�����������ʱ��
//*/
//__swi(EGON2_SWI_TIMER_REQ)              unsigned int wBoot_timer_request(void *       /* func_addr */, void * /* p_arg */);
//__swi(EGON2_SWI_TIMER_REL)              int          wBoot_timer_release(unsigned int /* hd        */);
//__swi(EGON2_SWI_TIMER_START)            int          wBoot_timer_start  (unsigned int /* hd        */, int /* delay_time*/, int /* auto_restart */);
//__swi(EGON2_SWI_TIMER_STOP)             int          wBoot_timer_stop   (unsigned int /* hd        */);
//__swi(EGON2_SWI_SYSTEM_RESET)           void         wBoot_system_reset (void						 );
//__swi(EGON2_SWI_TIMER_DELAY)			void		 wBoot_timer_delay  (unsigned int /* delay  ms */);
///*
//     DMA���� ϵͳ����
//    1.����DMAͨ��
//    2.�ͷ�DMAͨ��
//    3.����DMA
//    4.ֹͣDMA
//    5.����DMA����
//*/
//__swi(EGON2_SWI_DMA_REQ)               unsigned int wBoot_dma_request  (unsigned int /* dmatype */                                );
//__swi(EGON2_SWI_DMA_REL)               int          wBoot_dma_release  (unsigned int /* hDma    */                                );
//__swi(EGON2_SWI_DMA_START)             int          wBoot_dma_start    (unsigned int /* hDMA    */, unsigned int /* saddr */, unsigned int /* daddr */, unsigned int /* bytes */);
//__swi(EGON2_SWI_DMA_STOP)              int          wBoot_dma_stop     (unsigned int /* hDMA    */                                );
//__swi(EGON2_SWI_DMA_SETTING)           unsigned int wBoot_dma_Setting  (unsigned int /* hDMA    */, void *       /* cfg    */     );
//__swi(EGON2_SWI_DMA_RESTART)           int          wBoot_dma_Restart  (unsigned int /* hDMA    */                                );
//__swi(EGON2_SWI_DMA_QUERYSTATE)        int          wBoot_dma_QueryState(unsigned int /* hDMA    */                               );
//__swi(EGON2_SWI_DMA_QUERYCHAN)         int          wBoot_dma_QueryChan(unsigned int /* hDMA    */                                );
//
///*
//    ��ӡ(����) ϵͳ����
//    1.uart printf   : PC����Ļ��ӡ����
//    2.display printf: LCD����Ļ��ӡ����
//*/
//__swi (EGON2_SWI_UART_DEBUG)			void         wBoot_uprintf      ( const char * /* str */                    );
//__swi (EGON2_SWI_UART_NTDEBUG)			void		 wBoot_ntprintf     ( const char * /* str */                    );
////__swi (EGON2_SWI_DISPLAY_DEBUG)		void wBoot_printf               ( const char * /* str */                    );
//__swi (EGON2_SWI_UART_GETC )            char         wBoot_getc         ( void                                      );
//__swi (EGON2_SWI_UART_GETS )            unsigned int wBoot_getall       ( char *       /* str */                    );
//__swi (EGON2_SWI_UART_PUTC )            void         wBoot_putc         ( char         /* str */                    );
//__swi (EGON2_SWI_UART_GETC_DELAY )      char         wBoot_getc_delay   ( unsigned int /* delay */					);
///*
//    ��ת���� ϵͳ����
//*/
//__swi (EGON2_SWI_JUMP_TO )              void 		 wBoot_jump_to      ( unsigned int  /* addr */                  );
//__swi (EGON2_SWI_JUMP_FEL)				void         wBoot_jump_fel     ( void										);
//__swi (EGON2_SWI_JUMP_TO_LINUX)			void		 wBoot_jump_to_linux( int addr,  int mod_id, unsigned int paddr, unsigned int kernal_addr);
///*
//	Ƶ�ʷ��� ϵͳ����
//*/
//__swi (EGON2_SWI_CLOCK )				unsigned int wBoot_get_clock    ( unsigned int  /* clock name */            );
///*
//	�ű����� ϵͳ����
//	1. ��ȡ���������ݣ�������ͨ���ݺ��ַ�������
//	2. ��ȡ�������µ��Ӽ�����
//	3. ��ȡ�����ĸ���
//	4. ��ȡ�������µ�GPIO����
//	5. ��ȡ�������µ�GPIO����
//*/
//__swi(EGON2_SWI_SCRIPT_FETCH)              int  wBoot_script_parser_fetch(char *main_name, char *sub_name, int value[], int count);
//__swi(EGON2_SWI_SCRIPT_PATCH)              int  wBoot_script_parser_patch(char *main_name, char *sub_name, int value);
//__swi(EGON2_SWI_SCRIPT_SUBKEY_COUNT)       int  wBoot_script_parser_subkey_count(char *main_name);
//__swi(EGON2_SWI_SCRIPT_MAINKEY_COUNT)      int  wBoot_script_parser_mainkey_count(void);
//__swi(EGON2_SWI_SCRIPT_MAINKEY_GPIO_COUNT) int  wBoot_script_parser_mainkey_get_gpio_count(char *main_name);
//__swi(EGON2_SWI_SCRIPT_MAINKEY_GPIO_DATA)  int  wBoot_script_parser_mainkey_get_gpio_cfg(char *main_name, void *gpio_cfg, int gpio_count);
///*
//    GPIO���� ϵͳ����
//*/
//__swi(EGON2_SWI_GPIO_REQUEST)        unsigned int wBoot_GPIO_Request				 (user_gpio_set_t *gpio_list, 	    unsigned int group_count_max			 );
//__swi(EGON2_SWI_GPIO_RELEASE)                 int wBoot_GPIO_Release				 (unsigned int p_handler, 	            		 int if_release_to_default_status);
//__swi(EGON2_SWI_GPIO_GET_ALL_PIN_STATUS)      int wBoot_GPIO_Get_All_PIN_Status	     (unsigned int p_handler, user_gpio_set_t *gpio_status, unsigned int gpio_count_max,  unsigned int if_get_from_hardware);
//__swi(EGON2_SWI_GPIO_GET_ONE_PIN_STATUS)      int wBoot_GPIO_Get_One_PIN_Status	     (unsigned int p_handler, user_gpio_set_t *gpio_status, 		 const char *gpio_name, unsigned int if_get_from_hardware);
//__swi(EGON2_SWI_GPIO_SET_ONE_PIN_STATUS)      int wBoot_GPIO_Set_One_PIN_Status	     (unsigned int p_handler, user_gpio_set_t *gpio_status, 	     const char *gpio_name, unsigned int if_set_to_current_input_status);
//__swi(EGON2_SWI_GPIO_SET_ONE_PIN_IO)          int wBoot_GPIO_Set_One_PIN_IO_Status   (unsigned int p_handler, unsigned int if_set_to_output_status, const char *gpio_name);
//__swi(EGON2_SWI_GPIO_SET_ONE_PIN_PULL)        int wBoot_GPIO_Set_One_PIN_Pull	     (unsigned int p_handler, unsigned int set_pull_status,   	     const char *gpio_name);
//__swi(EGON2_SWI_GPIO_SET_ONE_PIN_DLEVEL)      int wBoot_GPIO_Set_One_PIN_driver_level(unsigned int p_handler, unsigned int set_driver_level,  		 const char *gpio_name);
//__swi(EGON2_SWI_GPIO_SET_ONE_PIN_DATA)        int wBoot_GPIO_Write_One_PIN_Value	 (unsigned int p_handler, unsigned int value_to_gpio,     		 const char *gpio_name);
//__swi(EGON2_SWI_GPIO_GET_ONE_PIN_DATA)        int wBoot_GPIO_Read_One_PIN_Value	     (unsigned int p_handler, 								   		 const char *gpio_name);
///*
//    KEY������
//*/
//__swi(EGON2_SWI_KEY_GET_STATUS)               int wBoot_key_get_status               (void);
//__swi(EGON2_SWI_KEY_GET_VALUE)				  int wBoot_key_get_value				 (void);








#endif  /*#ifndef __SYSCALL_H_*/


