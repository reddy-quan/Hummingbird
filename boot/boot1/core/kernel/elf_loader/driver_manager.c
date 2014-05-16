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
#include "mod_private.h"
#include "elf_parser.h"

eGon2_drv_manage_t      driver_manage = {0};
eGon2_mod_hd_t          driver_hd[EMOD_COUNT_MAX] = {0};
/*
*************************************************************************************************
*
*  ��������     _major_is_empty
*
*  ����  ��     ����
*
*  ����ֵ��     -1                        ����
*
*               0                         �����Ѿ�����
*
*               1                         ������û�а�װ
*
*  ˵����       ��鵱ǰ�����豸���Ƿ��Ѿ���ʹ�ã��Լ����豸���Ƿ���ȷ
*
*************************************************************************************************
*/
static __s32 _major_is_empty(__u32 mid)
{
    /* ���豸�ų�����32bit�����ݣ�����Խ����� */
	if(driver_manage.mid_bitmap & (1 << mid))                  //�Ѿ�����ʹ��
	{
		return 0;
	}
	else
	{
		return 1;
	}
}
/*
*************************************************************************************************
*
*  ��������     _major_set_empty
*
*  ����  ��     ����
*
*  ����ֵ��
*
*  ˵����       �����ǰ�����豸�Ŷ�Ӧ������
*
*************************************************************************************************
*/
static void _major_set_empty(__u32 mid)
{
    eGon2_drv_head_t *driver_reg = &driver_manage.drv_manage[mid];

	driver_manage.mid_bitmap &= ~(1 << mid);
	driver_reg->mid    = 0;
	driver_reg->init   = 0;
	driver_reg->exit   = 0;
	driver_reg->open   = 0;
	driver_reg->close  = 0;
	driver_reg->read   = 0;
	driver_reg->write  = 0;
	driver_reg->ioctl  = 0;
}
/*
*************************************************************************************************
*
*  ��������     _major_set_used
*
*  ����  ��     ����
*
*  ����ֵ��
*
*  ˵����       ���õ�ǰ�����豸�Ŷ�Ӧ������Ϊʹ��״̬
*
*************************************************************************************************
*/
static __s32 _major_set_used(__u32 mid)
{
	if(driver_manage.mid_bitmap & (1 << mid))                  //�Ѿ�����ʹ��
	{
		return -1;
	}
	else
	{
		driver_manage.mid_bitmap |= (1 << mid);

		return 1;
	}
}
/*
*************************************************************************************************
*
*  ��������     _ops_is_all_valid
*
*  ����  ��     ����
*
*  ����ֵ��
*
*  ˵����
*
*************************************************************************************************
*/
static __s32 _ops_is_all_valid(eGon2_mod_t * driver_input)
{
	return (    driver_input->demo_func.eGon2_open 	&& 	driver_input->demo_func.eGon2_close 	&&
			    driver_input->demo_func.eGon2_init 	&& 	driver_input->demo_func.eGon2_exit	    &&
			    driver_input->demo_func.eGon2_read	&&	driver_input->demo_func.eGon2_write	    &&
			    driver_input->demo_func.eGon2_ioctl);
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
__s32 eGon2_driver_reg(void *class_header)
{
	eGon2_mod_t         *driver_input     = NULL;
	eGon2_drv_head_t    *driver_reg       = NULL;
    __u32                mod_id           = 0;

    //�������һ����ָ�룬ֱ�ӷ��ش���
	if(class_header == NULL)
	{
		return -1;
	}

	driver_input = (eGon2_mod_t *)class_header;
	mod_id       = driver_input->mod_id;                    //��ȡģ��ID
	if(mod_id > EMOD_COUNT_MAX)                             //���ģ��IDԽ�磬�򷵻ش���
	{
	    eGon2_printf("driver id is too large\n");
		return  -1;
	}

    //���ģ��ID�Ƿ��Ѿ�����ʹ�ù�
    //�������ʹ�ù����򷵻ش���
	if(_major_is_empty( mod_id) <= 0)
	{
	    eGon2_printf("this driver is already installed\n");
		return  -1;
	}
    //û����ʹ�ù����������־λ����ʾ����ʹ��
	_major_set_used(mod_id);

	//�жϲ��������Ƿ�Ϸ�
	if(!_ops_is_all_valid(driver_input))
	{
	    eGon2_printf("some driver functions are not valid\n");
		return  -1;
	}
    //����ģ��ID����ȡ������������ĵ�ַ
	driver_reg = &(driver_manage.drv_manage[mod_id]);
	driver_reg->mid    = mod_id;
	driver_reg->init   = driver_input->demo_func.eGon2_init;
	driver_reg->exit   = driver_input->demo_func.eGon2_exit;
	driver_reg->open   = driver_input->demo_func.eGon2_open;
	driver_reg->close  = driver_input->demo_func.eGon2_close;
	driver_reg->read   = driver_input->demo_func.eGon2_read;
	driver_reg->write  = driver_input->demo_func.eGon2_write;
	driver_reg->ioctl  = driver_input->demo_func.eGon2_ioctl;
	driver_reg->standby= driver_input->demo_func.eGon2_standby;

    //����ע�ắ��
	driver_reg->init();

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
__s32 	eGon2_driver_unreg(__u32 mod_id)
{
    eGon2_drv_head_t *driver_reg = NULL;

    if(mod_id > EMOD_COUNT_MAX)
    {
		return -1;
	}
    //ȷ�ϸ�major��Ч
	if(_major_is_empty( mod_id) != 0)
	{
		return -1;
	}
    //�����û���ж�غ���
    if(!driver_hd[mod_id].usr_open)
    {
        //�����ǰ�û������˴򿪣��������û�ж������
        driver_reg = &(driver_manage.drv_manage[mod_id]);
        driver_reg->exit();
        //�������ʹ�ñ�־
        _major_set_empty(mod_id);

        return 0;
    }
    else
    {
        return -1;
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
__u32 eGon2_driver_open(__u32 mod_id, void *open_arg)
{
	eGon2_drv_head_t *driver_reg = NULL;

	//���Ƿ���Ч
	if(mod_id > EMOD_COUNT_MAX)
    {
		return 0;
	}
	if(_major_is_empty(mod_id) != 0)
	{
		return 0;
	}

	driver_reg = &(driver_manage.drv_manage[mod_id]);
	if(!driver_hd[mod_id].usr_open)                 //��û�д򿪹���ʱ������򿪣������ܴ�
	{
	    if(driver_reg->open)                        //���open������Ϊ�գ������ִ��open����
    	{
    	    //�����û���open����
    		driver_hd[mod_id].usr_hd = driver_reg->open(mod_id, open_arg);
    		//����û�open��������0����ʾopenʧ�ܣ�openʧ��ֱ�ӷ���0
    		if(driver_hd[mod_id].usr_hd == 0)
    		{
    		    return 0;
    		}
    		else
    		{
    		    //�ɹ�������£��û�open����1
    		    driver_hd[mod_id].mod_id = mod_id;
    		    driver_hd[mod_id].usr_open ++;

    		    return (__u32)&driver_hd[mod_id];
    		}
	    }
	    else
	    {
	        return 0;
	    }
	}
	else
	{
	    //���û��Ѿ��򿪹�����ֱ�ӷ��ؾ������ȥ�����û�����
	    driver_hd[mod_id].usr_open ++;

	    return (__u32)&driver_hd[mod_id];
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
__s32 eGon2_driver_close(__u32 hd)
{
    eGon2_drv_head_t *driver_reg = NULL;
    eGon2_mod_hd_t   *drv_hd     = (eGon2_mod_hd_t *)hd;
    __u32             mod_id     = drv_hd->mod_id;

	//���Ƿ���Ч
	if(mod_id > EMOD_COUNT_MAX)
    {
		return -1;
	}
	if(_major_is_empty(mod_id) != 0)
	{
		return -1;
	}
	driver_reg = &(driver_manage.drv_manage[mod_id]);

    //����û���open������Ϊ�գ�ͬʱ��������0�����ȥһ�δ���
	if((drv_hd->usr_open > 0) && (driver_reg->close))
	{
	    drv_hd->usr_open --;
	}
	else
	{
	    return -1;
	}
	if(!drv_hd->usr_open)                  //���������0��ʱ�򣬾���������������close����
	{
        //�����û���close����
        driver_reg->close(drv_hd->usr_hd);
	    drv_hd->usr_hd = 0;

	    return 0;
	}
	else
	{
	    //�������û�����������ֱ�ӷ��عرճɹ������ǲ�ȥ�����û�����
	    return 0;
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
__s32 eGon2_driver_read(void * pBuffer, __u32 blk, __u32 n, __u32 hd)
{
    eGon2_drv_head_t *driver_reg = NULL;
    eGon2_mod_hd_t   *drv_hd     = (eGon2_mod_hd_t *)hd;
    __u32             mod_id     = drv_hd->mod_id;

	//���Ƿ���Ч
	if(mod_id > EMOD_COUNT_MAX)
    {
		return -1;
	}
	if(_major_is_empty( mod_id ) != 0)
	{
		return -1;
	}
    driver_reg = &(driver_manage.drv_manage[mod_id]);
	if(drv_hd->usr_open)           //���û��Ĵ�������0��ʱ�򣬿��Դ�
	{
        //�����û���read����
        return driver_reg->read(pBuffer, blk, n, drv_hd->usr_hd);
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
__s32 eGon2_driver_write(void * pBuffer, __u32 blk, __u32 n, __u32 hd)
{
    eGon2_drv_head_t *driver_reg = NULL;
    eGon2_mod_hd_t   *drv_hd     = (eGon2_mod_hd_t *)hd;
    __u32             mod_id     = drv_hd->mod_id;

	//���Ƿ���Ч
	if(mod_id > EMOD_COUNT_MAX)
    {
		return -1;
	}
	if(_major_is_empty( mod_id ) != 0)
	{
		return -1;
	}
    driver_reg = &(driver_manage.drv_manage[mod_id]);
	if(drv_hd->usr_open)           //���û��Ĵ�������0��ʱ�򣬿��Դ�
	{
        //�����û���read����
        return driver_reg->write(pBuffer, blk, n, drv_hd->usr_hd);
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
__s32 eGon2_driver_ioctl(__u32 hd, __u32 cmd, __s32 aux, void *pBuffer)
{
    eGon2_drv_head_t *driver_reg = NULL;
    eGon2_mod_hd_t   *drv_hd     = (eGon2_mod_hd_t *)hd;
    __u32             mod_id     = drv_hd->mod_id;

	//���Ƿ���Ч
	if(_major_is_empty( mod_id ) != 0)
	{
		return -1;
	}
    driver_reg = &(driver_manage.drv_manage[mod_id]);
	if(drv_hd->usr_open)           //���û��Ĵ�������0��ʱ�򣬿��Դ�
	{
        //�����û���read����
        return driver_reg->ioctl(drv_hd->usr_hd, cmd, aux, pBuffer);
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
*    ˵��    ��
*
*
************************************************************************************************************
*/
__s32 eGon2_driver_standby(__u32 mod_id, __u32 cmd, void *pBuffer)
{
	eGon2_drv_head_t *driver_reg = NULL;

	//���Ƿ���Ч
	if(_major_is_empty( mod_id ) != 0)
	{
		return -1;
	}
	driver_reg = &(driver_manage.drv_manage[mod_id]);
	if(driver_reg->standby)
	{
		return driver_reg->standby(cmd, pBuffer);
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
*    ˵��    ��
*
*
************************************************************************************************************
*/
__s32 eGon2_driver_install(const char *pstr)
{
    void    *paddr;
    H_FILE  pfile;
    __s32   ret;
    __u32   length;

    pfile = FS_fopen(pstr, "r+");
    if(!pfile)
    {
        eGon2_printf("can't find %s\n", pstr);

        return -1;
    }
    //��ȡ�ļ�����
    length = FS_filelen(pfile);
    if(!length)
    {
        eGon2_printf("error: file %s length is 0\n", pstr);
        FS_fclose(pfile);

        return -1;
    }
    //�����ļ����Ȼ�ȡ�ڴ�
    paddr = eGon2_malloc(length);
    if(!paddr)
    {
        eGon2_printf("unable to malloc memory for install driver\n");
        FS_fclose(pfile);

        return -1;
    }
    if(!FS_fread(paddr, length, 1, pfile))
    {
        eGon2_printf("read %s fail\n", pstr);
        FS_fclose(pfile);
        eGon2_free(paddr);

        return -1;
    }

    FS_fclose(pfile);

    ret = elf_loader(paddr, 0);
    eGon2_free(paddr);

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
*    ����ֵ  ��
*
*    ˵��    ��
*
*
************************************************************************************************************
*/
typedef __s32 (* app_func)(__s32 argc, char *argv[]);
__s32 eGon2_run_app(__s32 argc, char **argv)
{
    void    *paddr;
    H_FILE  pfile;
    __u32   entry;
    app_func  func;
    __s32   ret;
    __u32   length;

    if(argc <= 0)
    {
        return -1;
    }
    //���ļ�
    pfile = FS_fopen(&argv[0][0], "r+");
    if(!pfile)
    {
        eGon2_printf("can't find %s\n", argv[0]);

        return -1;
    }
    //��ȡ�ļ�����
    length = FS_filelen(pfile);
    if(!length)
    {
        eGon2_printf("error: file %s length is 0\n", argv[0]);
        FS_fclose(pfile);

        return -1;
    }
    paddr = eGon2_malloc(length);
    if(!paddr)
    {
        eGon2_printf("unable to malloc memory for install driver\n");
        FS_fclose(pfile);

        return -1;
    }
    if(!FS_fread(paddr, length, 1, pfile))
    {
        eGon2_printf("read %s fail\n", argv[0]);
        FS_fclose(pfile);
        eGon2_free(paddr);

        return -1;
    }

    FS_fclose(pfile);

    ret = elf_loader(paddr, &entry);
    eGon2_free(paddr);
    if(ret < 0)
    {
        eGon2_printf("elf file %s load fail\n", argv[0]);
        return -1;
    }

    func = (app_func)entry;
    //ˢ��cache
	flush_icache();
	flush_dcache();

    func(argc, argv);

    return 0;
}

