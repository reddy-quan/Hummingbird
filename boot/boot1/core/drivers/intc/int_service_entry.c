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

extern void  open_sys_int( void );
extern void  close_sys_int( void );
extern __s32 timer0_int_func(void *arg);
extern __s32 timer1_int_func(void *arg);

typedef __s32  (* __int_func )(void* /*p_arg*/);     /* isr function pointer                                         */

static int esIRQHandler_default( void * pArg );

typedef struct
{
    void 	       *pArg;               //�жϺ�������
    __int_func      pIsr;               //�жϺ�����ַ

}__int_func_t;

__int_func_t  eGon2_IRQVectorTable[GIC_IRQ_NUM];
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
static void gic_distributor_init(void)
{
	__u32 cpumask = 0x01010101;
	__u32 gic_irqs;
	__u32 i;

	GICD_CTLR = 0;

	/* check GIC hardware configutation */
	gic_irqs = ((GICD_TYPE & 0x1f) + 1) * 32;
	if (gic_irqs > 1020)
	{
		gic_irqs = 1020;
	}
	else if (gic_irqs < GIC_IRQ_NUM)
	{
		eGon2_printf("GIC parameter config error, only support %d"
				" irqs < %d(spec define)!!\n", gic_irqs, GIC_IRQ_NUM);
		return ;
	}

	/* set trigger type to be level-triggered, active low */
	for (i=0; i<GIC_IRQ_NUM; i+=16)
	{
		GICD_ICFGR(i>>4) = 0;
	}
	/* set priority */
	for (i=GIC_SRC_SPI(0); i<GIC_IRQ_NUM; i+=4)
	{
		GICD_SPI_PRIO((i-32)>>2) = 0xa0a0a0a0;
	}
	/* set processor target to only cpu0 */
	for (i=32; i<GIC_IRQ_NUM; i+=4)
	{
		GICD_SPI_ITARG((i-32)>>2) = cpumask;
	}
	/* disable all interrupts */
	for (i=32; i<GIC_IRQ_NUM; i+=32)
	{
		GICD_ICENABLER(i>>5) = 0xffffffff;
	}
	/* clear all interrupt active state */
	for (i=32; i<GIC_IRQ_NUM; i+=32)
	{
		GICD_ICACTIVER(i>>5) = 0xffffffff;
	}
    //enable gic distributor
	GICD_CTLR = 1;

	return ;
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
static void gic_cpuif_init(void)
{
	__u32 i;

	GICC_CTLR = 0;
	/*
	 * Deal with the banked PPI and SGI interrupts - disable all
	 * PPI interrupts, ensure all SGI interrupts are enabled.
	*/
	GICD_ICENABLER(0) = 0xffff0000;
	GICD_ISENABLER(0) = 0x0000ffff;
	/* Set priority on PPI and SGI interrupts */
	for (i=0; i<16; i+=4)
	{
		GICD_SGI_PRIO(i>>2) = 0xa0a0a0a0;
	}
	for (i=16; i<32; i+=4)
	{
		GICD_PPI_PRIO((i-16)>>2) = 0xa0a0a0a0;
	}

	GICC_PMR  = 0xf0;
    //enable the gic cpu interface
	GICC_CTLR = 1;

	return ;
}
/*
*********************************************************************************************************
*										   IRQHandler_default
*
* Description: �����е�û�а�װ���������жϵ�Ĭ�ϴ�����, һ������˺�����ϵͳ�������κ����飬
*               �ȴ�watch dog reset system
* Arguments	 : void
* Returns	 : void
*********************************************************************************************************
*/
static int esIRQHandler_default(void * pArg)
{
	eGon2_printf("int not support\n");
    while(1);

	return 0;
}

/*
*********************************************************************************************************
*										   eGon2_Int_Init
*
* Description: ���жϽ��г�ʼ���������ر������ж�ʹ�ܣ���mask����������ж�pengding
*
* Arguments	 : void
* Returns	 : void
*********************************************************************************************************
*/
void eGon2_Int_Init(void)
{
	int i;

	for (i=0; i<GIC_IRQ_NUM; i++)
	{
		eGon2_IRQVectorTable[i].pIsr = esIRQHandler_default;
	}
	eGon2_IRQVectorTable[GIC_SRC_TIMER0].pIsr = timer0_int_func;
	eGon2_IRQVectorTable[GIC_SRC_TIMER1].pIsr = timer1_int_func;

	gic_distributor_init();
	gic_cpuif_init();

	return;
}
/*
*********************************************************************************************************
*										   eGon2_Int_Exit
*
* Description: �ж��˳�ʱ����еĲ����������ر������ж�ʹ�ܣ��ر�mask����������ж�pengding
*
* Arguments	 : void
* Returns	 : void
*********************************************************************************************************
*/
void eGon2_Int_Exit(void)
{
    return;
}
/*
*********************************************************************************************************
*										   eGon2_InsINT_Func
*
* Description:  Ϊ�ж�Դ���ö�Ӧ���жϷ������װ�ú���
*
* Arguments	 : irq_no     �жϺ�
*              finc_addr  �жϷ�����
* Returns	 : 0  ע��ɹ�
*              -1 ע��ʧ��
*********************************************************************************************************
*/
__s32 eGon2_InsINT_Func(__u32 irq_no, int *func_addr, void *p_arg)
{
    close_sys_int();
    if (irq_no < GIC_IRQ_NUM)
    {
		eGon2_IRQVectorTable[irq_no].pIsr = (__int_func )func_addr;
        eGon2_IRQVectorTable[irq_no].pArg = p_arg;
		open_sys_int();

		return 0;
	}
    open_sys_int();

    return -1;
}

/*
*********************************************************************************************************
*										   eGon2_UnsInt_Func
*
* Description:  Ϊ�ж�Դ���ö�Ӧ���жϷ������ж�غ���
*
* Arguments	 : irq_no     �жϺ�
*
* Returns	 : 0  ж�سɹ�
*              -1 ж��ʧ��
* Note       : û��ע������жϷ�����Ҳ���Խ���ж��?
*********************************************************************************************************
*/
__s32 eGon2_UnsInt_Func(__u32 irq_no)
{
    close_sys_int();
    if(irq_no < GIC_IRQ_NUM)
    {
    	if(eGon2_IRQVectorTable[irq_no].pIsr != esIRQHandler_default)
    	{
			eGon2_IRQVectorTable[irq_no].pIsr = (__int_func )esIRQHandler_default;
        	eGon2_IRQVectorTable[irq_no].pArg = 0;
			open_sys_int();
		}

		return 0;
	}
    open_sys_int();

    return -1;
}
/*
*********************************************************************************************************
*										   EnableInt
*
* Description:  ʹ���ж�
*
* Arguments	 : irq_no     �жϺ�
*
* Returns	 :
*
*********************************************************************************************************
*/
__s32 eGon2_EnableInt(__u32 irq_no)
{
	__u32 reg_val;
	__u32 offset;

	if (irq_no >= GIC_IRQ_NUM)
	{
		eGon2_printf("irq NO.(%d) > GIC_IRQ_NUM(%d) !!\n", irq_no, GIC_IRQ_NUM);
		return -1;
	}
	offset   = irq_no >> 5; // ��32
	reg_val  = GICD_ISENABLER(offset);
	reg_val |= 1 << (irq_no & 0x1f);
	GICD_ISENABLER(offset) = reg_val;

    return 0;
}

/*
*********************************************************************************************************
*										   DisableInt
*
* Description:  ��ֹ�ж�
*
* Arguments	 : irq_no     �жϺ�
*
* Returns	 :
*
*********************************************************************************************************
*/
__s32 eGon2_DisableInt(__u32 irq_no)
{
	__u32 reg_val;
	__u32 offset;

	if (irq_no >= GIC_IRQ_NUM)
	{
		eGon2_printf("irq NO.(%d) > GIC_IRQ_NUM(%d) !!\n", irq_no, GIC_IRQ_NUM);
		return -1;
	}

	offset   = irq_no >> 5; // ��32
	reg_val  = GICD_ISENABLER(offset);
	reg_val &= ~(1 << (irq_no & 0x1f));
	GICD_ISENABLER(offset) = reg_val;

    return 0;
}



void gic_sgi_handler(__u32 id)
{
	eGon2_printf("SGI irq %d coming... \n", id);
}

void gic_ppi_handler(__u32 id)
{
	eGon2_printf("PPI irq %d coming... \n", id);
}

void gic_spi_handler(__u32 id)
{
	if(eGon2_IRQVectorTable[id].pIsr != (__int_func )esIRQHandler_default)
	{
		eGon2_IRQVectorTable[id].pIsr(eGon2_IRQVectorTable[id].pArg);
	}

	return ;
}

void gic_clear_pending(u32 idnum)
{
	__u32 reg_val;
	__u32 offset;

	offset = idnum >> 5; // ��32
	reg_val = GICD_ICPENDR(offset);
	reg_val |= (1 << (idnum & 0x1f));
	GICD_ICPENDR(offset) = reg_val;

	return ;
}

void gic_irq_handler(void)
{
	u32 idnum;

	idnum = GICC_IAR;
	if (idnum == 1023)
	{
		eGon2_printf("spurious irq !!\n");
		return;
	}
	if (idnum >= GIC_IRQ_NUM)
	{
		eGon2_printf("irq NO.(%d) > GIC_IRQ_NUM(%d) !!\n", idnum, GIC_IRQ_NUM-32);
		return;
	}
	if (idnum < 16)
	{
		gic_sgi_handler(idnum);
	}
	else if (idnum < 32)
	{
		gic_ppi_handler(idnum);
	}
	else
	{
		gic_spi_handler(idnum);
	}

	GICC_EOIR = idnum;
	GICC_DIR  = idnum;

	gic_clear_pending(idnum);

	return ;
}


