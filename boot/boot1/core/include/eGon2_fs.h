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
#ifndef     __EGON2_FS__H__
#define     __EGON2_FS__H__             1

typedef struct FS_FORMAT_INFO
{
    int m_type;
	int s_type;
    int size;
}FS_FORMAT_INFO_t;

typedef struct FS_PART_OPTS
{
	int (*Write)(unsigned int nSectNum, unsigned int nSectorCnt, void * pBuf);
	int (*Read )(unsigned int nSectNum, unsigned int nSectorCnt, void * pBuf);
	int (*Init )(void);
	int (*Exit )(void);
}FS_PART_OPTS_t;


/**
**********************************************************************************************************************
*
* @fn - int FS_regpartopts(void)
*
* @brief - ע��������������������ļ�ϵͳ��ʼ��֮ǰ���
* @return - OK OR FAIL
**********************************************************************************************************************
*/
int FS_regpartopts(FS_PART_OPTS_t *pPartOpts);

/**
**********************************************************************************************************************
*
* @fn - int FSMount(void)
*
* @brief - ����flash���е�ϵͳ����
* @return - OK OR FAIL
**********************************************************************************************************************
*/
int FSMount(char disk);

/**
**********************************************************************************************************************
*
* @fn - int FSUnmount(void)
*
* @brief - ж��flash�̵�ϵͳ����
* @return - OK OR FAIL
**********************************************************************************************************************
*/
int FSUnmount(char disk);
/**
**********************************************************************************************************************
*
* @fn - HDIR FS_opendir(char * dir)
*
* @brief - �򿪾��񻺴��е�һ��Ŀ¼
* @param - dir : ������Ŀ¼��(�����Ǿ���·��)
* @return - NULL
*          > 0  - Ŀ¼���
**********************************************************************************************************************
*/
HDIR FS_opendir(const char * dir);

/**
**********************************************************************************************************************
*
* @fn - int  FS_readdir(HDIR hdir, ENTRY* ent)
*
* @brief - ��ȡ���񻺴��е�Ŀ¼��Ŀ¼��
* @param - hdir : Ŀ¼���
* @param - ent : input entry prt for store the result
* @return - OK or FAIL
**********************************************************************************************************************
*/
int  FS_readdir(HDIR hdir, ENTRY* ent);

/**
**********************************************************************************************************************
*
* @fn - int   FS_closedir(HDIR hdir)
*
* @brief - �رվ��񻺴��е�Ŀ¼��Ŀ¼��
* @param - hdir : Ŀ¼���
* @return - OK or FAIL
**********************************************************************************************************************
*/
int   FS_closedir(HDIR hdir);

/**
**********************************************************************************************************************
*
* @fn - int 	FS_mkdir(char * dir)
*
* @brief - �ھ��񻺴��д���һ��Ŀ¼������ͬ����dir�����˳�
* @param - dir : ������Ŀ¼��(�����Ǿ���·��)
* @return - OK or FAIL
**********************************************************************************************************************
*/
int 	FS_mkdir(char * dir);

/**
**********************************************************************************************************************
*
* @fn - int 	FS_rmdir(char * dir)
*
* @brief - �ھ��񻺴���ɾ��һ��Ŀ¼��Ŀ¼����Ϊ���Ҵ��ڣ����򱨴�
* @param - dir : ������Ŀ¼��(�����Ǿ���·��)
* @return - OK or FAIL
**********************************************************************************************************************
*/
int 	FS_rmdir(char * dir);

/**
**********************************************************************************************************************
*
* @fn - H_FILE FS_fopen(char * filename, char * mode)
*
* @brief - �ھ��񻺴��� �� or ����һ���ļ�
* @param - filename : �������ļ���(�����Ǿ���·��)
* @param - mode : ģʽ��ͬ��׼fopen������
* @return - NULL ʧ��
*           >  0 �ļ����
**********************************************************************************************************************
*/
H_FILE FS_fopen(const char * filename, char * mode);

/**
**********************************************************************************************************************
*
* @fn - int FS_fclose(H_FILE hfile)
*
* @brief - �ھ��񻺴��йر�һ���ļ�
* @param - filename : �������ļ���(�����Ǿ���·��)
* @return - OK or FAIL
**********************************************************************************************************************
*/
int FS_fclose(H_FILE hfile);

