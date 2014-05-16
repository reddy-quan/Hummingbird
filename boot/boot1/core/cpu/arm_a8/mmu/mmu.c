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
#ifndef  __mmu_c
#define  __mmu_c

#include "cpu_private.h"


extern void    flush_icache( void );
extern void    flush_dcache( void );
extern void    mmu_set_domain_access( void );
extern void    enable_prediction( void );


void mmu_system_init(__u32 dram_base, __u32 dram_size, __u32 mmu_base)
{
	__s32   *mmu_tlb_address = (__s32 *)mmu_base;
	__u32    i;

	//����16k�Ķα������СΪ1M
	for(i = 0; i < 4 * 1024; i++)
	{
		mmu_tlb_address[i] = ((i << 20))   |
							  (0 << 19)    |
							  (0 << 18)    |
							  (0 << 17)    |
							  (0 << 16)    |
							  (0 << 15)    |
							  (0 << 12)    |
							  (3 << 10)    |
							  (0 <<  9)    |
							  (15 << 5)    |
							  (0  << 4)    |
							  (0  << 3)    |
							  (0  << 2)    |
							  (2  << 0);
	}
	//cache sram
	mmu_tlb_address[0] = (0 << 20)    |         //��ַ
						 (0 << 19)    |         //��ȫ����
						 (0 << 18)    |         //1M�α�
						 (0 << 17)    |         //not global
						 (0 << 16)    |         //not shared
						 (0 << 15)    |         //
						 (0 << 12)    |         //
						 (3 << 10)    |         //����Ȩ�� ��Ȩ
						 (0 <<  9)    |         //
						 (15 << 5)    |         //�����
						 (0  << 4)    |         //
						 (1  << 3)    |         //cacheable
						 (0  << 2)    |         //nonbufferable
						 (2  << 0);             //�α�
	//cache dram
	for(i = 0; i < dram_size; i++)
	{
		mmu_tlb_address[i + (dram_base>>20)] = (dram_base + (i << 20))  |
									     (0 << 19)    |
										 (0 << 18)    |
										 (0 << 17)    |
										 (0 << 16)    |
										 (0 << 15)    |
										 (0 << 12)    |
										 (3 << 10)    |
									     (0 <<  9)    |
										 (15 << 5)    |
										 (0  << 4)    |
										 (1  << 3)    |
										 (1  << 2)    |
										 (2  << 0);
	}

	//set ttb
	mmu_flush_TLB();
	mmu_base |= (1 << 0) | (1 << 1) | (2 << 3);
	asm("mcr p15,0,%0,c2,c0,0"::"r"(mmu_base));
	asm("mcr p15,0,%0,c2,c0,1"::"r"(mmu_base));
	//__asm{mcr p15, 0, mmu_base, c2, c0, 0}
	//__asm{mcr p15, 0, mmu_base, c2, c0, 1}
	//clean i/d cache
	flush_icache();
	flush_dcache();
	//set domain controller
	mmu_set_domain_access();

	return ;
}

void   mmu_enable( void )
{
	__u32 c1format = 0;

	asm("mrc p15,0,%0,c1,c0,0":"=r"(c1format));
	//__asm{MRC p15,0,c1format,c1,c0,0}
	c1format &= ~ 0x1007;
	c1format |= 0x100d;				//��ICACHE��DCACHE, MMU��DISABLE ALIGIN CHECK
	asm("mcr p15,0,%0,c1,c0,0"::"r"(c1format));
	//__asm{MCR p15,0,c1format,c1,c0,0}

	//enable_prediction();
}



void   mmu_disable( void )
{
	__u32 c1format = 0;

	asm("mrc p15,0,%0,c1,c0,0":"=r"(c1format));
	//__asm{MRC p15,0,c1format,c1,c0,0}
	c1format &= ~ 0x1087;
	c1format |= 0;
	asm("mcr p15,0,%0,c1,c0,0"::"r"(c1format));
	//__asm{MCR p15,0,c1format,c1,c0,0}
}





#endif     //  ifndef __mmu_c

/* end of mmu.c */
