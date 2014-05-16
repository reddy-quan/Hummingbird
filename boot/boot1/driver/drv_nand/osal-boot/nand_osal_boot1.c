/*
* (C) Copyright 2007-2012
* Allwinner Technology Co., Ltd. <www.allwinnertech.com>
* Neil Peng<penggang@allwinnertech.com>
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
#include "egon2.h"

__hdle   nand_gpio_hd = 0;
__hdle hDMA;

/* DMA ��������  */
#define  CSP_DMAC_DMATYPE_NORMAL         			0
#define  CSP_DMAC_DMATYPE_DEDICATED      			1

#define CSP_DMAC_CFG_CONTINUOUS_ENABLE              (0x01)	//(0x01<<29)
#define CSP_DMAC_CFG_CONTINUOUS_DISABLE             (0x00)	//(0x01<<29)

//* DMA �ȴ�ʱ�� */
#define	CSP_DMAC_CFG_WAIT_1_DMA_CLOCK				(0x00)	//(0x00<<26)
#define	CSP_DMAC_CFG_WAIT_2_DMA_CLOCK				(0x01)	//(0x01<<26)
#define	CSP_DMAC_CFG_WAIT_3_DMA_CLOCK				(0x02)	//(0x02<<26)
#define	CSP_DMAC_CFG_WAIT_4_DMA_CLOCK				(0x03)	//(0x03<<26)
#define	CSP_DMAC_CFG_WAIT_5_DMA_CLOCK				(0x04)	//(0x04<<26)
#define	CSP_DMAC_CFG_WAIT_6_DMA_CLOCK				(0x05)	//(0x05<<26)
#define	CSP_DMAC_CFG_WAIT_7_DMA_CLOCK				(0x06)	//(0x06<<26)
#define	CSP_DMAC_CFG_WAIT_8_DMA_CLOCK				(0x07)	//(0x07<<26)

/* DMA ����Դ�� ���� */
/* DMA Ŀ�Ķ� ������ */
#define	CSP_DMAC_CFG_DEST_DATA_WIDTH_8BIT			(0x00)	//(0x00<<24)
#define	CSP_DMAC_CFG_DEST_DATA_WIDTH_16BIT			(0x01)	//(0x01<<24)
#define	CSP_DMAC_CFG_DEST_DATA_WIDTH_32BIT			(0x02)	//(0x02<<24)

/* DMA Ŀ�Ķ� ͻ������ģʽ */
#define	CSP_DMAC_CFG_DEST_1_BURST       			(0x00)	//(0x00<<23)
#define	CSP_DMAC_CFG_DEST_4_BURST		    		(0x01)	//(0x01<<23)

/* DMA Ŀ�Ķ� ��ַ�仯ģʽ */
#define	CSP_DMAC_CFG_DEST_ADDR_TYPE_LINEAR_MODE		(0x00)	//(0x00<<21)
#define	CSP_DMAC_CFG_DEST_ADDR_TYPE_IO_MODE 		(0x01)	//(0x01<<21)
#define	CSP_DMAC_CFG_DEST_ADDR_TYPE_HPAGE_MODE 		(0x02)	//(0x02<<21)
#define	CSP_DMAC_CFG_DEST_ADDR_TYPE_VPAGE_MODE 		(0x03)	//(0x03<<21)


/* DMA ����Դ�� ���� */
/* DMA Դ�� ������ */
#define	CSP_DMAC_CFG_SRC_DATA_WIDTH_8BIT			(0x00)	//(0x00<<8)
#define	CSP_DMAC_CFG_SRC_DATA_WIDTH_16BIT			(0x01)	//(0x01<<8)
#define	CSP_DMAC_CFG_SRC_DATA_WIDTH_32BIT			(0x02)	//(0x02<<8)

/* DMA Դ�� ͻ������ģʽ */
#define	CSP_DMAC_CFG_SRC_1_BURST       				(0x00)	//(0x00<<7)
#define	CSP_DMAC_CFG_SRC_4_BURST		    		(0x01)	//(0x01<<7)

/* DMA Դ�� ��ַ�仯ģʽ */
#define	CSP_DMAC_CFG_SRC_ADDR_TYPE_LINEAR_MODE		(0x00)	//(0x00<<5)
#define	CSP_DMAC_CFG_SRC_ADDR_TYPE_IO_MODE 			(0x01)	//(0x01<<5)
#define	CSP_DMAC_CFG_SRC_ADDR_TYPE_HPAGE_MODE 		(0x02)	//(0x02<<5)
#define	CSP_DMAC_CFG_SRC_ADDR_TYPE_VPAGE_MODE 		(0x03)	//(0x03<<5)

