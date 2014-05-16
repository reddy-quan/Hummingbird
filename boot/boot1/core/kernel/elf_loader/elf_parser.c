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
#include "elf_parser.h"
#include "elf_private.h"
#include "mod_private.h"
#include "include.h"
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
static __s32 _is_spec_section(void *elf_img, struct __ELF32SHEAD *elf32shead, __u8 *section_name_section_base, eGon2_mod_t **priv_save)
//ȷ���Ƿ�Ϊ����ĶΣ���ȷ���������ͣ���һ������������Ӧ��
{
	eGon2_mod_t	*tmp_header = NULL;
	__u8 * section_name = NULL;
	*priv_save = NULL;

    //name��Աָ�������section�����֡�����ֵ��section��ͷ�ַ���section������
	section_name = (__u32)elf_img + section_name_section_base + elf32shead->name;

	//����ҵ���EGON2_MAGIC��
	if(!strcmp((void *)section_name, eGon2_HEADER_COMM_MAGIC))
	{
        //offset��������������ļ��е�ƫ��
        //��ˣ�tmp_headerָ���˸öε��׵�ַ
		tmp_header = (eGon2_mod_t *) ((__u32)elf_img + (__u32)(elf32shead->offset));
		//����Ȼ��privָ����Ǹö�(�Զ���εľ�������)����ELF�ļ��Ѿ�û��̫���ϵ
        *priv_save = tmp_header;
        if(tmp_header->magic[7] == 'p')
        {
            return eGON2_SUB_TYPE_APP;
        }
        else
        {
            return eGON2_SUB_TYPE_DRV;
        }
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
static __s32 _load_section(__u32 elf_type, void *elf_img, struct __ELF32SHEAD *elf32shead)
{

	__u32 copyclear = 0;

	//�����Σ��ж϶ε�����
	//SHT_PROGBITS: ��section���汻�������˵�һЩ��Ϣ�����ĸ�ʽ������ȡ���ڳ�����
	//SHT_NOBITS  : �����͵�section���ļ��в�ռ�ռ䣬��������SHT_PROGBITS�����ܸ�section�������ֽڣ�
	//              sh_offset��Ա�����˸����ϵ��ļ�ƫ����
	switch (elf32shead->type)
	{
		//����Σ�����RO,RW��
		case EELF_SHT_PROGBITS:
		{
		    //SHF_ALLOC : ��section�ڽ���ִ�й�����ռ�����ڴ�
			if(elf32shead->flags & EELF_SHF_ALLOC)
			{
			    //SHF_EXECINSTR : ��section�����˿�ִ�еĻ���ָ��
			    //SHF_WRITE     : ��section�������ڽ���ִ�й����пɱ�д������
            	if((elf32shead->flags & EELF_SHF_EXECINSTR) || (elf32shead->flags & EELF_SHF_WRITE))
        		{
            		copyclear = 1;
        		}
			}
			break;
		}
		//NOBITS��
        case EELF_SHT_NOBITS:
		{
		    //SHF_ALLOC : ��section�ڽ���ִ�й�����ռ�����ڴ�
        	if(elf32shead->flags & EELF_SHF_ALLOC)
			{
            	copyclear = 2;
        	}
			break;
		}
        default:
           return 0;
	}

	if(copyclear == 1)
	{
	    //��������������������Ӧ��ַ
   		memcpy((void *)(elf32shead->addr),(void *)((__u32)elf_img + (__u32)(elf32shead->offset)), elf32shead->size);

		return 1;
	}
	else if(copyclear == 2)
	{
	    //���ڴ����������
		memset((void *)(elf32shead->addr), 0, elf32shead->size);

		return 1;
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
__s32 elf_loader(void * elf_img, __u32 *entry)
{
	__u32   i = 0;
	__u8	sepc_section_index = 0;	//����ε�index
	struct  __ELF32HEAD 	_elf32head;
	struct  __ELF32SHEAD 	_elf32shead;
	struct  __ELF32HEAD     *elf32head  = &_elf32head;
	struct  __ELF32SHEAD    *elf32shead = &_elf32shead;
	         eGon2_mod_t     *priv = NULL;
	__u32	section_name_section_offset = 0;	//����sction����section ���ļ��е�λ��
	__u32                    elf_type;

	elf32head = (struct __ELF32HEAD  *)elf_img;
	elf_type = 0;

    //check it, �ж��Ƿ�Ϊ��Ч��elf�ļ�
    //elf�ļ���ͷ����4���̶��ֽڣ��ֱ���0x7f, 'E', 'L', 'F',�������������������������Ϊ����elf��ʽ���ļ�
	if(	(elf32head->ident[EI_MAG0] != ELFMAG0 ) ||
		(elf32head->ident[EI_MAG1] != ELFMAG1 ) ||
		(elf32head->ident[EI_MAG2] != ELFMAG2 ) ||
		(elf32head->ident[EI_MAG3] != ELFMAG3 ) )
	{
		*entry = 0;
        eGon2_printf("elf file header magic not match\n");

		return -1;
	}

	//������ڵ�ַ
	//entry��elf�ṩ�ĵ�һ��������Ƶ������ַ�������������̡������ļ�û����ι�������ڵ㣬�ó�Ա�ͱ���Ϊ0
	if((__u32)(elf32head->entry) < 0x40000000)
	{
	    eGon2_printf("elf file entry address not correct\n");

	    return -1;
	}

	//����section_name_section_offset
	//shoff������sectionͷ��section header table����elf�ļ��е�ƫ����(���ֽڼ���)
	//shstrndx�����Ÿ�section�����ַ��������ڵ�sectionͷ��(section header table)������
	//elf�����е�section�������һ��ÿ�����ǰ��չ̶������ݽṹ
	//��ˣ�elf32sheadָ����section�����ַ��������ڵ�sectionͷ��ĵ�ַ
	elf32shead = ( struct __ELF32SHEAD *)((__u32)elf_img + elf32head->shoff + (sizeof(struct __ELF32SHEAD))*(elf32head->shstrndx));
	//offset��Ա�����˸�section���ֽ�ƫ����(���ļ���ʼ����)
	//section_name_section_offset������section�����ַ������ļ��е�ƫ��
	section_name_section_offset = elf32shead->offset;

	//shnum��Ա�������ڳ���ͷ������ڵĸ���
	//���forѭ�����������еĳ���ͷ��
	for(i = 0; i < elf32head->shnum; i++)
	{
		__s32 temp = 0;

        //����elf32shead����ָ��ÿһ��section��ĵ�ַ
		elf32shead = ( struct __ELF32SHEAD *)((__u32)elf_img + elf32head->shoff + (sizeof(struct __ELF32SHEAD)) * i);
		temp = _is_spec_section(elf_img, elf32shead, (__u8 *)section_name_section_offset, &priv);

		if(temp == eGON2_SUB_TYPE_APP || temp == eGON2_SUB_TYPE_DRV)
		{
			elf_type = temp;
			sepc_section_index = i;

			break;
		}
	}
	if(elf_type == 0)
	{
	    eGon2_printf("elf file is not a app or driver\n");

		return -1;
	}

	//��fex֧�ֵ�elf�ļ�
	for(i = 0; i < elf32head->shnum; i++)
	{
	    //����elf32shead����ָ��ÿһ��section��ĵ�ַ
		elf32shead = ( struct __ELF32SHEAD *)((__u32)elf_img + elf32head->shoff + (sizeof(struct __ELF32SHEAD)) * i);

		//ֻҪ����ǰ���ҵ����ض���
		if(sepc_section_index != i)
		{
			//��ͨ����Ĵ���
			if(_load_section(elf_type, elf_img, elf32shead) < 0)
			{
				return -1;
			}
		}
	}

	//���������
	if(priv != NULL)
	{
		if(elf_type == eGON2_SUB_TYPE_DRV)
		{
			return eGon2_driver_reg(priv);
		}
		else if(elf_type == eGON2_SUB_TYPE_APP)
		{
			*entry = (__u32)priv->main;

			return 0;
		}
	}

    eGon2_printf("can't find elf file entry\n");

	return -1;
}

