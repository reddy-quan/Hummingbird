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
#include  "boot_img.h"
#include  "BootOS_i.h"
#include  "common.h"
#include  "setup.h"
#include  "imgheader.h"
#include  "superblock.h"

#define    ANDROID_FASTBOOT_MODE		1
#define    ANDROID_RECOVERY_MODE		2
#define    ANDROID_AUTOMODIFY_MODE      3
#define    ANDROID_USB_RRECOVERY_MODE   4

extern  boot1_private_head_t  boot1_priv_para;

extern  int boot_recovery_status;

#ifdef CONFIG_LOAD_BOOTIMG
//********************************************************************************************************


/* borrowed from linux/include/linux/kernel.h */

#ifndef ALIGN
#define ALIGN(x, a)		__ALIGN_MASK(x, (typeof(x))(a)-1)
#define __ALIGN_MASK(x, mask)	(((x)+(mask))&~(mask))
#endif


int read_boot_img(__u32 boot_part_start, __u32 addr)
{

    unsigned kaddr, raddr,ksize_aligned,rsize_aligned,kstart_block,rstart_block;
    wBoot_block_read(boot_part_start,CFG_FASTBOOT_MKBOOTIMAGE_PAGE_SIZE>>9,(void *)addr);
    
	struct fastboot_boot_img_hdr *fb_hdr = (struct fastboot_boot_img_hdr *)addr;

    __debug("---------------------\n");
    __debug("page_size  : %x \n", fb_hdr->page_size);
    __debug("kernel size: %x \n", fb_hdr->kernel_size);
    __debug("kernel addr: %x \n", fb_hdr->kernel_addr);

    __debug("ramdisk size: %x \n", fb_hdr->ramdisk_size);
    __debug("ramdisk addr: %x \n", fb_hdr->ramdisk_addr);

    __debug("second size: %x \n", fb_hdr->second_size);
    __debug("second addr: %x \n", fb_hdr->second_addr);

    __debug("name: %s\n", fb_hdr->name);
    __debug("cmdline: %s\n", fb_hdr->cmdline);


    if (memcmp(fb_hdr->magic, FASTBOOT_BOOT_MAGIC, 8)) {
        __inf("boot1: bad boot image magic, maybe not a boot.img?\n");
        return -1;
    }

    kaddr = fb_hdr->kernel_addr;
    ksize_aligned = ALIGN(fb_hdr->kernel_size, fb_hdr->page_size);
    
    __debug(" ksize_aligned: %x \n",ksize_aligned);
    raddr = fb_hdr->ramdisk_addr;
    rsize_aligned = ALIGN(fb_hdr->ramdisk_size, fb_hdr->page_size);
    __debug(" rsize_aligned: %x \n",rsize_aligned);
    kstart_block = boot_part_start + ((fb_hdr->page_size)>>9);

    rstart_block = kstart_block+(ksize_aligned>>9);
    //read kernel from boot partition
    wBoot_block_read(kstart_block,ksize_aligned>>9,(void*)kaddr);
                           
    //read ramdisk from boot partition
    wBoot_block_read(rstart_block,rsize_aligned>>9,(void*)raddr);

    //mark that we have copy the kernel and ramdisk to the right memory location,
    //and avoid memmove operation in uboot.
    fb_hdr->unused[0] = 0x55;
    fb_hdr->unused[1] = 0xaa;
    return 0;
               


}
#endif
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
static __s32 check_key_to_recovery(void)
{
    __s32 ret1, ret2;
	__s32 key_high, key_low;
	__s32 keyvalue;

	if(boot_recovery_status == 1)
	{
        #ifdef CONFIG_AW_HOMELET_PRODUCT
        ret1 = wBoot_script_parser_fetch("target", "usb_recovery", (void *)&keyvalue, 1);
		if(ret1)
		{
	        return ANDROID_AUTOMODIFY_MODE;
		}else
		{
           return keyvalue==1 ? ANDROID_USB_RRECOVERY_MODE : ANDROID_AUTOMODIFY_MODE;
        }
        #else
		return ANDROID_RECOVERY_MODE;
        #endif
	}
	keyvalue = boot1_priv_para.uart_port;
	__inf("key value = %d\n", keyvalue);

    ret1 = wBoot_script_parser_fetch("recovery_key", "key_max", &key_high, 1);
	ret2 = wBoot_script_parser_fetch("recovery_key", "key_min", &key_low, 1);
	if(ret1)
	{
		__inf("unable to find recovery_key key_max value\n");
		return -1;
	}
	if(ret2)
	{
		__inf("unable to find recovery_key key_min value\n");
		return -1;
	}
	__inf("recovery key high %d, low %d\n", key_high, key_low);
	if((keyvalue >= key_low) && (keyvalue <= key_high))
	{
		__inf("key found, android recovery\n");
		return ANDROID_RECOVERY_MODE;
	}
    #if 0
    ret1 = wBoot_script_parser_fetch("fastboot_key", "key_max", &key_high, 1);
	ret2 = wBoot_script_parser_fetch("fastboot_key", "key_min", &key_low, 1);
	if(ret1)
	{
		__inf("unable to find fastboot_key key_max value\n");
		return -1;
	}
	if(ret2)
	{
		__inf("unable to find fastboot_key key_min value\n");
		return -1;
	}
	__inf("fastboot key high %d, low %d\n", key_high, key_low);
	if((keyvalue >= key_low) && (keyvalue <= key_high))
	{
		__inf("key found, android fastboot\n");
		return ANDROID_FASTBOOT_MODE;
	}

	__inf("key invalid\n");
    #endif
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
static __s32 boot_show_logo(char *logo_name, __u32 logo_show, __u32 logo_address)
{
    if((!logo_show ) || (!logo_name))
    {
        __inf("logo name is invalid or dont need show logo\n");

        return -1;
    }
    ShowPictureEx(logo_name, logo_address);
    __inf("show pic finish\n");

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
static __s32 boot_dsipatch_kernal(boot_sys_img_set_t  *os_img, __u32 *kernal_addr, __u32 *para_addr)
{
    H_FILE  hd_file = NULL;
    __s32   count;
    __s32   i, file_length = 0;
    void   *address;

	__inf("load kernel start\n");
    #ifndef   SPEED_UP_BOOT
    count = os_img->img_count;           //��ȡ����ĸ���
    for(i = 0; i < count; i++)
    {
        if(1 != i)
        {
        	address = (void *)os_img->img_set[i].img_base;
        }
        else
        {
        	address = wBoot_malloc(64 * 1024);
        }
        if(!address)
        {
        	__inf("img file %s base addres is NULL\n", (__u32)address);

        	return -1;
        }
        hd_file = wBoot_fopen(os_img->img_set[i].img_name, "rb");
        if(!hd_file)
        {
            __inf("open img file %s failed\n", os_img->img_set[i].img_name);

            goto dispath_error;
        }
        file_length = wBoot_flen(hd_file);
        if(file_length > os_img->img_set[i].img_size)
        {
            __inf("the img file %s length %d is larger than img max size %d\n", os_img->img_set[i].img_name, file_length, os_img->img_set[i].img_size);

            goto dispath_error;
        }
        wBoot_fread(address, 1, file_length, hd_file);
        wBoot_fclose(hd_file);

        os_img->img_set[i].img_full_size = file_length;
        if(1 == i)
        {
        	do_boot_linux((char *)os_img->img_set[1].img_base, (char *)address, file_length);
        	wBoot_free(address);
        }
        address = NULL;
        hd_file = NULL;
        file_length = 0;
    }
    address = (void *)os_img->script_info.img_base;
    if(!address)
    {
        __inf("no script could be filed\n");
    }
    else
    {
        memcpy(address, (void *)SCRIPT_BASE, os_img->script_info.img_size);
        os_img->script_info.img_full_size = os_img->script_info.img_size;
    }
    *kernal_addr = os_img->img_set[0].img_base;
    *para_addr 	 = os_img->img_set[1].img_base; 
#else
    *kernal_addr = 0x4a000000;
	hd_file = wBoot_fopen("c:\\linux\\u-boot.bin", "rb");
	if(!hd_file)
	{
    	__inf("open img file c:\\linux\\u-boot.bin failed\n");

    	goto dispath_error;
	}
	file_length = wBoot_flen(hd_file);
	wBoot_fread((void *)(*kernal_addr), 1, file_length, hd_file);
	wBoot_fclose(hd_file);
#endif
    __inf("load kernel successed\n");

    return 0;

dispath_error:
    if(hd_file)
    {
        wBoot_fclose(hd_file);
    }
    __inf("load kernel failed\n");

    return -1;
}
/*
*******************************************************************************
*                       PreBootOS
*
* Description:
*    �����������ϵͳ�����׼��������
*
* Parameters:
*    os_para:  input.  Boot�׶εĲ�����
*    Type  :  input.  ����ϵͳ����
*
* Return value:
*    ���سɹ�����ʧ�ܡ�
*
* note:
*    ��
*
*******************************************************************************
*/
__s32 PreBootOS(char *os_para, __u32 *kernal_addr, __u32 *para_addr, __s32 *logo_status)
{
   
    __s32  ret = -1;
    boot_sys_img_set_t  *os_img;
    
    #ifndef   SPEED_UP_BOOT
    //��ʼ�ű��������ű�������ʼ��
    os_img = (boot_sys_img_set_t *)os_para;
    //�˴����ӽӿڣ���ʾ����logo
    *logo_status = os_img->logo_off;
   // boot_show_logo(os_img->logo_name, os_img->logo_show, os_img->logo_address);
   boot_show_logo(os_img->logo_name, os_img->logo_show, BOOT_LOGO_MEM_BASE);
    //os_img->img_set = (char *)os_img + ((boot_sys_img_set_t *)0)->img_set;
    //������в���
    //���μ���OS�������ֵ��ڴ���
    //�ھ�������ϣ���ͬOSҲ����Ҫ�����������
    //�������е�OS����ͬ���Ĺ���ʼ���䵽�ڴ�
    #endif
    ret = boot_dsipatch_kernal(os_img, kernal_addr, para_addr);
    if(0 > ret)
    {
        goto PreBootOS_error;
    }
    //����ϵͳ����Ҫ�������Ѿ����ص��ڴ��У����ڸ��ݲ�ͬ��OS�������ԵĲ�������
    //�ҳ���Ҫ������OS
    __inf("start address = %x\n", *kernal_addr);

    return 0;

PreBootOS_error:

    return -1;
}
/*
*******************************************************************************
*                       BootOS_detect_os_type
*
* Description:
*
*
* Parameters:
*    Para  :
*
* Return value:
*    ��
*
* note:
*    ��
*
*******************************************************************************
*/
//��ʱ�����������û�����Ĳ�����������������һ��OS
int signature_verify(MBR *mbr);

__s32 BootOS_detect_os_type(__u32 *para_addr, __u32 *kernal_addr, void *os_info, int *logo_status)
{
    __s32  ret = -1;
    __s32  recovery = 0;
    boot_global_info_t  *global_info = (boot_global_info_t *)os_info;
    boot_sys_img_set_t   os_img_info;    
    char   buffer[MBR_SIZE];
    char   data[2048];
    MBR    *mbr;
    int    start = 0, i;
    bootloader_msg  *bt_msg;

    #ifdef CONFIG_LOAD_BOOTIMG
    __u32 boot_part_start;
    __u32 boot_part_length;
    #endif
    //��������ʾ�г����п��õ�OS���Ժ���display�滻
    //���ֻ��һ��OS����ֱ�ӽ����OS//
    #ifndef SPEED_UP_BOOT
    memset(&os_img_info, 0, sizeof(boot_sys_img_set_t));
    #endif
    //���� fast boot ����, ͨ����ϰ����ķ�ʽ
	recovery = check_key_to_recovery();
    #ifdef CONFIG_AW_HOMELET_PRODUCT
    if(recovery == ANDROID_AUTOMODIFY_MODE)
    {
    	sprite_led_init();
		ret = card_sprite();
		sprite_led_exit(ret);
		system_reset();
    }
    #endif
	//if(recovery > 0)
    {

        wBoot_block_read(0, MBR_SIZE>>9, buffer);
        mbr = (MBR *)buffer;
        for(i=0;i<mbr->PartCount;i++)
        {
            if(!strcmp("misc", (const char *)mbr->array[i].name))
            {
                start = mbr->array[i].addrlo;
                wBoot_block_read(start, 1, data);
                bt_msg = (bootloader_msg *)data;
                
				if(!strcmp(bt_msg->command, "efex"))
				{
					memset((char *)bt_msg->command, 0, 32);
					wBoot_block_write(start, 1, data);

					wBoot_jump_fel();
				}
                if(recovery > 0)
                {
				    memset((char *)bt_msg->command, 0, 32);
    				if(recovery == ANDROID_FASTBOOT_MODE)
    				{
    					strcpy((char *)bt_msg->command, "boot-fastboot");
    					__inf("fastboot mode\n");
    				}
    				else if(recovery == ANDROID_RECOVERY_MODE)
    				{
    					strcpy((char *)bt_msg->command, "boot-recovery");
    					__inf("recovery mode\n");
    				}
                    else if(recovery == ANDROID_USB_RRECOVERY_MODE)
    				{
    					strcpy((char *)bt_msg->command, "usb-recovery");
    					__inf("usb recovery mode\n");
    				}
    				wBoot_block_write(start, 1, data);
                }

				break;
            }

        }
    }
    

    if(signature_verify(mbr))
    {
    	__inf("signature verify failed\n");
        __u32 private_start;
        __u32 private_size;
        __u32 erase_block_once=16;
        char* private_data;
        int j;
    	//wBoot_power_set_off();
        for(j=0;j<mbr->PartCount;j++)
        {
        	if(!strcmp("private", (const char *)mbr->array[j].name))
            {
                private_start = mbr->array[j].addrlo;
                private_size  = mbr->array[j].lenlo ;
                __inf("private_stat =%d\n",private_start);
                __inf("private_size =%d\n",private_size);
                private_data = wBoot_malloc(erase_block_once<<9);
                memset(private_data,0xff,erase_block_once<<9);
                while(private_size>=erase_block_once)
                {
                     wBoot_block_write(private_start,erase_block_once,private_data);
                     private_start += erase_block_once;
                     private_size -= erase_block_once;
                }
                if(private_size)
                {
                    wBoot_block_write(private_start,private_size,private_data);
                }
                wBoot_free(private_data);
                __inf("erase  private done!\n");
               break;
                
            }
    	}

    }
#ifdef CONFIG_LOAD_BOOTIMG    
    for(i=0;i<mbr->PartCount;i++)
    {
        if(!strcmp("boot", (const char *)mbr->array[i].name))
        {
            boot_part_start  = mbr->array[i].addrlo;
            boot_part_length = mbr->array[i].lenlo;
            __debug("find boot partition!\n");
            __debug("boot_part_start=%x\n",boot_part_start);
            __debug("boot_part_length=%x\n",boot_part_length);
            break;
        }
 
    }
#endif

    #ifndef SPEED_UP_BOOT
    ret = script_patch("c:\\linux\\linux.ini", &os_img_info, 1);
    if(ret < 0)
    {
        __inf("NO OS to Boot\n");

        goto _boot_pre_err;
    }
    #endif
	//��ȡ��Ҫboot��ϵͳ�Ľű�
	#ifdef SPEED_UP_BOOT
    boot_show_logo("c:\\os_show\\bootlogo.bmp", 1, BOOT_LOGO_MEM_BASE);
    #endif

   
	ret = PreBootOS((char *)&os_img_info, kernal_addr, para_addr, logo_status);
	if(ret < 0)
	{
	    goto _boot_pre_err;
    }

    #ifdef CONFIG_LOAD_BOOTIMG
    __debug("begain read bootimg\n");
    ret=read_boot_img(boot_part_start,0x40007800);
    if(ret < 0)
	{
	    goto _boot_pre_err;
    }
    __debug("read bootimg done\n");
    #endif
    
    #ifdef SPEED_UP_BOOT
    #ifndef CONFIG_AW_HOMELET_PRODUCT
     WaitForDeInitFinish();
    #endif
    #endif
	//���￪ʼ��ת��������OS���
_boot_pre_err:
    #ifndef SPEED_UP_BOOT
    wBoot_free(global_info);
    #endif
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
void BootOS(__u32 para_addr, __u32 kernal_addr)
{
    __inf("jump to\n");
    wBoot_jump_to_linux(0, 3495, para_addr, kernal_addr);

    {
        __u32 delay = 0x55;

        while(delay == 0x55);
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
#define  HASH_BUFFER_BYTES                (32 * 1024)
#define  HASH_BUFFER_SECTORS              (HASH_BUFFER_BYTES/512)

#define  PARTITION_BOOT_VERIFY_OFFSET  (4 * 1024/512)
#define  PARTITION_BOOT_VERIFY_COUNT   (20)
#define  PARTITION_BOOT_VERIFY_STEP    (256 * 1024/512)

#define  PARTITION_SYSTEM_VERIFY_OFFSET  (5 * 1024 * 1024/512)
#define  PARTITION_SYSTEM_VERIFY_COUNT   (40)
#define  PARTITION_SYSTEM_VERIFY_STEP    (5 * 1024 * 1024/512)

int signature_verify(MBR *mbr)
{
	int i, j;
	unsigned int tmp_start;
	unsigned int summary1, summary2;
	unsigned int s_value[4], h_value[4];
	unsigned char buffer[HASH_BUFFER_BYTES];
	unsigned int read_bytes;

	memset(buffer, 0, HASH_BUFFER_BYTES);
	//����hashֵ
	prepareCryptTable();		//׼��hash��
	//��ȡǩ��
	__inf("ras init\n");
	rsa_init();
	__inf("ras start\n");
	for(i=0;i<mbr->PartCount;i++)
	{
		__inf("part %d name = %s\n", i, (const char *)mbr->array[i].name);
		if(!strcmp("boot", (const char *)mbr->array[i].name))
		{
			tmp_start = mbr->array[i].addrlo;

			__inf("find boot part\n");
			HashString_init();

			read_bytes = sizeof(struct fastboot_boot_img_hdr);
			wBoot_block_read(tmp_start, (read_bytes + 511)/512, buffer);
			summary1 = HashString(buffer, 1, read_bytes);	//1��hash

			read_bytes = sizeof(struct image_header);
			wBoot_block_read(tmp_start + CFG_FASTBOOT_MKBOOTIMAGE_SECTOR, (read_bytes + 511)/512, buffer);
			summary1 = HashString(buffer, 1, read_bytes);	//1��hash

			//��ȡ�����ǩ��
			wBoot_block_read(mbr->array[i].addrlo, 2, buffer);
			s_value[0] = *(unsigned int *)(buffer + 608);
			s_value[1] = *(unsigned int *)(buffer + 612);
			s_value[2] = *(unsigned int *)(buffer + 616);
			s_value[3] = *(unsigned int *)(buffer + 620);

			rsa_decrypt( s_value, 4, h_value );

			summary2 = (h_value[0]<<0) | (h_value[1]<<8) | (h_value[2]<<16) | (h_value[3]<<24);
#if 0
			for(j=0;j<4;j++)
			{
				__inf("s_value[%d] = %x\n", j, s_value[j]);
			}
			for(j=0;j<4;j++)
			{
				__inf("h_value[%d] = %x\n", j, h_value[j]);
			}
#endif
			__inf("summary by hash %x\n", summary1);
			__inf("summary by rsa %x\n", summary2);
			if(summary1 != summary2)
			{
				__inf("boot signature invalid\n");

				return -1;
			}
		}
		else if(!strcmp("system", (const char *)mbr->array[i].name))
		{
			struct ext4_super_block  *sblock;

			tmp_start = mbr->array[i].addrlo;

			__inf("find system part\n");
			HashString_init();

			read_bytes = sizeof(struct ext4_super_block);
			wBoot_block_read(tmp_start + CFG_SUPER_BLOCK_SECTOR, (read_bytes + 511)/512, buffer);
			sblock = (struct ext4_super_block *)buffer;
#if 0
			{
				int k;
				__inf("s_inodes_count        = %x\n", sblock->s_inodes_count);
				__inf("s_blocks_count_lo     = %x\n", sblock->s_blocks_count_lo);
				__inf("s_r_blocks_count_lo   = %x\n", sblock->s_r_blocks_count_lo);
				__inf("s_free_blocks_count_lo= %x\n", sblock->s_free_blocks_count_lo);
				__inf("s_free_inodes_count   = %x\n", sblock->s_free_inodes_count);
				__inf("s_first_data_block    = %x\n", sblock->s_first_data_block);
				__inf("s_log_block_size      = %x\n", sblock->s_log_block_size);
				__inf("s_log_cluster_size    = %x\n", sblock->s_log_cluster_size);
				__inf("s_blocks_per_group    = %x\n", sblock->s_blocks_per_group);
				__inf("s_clusters_per_group  = %x\n", sblock->s_clusters_per_group);
				__inf("s_inodes_per_group    = %x\n", sblock->s_inodes_per_group);
				__inf("s_mtime               = %x\n", sblock->s_mtime);
				__inf("s_wtime               = %x\n", sblock->s_wtime);
				__inf("s_mnt_count           = %x\n", sblock->s_mnt_count);
				__inf("s_max_mnt_count       = %x\n", sblock->s_max_mnt_count);
				__inf("s_magic               = %x\n", sblock->s_magic);
				__inf("s_state               = %x\n", sblock->s_state);
				__inf("s_errors              = %x\n", sblock->s_errors);
				__inf("s_minor_rev_level     = %x\n", sblock->s_minor_rev_level);
				__inf("s_lastcheck           = %x\n", sblock->s_lastcheck);
				__inf("s_checkinterval       = %x\n", sblock->s_checkinterval);
				__inf("s_creator_os          = %x\n", sblock->s_creator_os);
				__inf("s_rev_level           = %x\n", sblock->s_rev_level);
				__inf("s_def_resuid          = %x\n", sblock->s_def_resuid);
				__inf("s_def_resgid          = %x\n", sblock->s_def_resgid);

				__inf("s_first_ino           = %x\n", sblock->s_first_ino);
				__inf("s_inode_size          = %x\n", sblock->s_inode_size);
				__inf("s_block_group_nr      = %x\n", sblock->s_block_group_nr);
				__inf("s_feature_compat      = %x\n", sblock->s_feature_compat);
				__inf("s_feature_incompat    = %x\n", sblock->s_feature_incompat);
				__inf("s_feature_ro_compat   = %x\n", sblock->s_feature_ro_compat);
				for(k=0;k<16;k++)
				{
					__inf("s_uuid[%d]        = %x\n", k, sblock->s_uuid[k]);
				}
				for(k=0;k<16;k++)
				{
					__inf("s_volume_name[%d] = %x\n", k, sblock->s_volume_name[k]);
				}
				for(k=0;k<64;k++)
				{
					__inf("s_last_mounted[%d]= %x\n", k, sblock->s_last_mounted[k]);
				}
				__inf("s_algorithm_usage_bitmap= %x\n", sblock->s_algorithm_usage_bitmap);

				__inf("s_prealloc_blocks     = %x\n", sblock->s_prealloc_blocks);
				__inf("s_prealloc_dir_blocks = %x\n", sblock->s_prealloc_dir_blocks);
				__inf("s_reserved_gdt_blocks = %x\n", sblock->s_reserved_gdt_blocks);

				for(k=0;k<16;k++)
				{
					__inf("s_journal_uuid[%d]= %x\n", k, sblock->s_journal_uuid[k]);
				}
				__inf("s_journal_inum        = %x\n", sblock->s_journal_inum);
				__inf("s_journal_dev         = %x\n", sblock->s_journal_dev);
				__inf("s_last_orphan         = %x\n", sblock->s_last_orphan);
				for(k=0;k<16;k++)
				{
					__inf("s_hash_seed[%d]   = %x\n", k, sblock->s_hash_seed[k]);
				}
				__inf("s_def_hash_version    = %x\n", sblock->s_def_hash_version);
				__inf("s_jnl_backup_type     = %x\n", sblock->s_jnl_backup_type);
				__inf("s_desc_size           = %x\n", sblock->s_desc_size);
				__inf("s_default_mount_opts  = %x\n", sblock->s_default_mount_opts);
				__inf("s_first_meta_bg       = %x\n", sblock->s_first_meta_bg);
				__inf("s_mkfs_time           = %x\n", sblock->s_mkfs_time);
				for(k=0;k<17;k++)
				{
					__inf("s_jnl_blocks[%d]  = %x\n", k, sblock->s_jnl_blocks[k]);
				}

				__inf("s_blocks_count_hi     = %x\n", sblock->s_blocks_count_hi);
				__inf("s_r_blocks_count_hi   = %x\n", sblock->s_r_blocks_count_hi);
				__inf("s_free_blocks_count_hi= %x\n", sblock->s_free_blocks_count_hi);
				__inf("s_min_extra_isize     = %x\n", sblock->s_min_extra_isize);
				__inf("s_want_extra_isize    = %x\n", sblock->s_want_extra_isize);
				__inf("s_flags               = %x\n", sblock->s_flags);

				__inf("s_raid_stride         = %x\n", sblock->s_raid_stride);
				__inf("s_mmp_update_interval = %x\n", sblock->s_mmp_update_interval);
				__inf("s_mmp_block           = %x\n", sblock->s_mmp_block);
				__inf("s_raid_stripe_width   = %x\n", sblock->s_raid_stripe_width);
				__inf("s_log_groups_per_flex = %x\n", sblock->s_log_groups_per_flex);
				__inf("s_reserved_char_pad   = %x\n", sblock->s_reserved_char_pad);

				__inf("s_reserved_pad        = %x\n", sblock->s_reserved_pad);
				__inf("s_kbytes_written      = %x\n", sblock->s_kbytes_written);
				__inf("s_snapshot_inum       = %x\n", sblock->s_snapshot_inum);
				__inf("s_snapshot_id         = %x\n", sblock->s_snapshot_id);
				__inf("s_snapshot_r_blocks_count= %x\n", sblock->s_snapshot_r_blocks_count);
				__inf("s_snapshot_list       = %x\n", sblock->s_snapshot_list);
			}
#endif
			sblock->s_mtime     = CFG_SUPER_BLOCK_STAMP_VALUE;
			sblock->s_mnt_count = CFG_SUPER_BLOCK_STAMP_VALUE & 0xffff;
			memset(sblock->s_last_mounted, 0, 64);
			summary1 = HashString(buffer, 1, (unsigned int)&(((struct ext4_super_block *)0)->s_snapshot_list));	//1��hash

			//��ȡ�����ǩ��
			wBoot_block_read(mbr->array[i].addrlo, 2, buffer);
			s_value[0] = *(unsigned int *)(buffer + 1000 - 10 * 4 + 0);
			s_value[1] = *(unsigned int *)(buffer + 1000 - 10 * 4 + 4);
			s_value[2] = *(unsigned int *)(buffer + 1000 - 10 * 4 + 8);
			s_value[3] = *(unsigned int *)(buffer + 1000 - 10 * 4 + 12);

			rsa_decrypt( s_value, 4, h_value );
			summary2 = (h_value[0]<<0) | (h_value[1]<<8) | (h_value[2]<<16) | (h_value[3]<<24);
#if 0
			for(j=0;j<4;j++)
			{
				__inf("s_value[%d] = %x\n", j, s_value[j]);
			}
			for(j=0;j<4;j++)
			{
				__inf("h_value[%d] = %x\n", j, h_value[j]);
			}
#endif
			__inf("summary by hash %x\n", summary1);
			__inf("summary by rsa %x\n", summary2);
			if(summary1 != summary2)
			{
				__inf("system signature invalid\n");

				return -1;
			}

			break;
		}
	}

	return 0;
}