/* DMA ����Ŀ�Ķ� D��DMA Ŀ��ѡ�� */
#define	CSP_DMAC_CFG_DEST_TYPE_D_SRAM 				(0x00)	//(0x00<<16)
#define	CSP_DMAC_CFG_DEST_TYPE_D_SDRAM				(0x01)	//(0x01<<16)
#define	CSP_DMAC_CFG_DEST_TYPE_TCON0				(0x02)	//(0x02<<16)
#define	CSP_DMAC_CFG_DEST_TYPE_NFC  		    	(0x03)	//(0x03<<16)

/* DMA ����Դ�� D��DMA Ŀ��ѡ�� */
#define	CSP_DMAC_CFG_SRC_TYPE_D_SRAM 				(0x00)	//(0x00<<0)
#define	CSP_DMAC_CFG_SRC_TYPE_D_SDRAM				(0x01)	//(0x01<<0)
#define	CSP_DMAC_CFG_SRC_TYPE_TCON0				    (0x02)	//(0x02<<0)
#define	CSP_DMAC_CFG_SRC_TYPE_NFC  		    	   	(0x03)	//(0x03<<0)

void OSAL_CacheRangeFlush(void *addr, __u32 bytes, __u32 range)
{
	wlibc_CleanFlushDCacheRegion(addr, bytes);

	return;
}
/*
****************************************************************************************************
*
*             OSAL_DmaRequest
*
*  Description:
*       request dma
*
*  Parameters:
*		type	0: normal timer
*				1: special timer
*  Return value:
*		dma handler
*		if 0, fail
****************************************************************************************************
*/
int NAND_RequestDMA(void)
{
    hDMA = wBoot_dma_request(CSP_DMAC_DMATYPE_DEDICATED);
    return (int)hDMA;
}
/*
****************************************************************************************************
*
*             OSAL_DmaRelease
*
*  Description:
*       release dma
*
*  Parameters:
*       hDma	dma handler
*
*  Return value:
*		EPDK_OK/FAIL
****************************************************************************************************
*/
__s32 NAND_ReleaseDMA(void)
{
	/* stop dma                 */
    wBoot_dma_stop(hDMA);

    return wBoot_dma_release(hDMA);
}
/*
****************************************************************************************************
*
*             OSAL_DmaConfig
*
*  Description:
*       start interrupt
*
*  Parameters:
*       hTmr	timer handler
*		pArg    *(pArg + 0)         ctrl
*               *(pArg + 1)         page size
*               *(pArg + 2)         page step
*               *(pArg + 3)         comity & block count
*
*  Return value:
*		EPDK_OK/FAIL
*
**********************************************************************************************************************
*/
__s32 NAND_DMAConfigStart(int rw, unsigned int buff_addr, int len)
{
    __dma_setting_t  p;
    CSP_dma_config_t dma_param;
    CSP_dma_config_t *pArg = &dma_param;
    __u32 saddr, daddr;

	if(rw)  //write
	{
		if(buff_addr & 0xC0000000) //DRAM
			dma_param.src_drq_type = CSP_DMAC_CFG_SRC_TYPE_D_SDRAM;
		else  //SRAM
			dma_param.src_drq_type = CSP_DMAC_CFG_SRC_TYPE_D_SRAM;
		dma_param.src_addr_type = CSP_DMAC_CFG_SRC_ADDR_TYPE_LINEAR_MODE;  //linemode
		dma_param.src_burst_length = CSP_DMAC_CFG_SRC_4_BURST;  //burst mode
		dma_param.src_data_width = CSP_DMAC_CFG_SRC_DATA_WIDTH_32BIT;  //32bit

		dma_param.dst_drq_type = CSP_DMAC_CFG_DEST_TYPE_NFC;
		dma_param.dst_addr_type = CSP_DMAC_CFG_DEST_ADDR_TYPE_IO_MODE; //IO mode
		dma_param.dst_burst_length = CSP_DMAC_CFG_DEST_4_BURST; // burst mode
		dma_param.dst_data_width = CSP_DMAC_CFG_DEST_DATA_WIDTH_32BIT; //32 bit

		dma_param.wait_state = CSP_DMAC_CFG_WAIT_1_DMA_CLOCK; // invalid value
		dma_param.continuous_mode = CSP_DMAC_CFG_CONTINUOUS_DISABLE; //no continous

		dma_param.cmt_blk_cnt =  0x7f077f07; //commit register

	}
	else //read
	{
		dma_param.src_drq_type = CSP_DMAC_CFG_SRC_TYPE_NFC;
		dma_param.src_addr_type = CSP_DMAC_CFG_SRC_ADDR_TYPE_IO_MODE;  //IO mode
		dma_param.src_burst_length = CSP_DMAC_CFG_SRC_4_BURST;  //burst mode
		dma_param.src_data_width = CSP_DMAC_CFG_SRC_DATA_WIDTH_32BIT;  //32bit

		if(buff_addr & 0xC0000000) //DRAM
			dma_param.dst_drq_type = CSP_DMAC_CFG_DEST_TYPE_D_SDRAM;
		else  //SRAM
			dma_param.dst_drq_type = CSP_DMAC_CFG_DEST_TYPE_D_SRAM;
		dma_param.dst_addr_type = CSP_DMAC_CFG_DEST_ADDR_TYPE_LINEAR_MODE; //line mode
		dma_param.dst_burst_length = CSP_DMAC_CFG_DEST_4_BURST; // burst mode
		dma_param.dst_data_width = CSP_DMAC_CFG_DEST_DATA_WIDTH_32BIT; //32 bit

		dma_param.wait_state = CSP_DMAC_CFG_WAIT_1_DMA_CLOCK; // invalid value
		dma_param.continuous_mode = CSP_DMAC_CFG_CONTINUOUS_DISABLE; //no continous

		dma_param.cmt_blk_cnt =  0x7f077f07; //commit register
	}


    p.cfg.src_drq_type      = pArg->src_drq_type;
    p.cfg.src_addr_type     = pArg->src_addr_type;
    p.cfg.src_burst_length  = pArg->src_burst_length;
    p.cfg.src_data_width    = pArg->src_data_width;
    p.cfg.dst_drq_type      = pArg->dst_drq_type;
    p.cfg.dst_addr_type     = pArg->dst_addr_type;
    p.cfg.dst_burst_length  = pArg->dst_burst_length;
    p.cfg.dst_data_width    = pArg->dst_data_width;
    p.cfg.wait_state        = 0;
    p.cfg.continuous_mode   = 0;

    p.pgsz                  = 0;
    p.pgstp                 = 0;
    p.cmt_blk_cnt           = pArg->cmt_blk_cnt;

	wBoot_dma_Setting(hDMA, (void *)&p);

	if(rw) //write
	{
	    saddr = buff_addr;
	    daddr = 0x01c03030;
	}
	else
	{
	    daddr = buff_addr;
	    saddr = 0x01c03030;
	}

    wlibc_CleanFlushDCacheRegion((void *)buff_addr, len);
    return wBoot_dma_start(hDMA, saddr, daddr, len);

}
/*
**********************************************************************************************************************
*                                       OSAL_DmaQueryStatus
*
* Description: This function is used to query interrupt pending and clear pending bits if some pending is be set
*
* Arguments  : hDma         dma handle
*
* Returns    : bit31~24     main interrupt no
*              bit23~16     sub interrupt no
*              bit1         dma end interrupt flag
*              bit0         dma half end interrupt flag
**********************************************************************************************************************
*/
__u32  	NAND_QueryDmaStat(void)
{
    return wBoot_dma_QueryState(hDMA);
}