/**
**********************************************************************************************************************
*
* @fn - int FS_fread(void * buffer, unsigned int block_size, unsigned int count, H_FILE hfile)
*
* @brief - ��ȡ���񻺴���һ���ļ�������
* @param - hfile : �򿪵��ļ����
* @param - buffer : ���buffer
* @param - block_size : ��ȡ�����ĵ�Ŀ��С
* @param - count : ��ȡ�����ĵ�Ŀ��
* @return - ��ȡ�����ĵ�Ŀ��Ŀ
**********************************************************************************************************************
*/
int FS_fread(void * buffer, unsigned int block_size, unsigned int count, H_FILE hfile);

/**
**********************************************************************************************************************
*
* @fn - int FS_fwrite(void * buffer, unsigned int block_size, unsigned int count, H_FILE hfile)
*
* @brief - д���񻺴���һ���ļ�
* @param - hfile : �򿪵��ļ����
* @param - buffer : ����Դbuffer
* @param - block_size : д�����ĵ�Ŀ��С
* @param - count : д�����ĵ�Ŀ��
* @return - �ɹ�д��ĵ�Ŀ��Ŀ
**********************************************************************************************************************
*/
int FS_fwrite(void * buffer, unsigned int block_size, unsigned int count, H_FILE hfile);

/**
**********************************************************************************************************************
*
* @fn - int FS_rmfile(char * filename)
*
* @brief - �ھ��񻺴���ɾ�� һ���ļ�
* @param - filename : �������ļ���(�����Ǿ���·��)
* @return - OK or FAIL
**********************************************************************************************************************
*/
int FS_rmfile(const char * filename);

/**
**********************************************************************************************************************
*
* @fn - int FS_fseek(H_FILE hfile, int offset, int whence)
*
* @brief - seek
* @param - hfile : �򿪵��ļ����
* @param - offset :
* @param - whence :
* @return - OK or FAIL
**********************************************************************************************************************
*/
int FS_fseek(H_FILE hfile, int offset, int whence);

/**
**********************************************************************************************************************
*
* @fn - int 	FS_feof(H_FILE hfile)
*
* @brief - test eof
* @param - hfile : �򿪵��ļ����
* @return - 1 : eof
*           0 : not eof
**********************************************************************************************************************
*/
int 	FS_feof(H_FILE hfile);

//PLUGINAPI FSTAT*  FS_fstat(H_FILE hfile);

/**
**********************************************************************************************************************
*
* @fn - int 	FS_filelen(H_FILE hfile)
*
* @brief - filelen
* @param - hfile : �򿪵��ļ����
* @return - file length
*
**********************************************************************************************************************
*/
unsigned int 	FS_filelen(H_FILE hfile);

/**
**********************************************************************************************************************
*
* @fn - int 	FS_ftell(H_FILE hfile)
*
* @brief - ftell
* @param - hfile : �򿪵��ļ����
* @return - file length
*
**********************************************************************************************************************
*/
unsigned int 	FS_ftell(H_FILE hfile);
/**
**********************************************************************************************************************
*
* @fn - int 	FS_getpart_info(const char *class_name, const char *name, void *part_info)
*
* @brief - ftell
* @param - hfile : class_name: ����ƥ��Ĵ�����
*                  name      : ����ƥ���������
*                  part_info : ���ڻ�ȡ���������ݽṹ��ַ
* @return - 0�� �ɹ�
*           -1: ʧ��
*
**********************************************************************************************************************
*/
int FS_getpart_info(const char *class_name, const char *name, part_info_t *part_info);
int FS_getpart_count(int part_attribute);
int FS_getpart_start(int part_index);
int FS_getpart_capacity(int part_index);
/**
**********************************************************************************************************************
*
* @fn - int 	FS_init(void)
*
* @brief - test eof
* @param - hfile : ��ʼ���ļ�ϵͳ���ڲ�����
* @return - 1 : eof
*           0 : not eof
**********************************************************************************************************************
*/
int 	FS_init(void);

/**
**********************************************************************************************************************
*
* @fn - int 	FS_exit(void)
*
* @brief - test eof
* @param - hfile :
* @return - 1 : eof
*           0 : not eof
**********************************************************************************************************************
*/
int 	FS_exit(void);

#endif  /* __EGON2_FS__H__ */


