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
#ifndef	_WBOOT_MBR_H_
#define	_WBOOT_MBR_H_


#define     DOWNLOAD_MAP_NAME   "dlinfo.fex"
#define     SUNXI_MBR_NAME      "sunxi_mbr.fex"
/* MBR       */
#define     MBR_SIZE			    (16 * 1024)
#define     DL_SIZE					(16 * 1024)
#define   	MBR_MAGIC			    "softw411"
#define     MBR_MAX_PART_COUNT		120
#define     MBR_COPY_NUM          	4    //mbr�ı�������
#define     MBR_RESERVED          	(MBR_SIZE - 32 - (MBR_MAX_PART_COUNT * sizeof(PARTITION)))   //mbr�����Ŀռ�
#define     DL_RESERVED           	(DL_SIZE - 32 - (MBR_MAX_PART_COUNT * sizeof(dl_one_part_info)))

/* partition information */
typedef struct sunxi_partition_t
{
	unsigned  int       addrhi;				//��ʼ��ַ, ������Ϊ��λ
	unsigned  int       addrlo;				//
	unsigned  int       lenhi;				//����
	unsigned  int       lenlo;				//
	unsigned  char      classname[16];		//���豸��
	unsigned  char      name[16];			//���豸��
	unsigned  int       user_type;          //�û�����
	unsigned  int       keydata;            //�ؼ����ݣ�Ҫ����������ʧ
	unsigned  int       ro;                 //��д����
	unsigned  char      reserved[68];		//�������ݣ�ƥ�������Ϣ128�ֽ�
}__attribute__ ((packed))PARTITION;

/* mbr information */
typedef struct sunxi_mbr
{
	unsigned  int       crc32;				        // crc 1k - 4
	unsigned  int       version;			        // �汾��Ϣ�� 0x00000100
	unsigned  char 	    magic[8];			        //"softw411"
	unsigned  int 	    copy;				        //����
	unsigned  int 	    index;				        //�ڼ���MBR����
	unsigned  int       PartCount;			        //��������
	unsigned  int       stamp[1];					//����
	PARTITION           array[MBR_MAX_PART_COUNT];	//
	unsigned  char      res[MBR_RESERVED];
}__attribute__ ((packed)) MBR;

typedef struct tag_one_part_info
{
	unsigned  char      name[16];           //����д���������豸��
	unsigned  int       addrhi;             //����д�����ĸߵ�ַ��������λ
	unsigned  int       addrlo;             //����д�����ĵ͵�ַ��������λ
	unsigned  int       lenhi;				//����д�����ĳ��ȣ���32λ��������λ
	unsigned  int       lenlo;				//����д�����ĳ��ȣ���32λ��������λ
	unsigned  char      dl_filename[16];    //����д�������ļ����ƣ����ȹ̶�16�ֽ�
	unsigned  char      vf_filename[16];    //����д������У���ļ����ƣ����ȹ̶�16�ֽ�
	unsigned  int       encrypt;            //����д�����������Ƿ���м��� 0:����   1��������
	unsigned  int       verify;             //����д�����������Ƿ����У�� 0:��У�� 1��У��
}__attribute__ ((packed)) dl_one_part_info;

//������д��Ϣ
typedef struct tag_download_info
{
	unsigned  int       crc32;				        		        //crc
	unsigned  int       version;                                    //�汾��  0x00000101
	unsigned  char 	    magic[8];			        		        //"softw311"
	unsigned  int       download_count;             		        //��Ҫ��д�ķ�������
	unsigned  int       stamp[3];									//����
	dl_one_part_info	one_part_info[MBR_MAX_PART_COUNT];	//��д��������Ϣ
	unsigned  char      res[DL_RESERVED];
}
__attribute__ ((packed)) download_info;


//��׼������Ϣ
#pragma pack(push, 1)
typedef struct tag_part_info_stand
{
	char 				indicator;			//��ʾ�÷����Ƿ��ǻ����
	char				start_head;			//������ʼ�Ĵ�ͷ
	short				start_sector:6;		//������ʼ������
	short				start_cylinder:10;	//������ʼ������
	char				part_type;			//��������			00H:û��ָ��  01H:DOS12	02H:xenix	04H:DOS16 05H:��չ����	06H:FAT16
											//					07H:NTFS	  0BH:FAT32
	char				end_head;			//���������Ĵ�ͷ
	short				end_sector:6;		//��������������
	short				end_cylinder:10;	//��������������
	//int					start_sectors;		//��ʼ������ǰ��˵��������Ӳ�̵ĸ��������������߼�����
	//int					total_sectors;		//�����е���������
	short				start_sectorl;
	short				start_sectorh;
	short				total_sectorsl;
	short				total_sectorsh;
}
part_info_stand;
#pragma pack(pop)
//��׼MBR
#pragma pack(push, 1)
typedef struct tag_mbr_stand
{
	char				mbr[0x89];			//��������¼
	char				err_info[0x135];	//������Ϣ
	part_info_stand		part_info[4];		//��������
	short				end_flag;			//�̶�ֵ 0x55aa
}
mbr_stand;
#pragma pack(pop)


//��̬���ݴ����ʽ
typedef struct tag_dynamic_data_form
{
	char  name[16];
	int   datasize;
	int   usedsize;
}
dynamic_data_form;

typedef struct tag_dynamic_boot_head
{
	char  magic[8];
	int   count;
	int   size;
}
dynamic_boot_head;

#endif	//_WBOOT_MBR_H_

/* end of _WBOOT_MBR_H_ */