int NAND_WaitDmaFinish(void)
{
    return 0;
}

int NAND_ClkRequest(void)
{
    return 0;
}

void NAND_ClkRelease(void)
{
    return ;
}

int NAND_AHBEnable(void)
{
    *(volatile __u32 *)(CCMU_REGS_BASE + 0x60) |= 1U << 13;
    return 0;
}

void NAND_AHBDisable(void)
{
    *(volatile __u32 *)(CCMU_REGS_BASE + 0x60) &= ~(1U << 13);
}

int NAND_ClkEnable(void)
{
     *(volatile __u32 *)(CCMU_REGS_BASE + 0x80) |= 1U<< 31;

     return 0;
}

void NAND_ClkDisable(void)
{
    *(volatile __u32 *)(CCMU_REGS_BASE + 0x80) &= ~(1U << 31);
}

/*
**********************************************************************************************************************
*
*             NAND_GetCmuClk
*
*  Description:
*
*
*  Parameters:
*
*
*  Return value:
*
*
**********************************************************************************************************************
*/
__u32 _GetCmuClk(void)
{
	__u32 reg_val;
	__u32 factor_n;
	__u32 factor_k;
	__u32 clock;

	reg_val  = *(volatile unsigned int *)(0x01c20000 + 0x28);//PLL6
	factor_n = (reg_val >> 8) & 0x1f;
	factor_k = ((reg_val >> 4) & 0x3) + 1;

	clock = 24 * factor_n * factor_k/2;

	return clock;
}
/*
**********************************************************************************************************************
*
*             NAND_GetCmuClk
*
*  Description:
*
*
*  Parameters:
*
*
*  Return value:
*
*
**********************************************************************************************************************
*/
int NAND_SetClk(unsigned int nand_clock)
{
	__u32 edo_clk, cmu_clk;
	__u32 cfg;
	__u32 nand_clk_divid_ratio;

	/*set nand clock*/
	//edo_clk = (nand_max_clock > 20)?(nand_max_clock-10):nand_max_clock;
	edo_clk = nand_clock * 2;

    cmu_clk = _GetCmuClk( );
	nand_clk_divid_ratio = cmu_clk / edo_clk;
	if (cmu_clk % edo_clk)
			nand_clk_divid_ratio++;
	if (nand_clk_divid_ratio){
		if (nand_clk_divid_ratio > 16)
			nand_clk_divid_ratio = 15;
		else
			nand_clk_divid_ratio--;
	}
	/*set nand clock gate on*/
	cfg = *(volatile __u32 *)(CCMU_REGS_BASE + 0x80);

	/*gate on nand clock*/
	cfg |= (1U << 31);
	/*take cmu pll6 as nand src block*/
	cfg &= ~(0x3 << 24);
	cfg |=  (0x1 << 24);
	//set divn = 0
	cfg &= ~(0x03 << 16);

	/*set ratio*/
	cfg &= ~(0x0f << 0);
	cfg |= (nand_clk_divid_ratio & 0xf) << 0;

	*(volatile __u32 *)(CCMU_REGS_BASE + 0x80) = cfg;

	return 0;
}

int NAND_GetClk(void)
{
	__u32 cmu_clk;
	__u32 cfg;
	__u32 nand_max_clock, nand_clk_divid_ratio;

	/*set nand clock*/
    cmu_clk = _GetCmuClk( );

    /*set nand clock gate on*/
	cfg = *(volatile __u32 *)(CCMU_REGS_BASE + 0x80);
    nand_clk_divid_ratio = ((cfg)&0xf) +1;
    nand_max_clock = cmu_clk/(2*nand_clk_divid_ratio);

    return nand_max_clock;


}

void NAND_PIORequest(void)
{
	user_gpio_set_t  gpio_set[23];
	__s32            ret;

	ret = wBoot_script_parser_mainkey_get_gpio_cfg("nand_para", (void *)gpio_set, 23);
	if(!ret)
	{
		nand_gpio_hd = wBoot_GPIO_Request(gpio_set, 23);
	}
}



void NAND_PIORelease(void)
{
    if(nand_gpio_hd)
    {
        wBoot_GPIO_Release(nand_gpio_hd, 0);
    }
}

void * NAND_Malloc(unsigned int Size)
{
	return wBoot_malloc(Size);
}

void NAND_Free(void *pAddr, unsigned int Size)
{
	wBoot_free(pAddr);
}

void  OSAL_IrqUnLock(__u32  p)
{
    ;
}
void  OSAL_IrqLock  (__u32 *p)
{
    ;
}

int NAND_WaitRbReady(void)
{
    return 0;
}

void *NAND_IORemap(unsigned int base_addr, unsigned int size)
{
    return (void *)base_addr;
}
/*
**********************************************************************************************************************
*
*             OSAL_printf
*
*  Description:  �û����������趨�Ƿ���Ҫ��ӡ
*
*
*  Parameters:
*
*
*  Return value:
*
*
**********************************************************************************************************************
*/
int NAND_Print(const char * str, ...)
{
    wlibc_uprintf(str);

    return 0;
}
