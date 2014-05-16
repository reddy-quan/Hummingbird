/*
************************************************************************************************************************
*                                                         eGON
*                                         the Embedded GO-ON Bootloader System
*
*                             Copyright(C), 2006-2008, SoftWinners Microelectronic Co., Ltd.
*											       All Rights Reserved
*
* File Name : Boot0_C_part.c
*
* Author : Gary.Wang
*
* Version : 1.1.0
*
* Date : 2007.12.18
*
* Description :
*
* Others : None at present.
*
*
* History :
*
*  <Author>        <time>       <version>      <description>
*
* Gary.Wang       2007.11.09      1.1.0        build the file
*
* Gary.Wang       2007.12.18      1.1.0        remove "BT0_self_rcv"
*
************************************************************************************************************************
*/
#include "boot0_i.h"
#include "arm_a8.h"

#include <string.h>

extern const boot0_file_head_t  BT0_head;
extern __u32 super_standby_flag;
__u32 odt_status;

static void move_RW( void );
static void clear_ZI( void );
static int  check_bootid(void);
static void timer_init(void);
static void print_version(void);
static __u32 check_odt(int ms);
static void open_cpuX(__u32 cpu);
static void close_cpuX(__u32 cpu);


static __u32 cpu_freq = 0;
static __u32 overhead = 0;
#define CCU_REG_VA (0xf1c20000)
#define CCU_REG_PA (0x01c20000)

#ifdef DRAM_CRC_TEST
#define SW_PA_TIMERC_IO_BASE              0x01c20c00
#define RTC_DATA_REG(x)				(SW_PA_TIMERC_IO_BASE + 0x120 + 4*(x))
#define lread(n)                    (*((volatile unsigned int *)(n)))
#define lwrite(addr,value)   (*((volatile unsigned int *)(addr)) = (value))

void standby_save2rtc(int index, unsigned int data)
{
    lwrite(RTC_DATA_REG(index), data);
}

unsigned int standby_get_from_rtc(int index)
{
    return lread(RTC_DATA_REG(index));
}
void standby_check_crc(void)
{
    int i, j;
    int *tmp = (int *)0x40000000;
    int crc = 0;
    int crc_check_bytes = 0;
     volatile int dbg=0x55;
    crc_check_bytes = standby_get_from_rtc(7);
    crc_check_bytes = crc_check_bytes & 7;
    standby_save2rtc(7, crc_check_bytes + 1);
    tmp += crc_check_bytes;
    msg("offset %d words!\n", crc_check_bytes);
    for(i = 0; i < 8; i++)
    {
        crc = 0;
        for(j = 0; j < 128 * 1024 * 1024; j+=32)
        {
            crc += *tmp;
            tmp += 8;
        }
        if (crc != standby_get_from_rtc(8 + i))
        {
            msg("%d M dram crc err!\n", i*128);
            msg("while here!\n");
           
            while(dbg==0x55);
        }
        else
        {
            msg("%d M dram crc ok!\n", i*128);
        }
    }
}
#endif
/*******************************************************************************
*��������: Boot0_C_part
*����ԭ�ͣ�void Boot0_C_part( void )
*��������: Boot0����C���Ա�д�Ĳ��ֵ�������
*��ڲ���: void
*�� �� ֵ: void
*��    ע:
*******************************************************************************/
void Boot0_C_part( void )
{
	__u32 status;
	__s32 dram_size;

	move_RW( );
	clear_ZI( );

	if(check_bootid())
    {
    	jump_to( FEL_BASE );
    }
    

    timer_init();
    UART_open( BT0_head.prvt_head.uart_port, (void *)BT0_head.prvt_head.uart_ctrl, 24*1000*1000 );
	odt_status = check_odt(5);
    if( BT0_head.prvt_head.enable_jtag )
    {
		jtag_init( (normal_gpio_cfg *)BT0_head.prvt_head.jtag_gpio );
    }
	msg("HELLO! BOOT0 is starting!\n");
	print_version();
    
    init_perfcounters(1, 0);
	change_runtime_env(0);

    open_cpuX(1);
    close_cpuX(1);
    
	mmu_system_init(EGON2_DRAM_BASE, 1 * 1024, EGON2_MMU_BASE);
	mmu_enable();
    
    if(BT0_head.boot_head.platform[7])
    {
        msg("read dram para.\n");
    }else
    {
        msg("try dram para.\n");
    }
	dram_size = init_DRAM(BT0_head.boot_head.platform[7]);                                // ��ʼ��DRAM


    if(dram_size)
	{
		msg("dram size =%dMB\n", dram_size);
	}
	else
	{
		msg("initializing SDRAM Fail.\n");
		mmu_disable( );
		jump_to( FEL_BASE );
	}
	msg("%x\n", *(volatile int *)0x52000000);
	msg("super_standby_flag = %d\n", super_standby_flag);
	if(1 == super_standby_flag)
	{
		//tmr_enable_watchdog();
		//disable_icache();
        #ifdef DRAM_CRC_TEST
        msg("hao...\n");
		standby_check_crc();
        #endif
		jump_to( 0x52000000 );
	}

	#if SYS_STORAGE_MEDIA_TYPE == SYS_STORAGE_MEDIA_NAND_FLASH
		status = load_Boot1_from_nand( );         // ����Boot1
	#elif SYS_STORAGE_MEDIA_TYPE == SYS_STORAGE_MEDIA_SPI_NOR_FLASH
		status = load_boot1_from_spinor( );         // ����Boot1
	#elif SYS_STORAGE_MEDIA_TYPE == SYS_STORAGE_MEDIA_SD_CARD
		status = load_boot1_from_sdmmc( (char *)BT0_head.prvt_head.storage_data );  // ����boot1
	#else
		#error The storage media of Boot1 has not been defined.
	#endif


	msg("Ready to disable icache.\n");

	mmu_disable( );                               // disable instruction cache

	if( status == OK )
	{
//		restart_watch_dog( );                     // restart watch dog
		//��תboot1֮ǰ����dram�Ĵ�Сд��ȥ
		//set_dram_size(dram_size );
		//��ת֮ǰ�������е�dram����д��boot1��
		set_dram_para((void *)&BT0_head.prvt_head.dram_para, dram_size);
        
        #if SYS_STORAGE_MEDIA_TYPE == SYS_STORAGE_MEDIA_NAND_FLASH
        set_nand_good_block_ratio_para((void *)&BT0_head.prvt_head.storage_data);
        #endif
        
		msg("Succeed in loading Boot1.\n"
		    "Jump to Boot1.\n");
		jump_to( BOOT1_BASE );                    // �������Boot1�ɹ�����ת��Boot1��ִ��
	}
	else
	{
//		disable_watch_dog( );                     // disable watch dog

		msg("Fail in loading Boot1.\n"
		    "Jump to Fel.\n");
		jump_to( FEL_BASE );                      // �������Boot1ʧ�ܣ�������Ȩ����Fel
	}
}
/*******************************************************************************
*��������: set_pll
*����ԭ�ͣ�void set_pll( void )
*��������: Boot0����C���Ա�д�� ����CPUƵ��
*��ڲ���: void
*�� �� ֵ: void
*��    ע:
*******************************************************************************/
void set_pll( void )
{
	__u32 reg_val, i;

	//�л���24M
	CCMU_REG_AHB_APB = 0x00010010;
	//����PLL1��384M
	reg_val = (0x21005000) | (0x80000000);
	CCMU_REG_PLL1_CTRL = reg_val;
	//��ʱ
	for(i=0;i<200;i++);
	//�л���PLL1
	reg_val = CCMU_REG_AHB_APB;
	reg_val &= ~(3 << 16);
	reg_val |=  (2 << 16);
	CCMU_REG_AHB_APB = reg_val;
	//��DMA
	CCMU_REG_AHB_MOD0 |= 1 << 6;

	//��PLL6�����õ�Ĭ��Ƶ��600M
	reg_val = CCMU_REG_PLL6_CTRL;
	reg_val |= 1<<31;
	CCMU_REG_PLL6_CTRL = reg_val;

	return ;
}


static __u32 check_odt(int ms)
{
#if 0
	__u32 value, tmp_cfg, tmp_pull;
	__u32 t1, t2;

	value = *(volatile __u32 *)(0x01c20828);
	tmp_cfg = value;

	value &= ~(0x07 << 24);
	*(volatile __u32 *)(0x01c20828) = value;

	value = *(volatile __u32 *)(0x01c20840);
	tmp_pull = value;
	value &= ~(0x03 << 28);
	value |=  (0x01 << 28);
	*(volatile __u32 *)(0x01c20840) = value;

	t1 = *(volatile unsigned int *)(0x01c20C00 + 0x84);
	t2 = t1 + ms;
	do
	{
		t1 = *(volatile unsigned int *)(0x01c20C00 + 0x84);
	}
	while(t2 >= t1);

	value = (*(volatile __u32 *)(0x01c20834)) & (1 << 14);

	*(volatile __u32 *)(0x01c20840) = tmp_pull;
	*(volatile __u32 *)(0x01c20828) = tmp_cfg;

	return value;
#endif
	return 0;
}

static __s32 check_bootid(void)
{
#if 0
	__u32 reg = 0x01c23800;
	__u32 value, i;

	for(i=0;i<4;i++)
	{
		value = *(volatile __u32 *)(reg + 0x10 + (i<<2));
		if(value)
		{
			return -1;
		}
	}
#endif
	return 0;
}

static void timer_init(void)
{
	*(volatile unsigned int *)(0x01c20000 + 0x144) |= (1U << 31);
	*(volatile unsigned int *)(0x01c20C00 + 0x80 )  = 1;
	*(volatile unsigned int *)(0x01c20C00 + 0x8C )  = 0x2EE0;
	*(volatile unsigned int *)(0x01c20C00 + 0x84 )  = 0;
}

static void print_version(void)
{
	msg("boot0 version : %s\n", BT0_head.boot_head.platform + 2);

	return;
}

static void move_RW( void )
{
	__u8    *psrc8;
	__u8    *pdst8;
	__u32   *psrc32;
	__u32   *pdst32;
	__u32   size;
	__u32   N;

	extern unsigned char Load$$Boot0_RW_ZI$$Base;
	extern unsigned char Image$$Boot0_RW_ZI$$Base;
	extern unsigned char Image$$Boot0_RW_ZI$$Length;

	size = (__u32) &Image$$Boot0_RW_ZI$$Length;
	psrc32  = (__u32 *)&Load$$Boot0_RW_ZI$$Base;
	pdst32  = (__u32 *)&Image$$Boot0_RW_ZI$$Base;

	N = size >> 4;
	while( N-- > 0 )
	{
		*pdst32++ = *psrc32++;
		*pdst32++ = *psrc32++;
		*pdst32++ = *psrc32++;
		*pdst32++ = *psrc32++;
	}

	N = size & ( ( 1 << 4 ) - 1 );
	psrc8 = (__u8 *)psrc32;
	pdst8 = (__u8 *)pdst32;
	while( N--)
	{
		*pdst8++ = *psrc8++;
	}
}



static void clear_ZI( void )
{
	__u8  *p8;
	__u32 *p32;
	__u32 size;
	__u32 N;

	extern unsigned char Image$$Boot0_RW_ZI$$ZI$$Base;
	extern unsigned char Image$$Boot0_RW_ZI$$ZI$$Length;

	size = (__u32)  &Image$$Boot0_RW_ZI$$ZI$$Length;
	p32  = (__u32 *)&Image$$Boot0_RW_ZI$$ZI$$Base;

	N = size >> 4;
	while( N-- > 0 )
	{
		*p32++ = 0;
		*p32++ = 0;
		*p32++ = 0;
		*p32++ = 0;
	}

	N = size & ( ( 1 << 4 ) - 1 );
	p8 = (__u8 *)p32;
	while( N--)
	{
		*p8++ = 0;
	}
}


__u32 get_cyclecount (void)
{
	__u32 value;
	// Read CCNT Register
	__asm{MRC p15, 0, value, c9, c13, 0}
	return value;
}

void init_perfcounters (__u32 do_reset, __u32 enable_divider)
{
	// in general enable all counters (including cycle counter)
	__u32 value = 1;

	// peform reset:
	if (do_reset)
	{
		value |= 2;     // reset all counters to zero.
		value |= 4;     // reset cycle counter to zero.
	}

	if (enable_divider)
		value |= 8;     // enable "by 64" divider for CCNT.

	value |= 16;

	// program the performance-counter control-register:
	__asm {MCR p15, 0, value, c9, c12, 0}

	// enable all counters:
	value = 0x8000000f;
	__asm {MCR p15, 0, value, c9, c12, 1}

	// clear overflows:
	__asm {MCR p15, 0, value, c9, c12, 3}

	return;
}

void change_runtime_env(__u32 mmu_flag)
{
	__u32 factor_n = 0;
	__u32 factor_k = 0;
	__u32 factor_m = 0;
	__u32 factor_p = 0;
	__u32 start = 0;
	__u32 cmu_reg = 0;
	volatile __u32 reg_val = 0;

	if(mmu_flag){
		cmu_reg = CCU_REG_VA;
	}else{
		cmu_reg = CCU_REG_PA;
	}
	//init counters:
	//init_perfcounters (1, 0);
	// measure the counting overhead:
	start = get_cyclecount();
	overhead = get_cyclecount() - start;
	//busy_waiting();
	//get runtime freq: clk src + divider ratio
	//src selection
	reg_val = *(volatile int *)(cmu_reg + 0x54);
	reg_val >>= 16;
	reg_val &= 0x3;
	if(0 == reg_val){
		//32khz osc
		cpu_freq = 32;

	}else if(1 == reg_val){
		//hosc, 24Mhz
		cpu_freq = 24000; 			//unit is khz
	}else if(2 == reg_val){
		//get pll_factor
		reg_val = *(volatile int *)(cmu_reg + 0x00);
		factor_p = 0x3 & (reg_val >> 16);
		factor_p = 1 << factor_p;		//1/2/4/8
		factor_n = 0x1f & (reg_val >> 8); 	//the range is 0-31
		factor_k = (0x3 & (reg_val >> 4)) + 1; 	//the range is 1-4
		factor_m = (0x3 & (reg_val >> 0)) + 1; 	//the range is 1-4

		cpu_freq = (24000*factor_n*factor_k)/(factor_p*factor_m);
		//cpu_freq = raw_lib_udiv(24000*factor_n*factor_k, factor_p*factor_m);
		//msg("cpu_freq = dec(%d). \n", cpu_freq);
		//busy_waiting();
	}

}

/*
 * input para range: 1-1000 us, so the max us_cnt equal = 1008*1000;
 */
void delay_us(__u32 us)
{
	__u32 us_cnt = 0;
	__u32 cur = 0;
	__u32 target = 0;

	//us_cnt = ((raw_lib_udiv(cpu_freq, 1000)) + 1)*us;
	us_cnt = ((cpu_freq/1000) + 1)*us;
	cur = get_cyclecount();
	target = cur - overhead + us_cnt;

#if 1
	while(!counter_after_eq(cur, target))
	{
		cur = get_cyclecount();
		//cnt++;
	}
#endif


	return;
}


__asm void cpuX_startup_to_wfi(void)
{

    mrs r0, cpsr
    bic r0, r0, #ARMV7_MODE_MASK
    orr r0, r0, #ARMV7_SVC_MODE
    orr r0, r0, #( ARMV7_IRQ_MASK | ARMV7_FIQ_MASK )   // After reset, ARM automaticly disables IRQ and FIQ, and runs in SVC mode.
    bic r0, r0, #ARMV7_CC_E_BIT                         // set little-endian
    msr cpsr_c, r0

    // configure memory system : disable MMU,cache and write buffer; set little_endian;
    mrc p15, 0, r0, c1, c0
    bic r0, r0, #( ARMV7_C1_M_BIT | ARMV7_C1_C_BIT )// disable MMU, data cache
    bic r0, r0, #( ARMV7_C1_I_BIT | ARMV7_C1_Z_BIT )// disable instruction cache, disable flow prediction
    bic r0, r0, #( ARMV7_C1_A_BIT)                  // disable align
    mcr p15, 0, r0, c1, c0                          
    // set SP for SVC mode
    mrs r0, cpsr
    bic r0, r0, #ARMV7_MODE_MASK
    orr r0, r0, #ARMV7_SVC_MODE
    msr cpsr_c, r0
    ldr sp, =0xb400

    //let the cpu1+ enter wfi state;
    /* step3: execute a CLREX instruction */
    clrex
    /* step5: execute an ISB instruction */
    isb sy
    /* step6: execute a DSB instruction  */
    dsb sy
    /* step7: execute a WFI instruction */
    wfi
    /* step8:wait here */
    b .
}

#define SW_PA_CPUCFG_IO_BASE              0x01c25c00
/*
 * CPUCFG
 */
#define AW_CPUCFG_P_REG0            0x01a4
#define CPUX_RESET_CTL(x) (0x40 + (x)*0x40)
#define CPUX_CONTROL(x)   (0x44 + (x)*0x40)
#define CPUX_STATUS(x)    (0x48 + (x)*0x40)
#define AW_CPUCFG_GENCTL            0x0184
#define AW_CPUCFG_DBGCTL0           0x01e0
#define AW_CPUCFG_DBGCTL1           0x01e4

#define AW_CPU1_PWR_CLAMP         0x01b0
#define AW_CPU1_PWROFF_REG        0x01b4
#define readl(addr)		(*((volatile unsigned long  *)(addr)))
#define writel(v, addr)	(*((volatile unsigned long  *)(addr)) = (unsigned long)(v))

#define IO_ADDRESS(IO_ADDR) (IO_ADDR)
#define IS_WFI_MODE(cpu)    (readl(IO_ADDRESS(SW_PA_CPUCFG_IO_BASE) + CPUX_STATUS(cpu)) & (1<<2))

void open_cpuX(__u32 cpu)
{
    long paddr;
    __u32 pwr_reg;

    paddr = (__u32)cpuX_startup_to_wfi;
    writel(paddr, IO_ADDRESS(SW_PA_CPUCFG_IO_BASE) + AW_CPUCFG_P_REG0);

    /* step1: Assert nCOREPORESET LOW and hold L1RSTDISABLE LOW.
              Ensure DBGPWRDUP is held LOW to prevent any external
              debug access to the processor.
    */
    /* assert cpu core reset */
    writel(0, IO_ADDRESS(SW_PA_CPUCFG_IO_BASE) + CPUX_RESET_CTL(cpu));
    /* L1RSTDISABLE hold low */
    pwr_reg = readl(IO_ADDRESS(SW_PA_CPUCFG_IO_BASE) + AW_CPUCFG_GENCTL);
    pwr_reg &= ~(1<<cpu);
    writel(pwr_reg, IO_ADDRESS(SW_PA_CPUCFG_IO_BASE) + AW_CPUCFG_GENCTL);
    /* DBGPWRDUP hold low */
    pwr_reg = readl(IO_ADDRESS(SW_PA_CPUCFG_IO_BASE) + AW_CPUCFG_DBGCTL1);
    pwr_reg &= ~(1<<cpu);
    writel(pwr_reg, IO_ADDRESS(SW_PA_CPUCFG_IO_BASE) + AW_CPUCFG_DBGCTL1);


    /* step3: clear power-off gating */
    pwr_reg = readl(IO_ADDRESS(SW_PA_CPUCFG_IO_BASE) + AW_CPU1_PWROFF_REG);
    pwr_reg &= ~(1);
    writel(pwr_reg, IO_ADDRESS(SW_PA_CPUCFG_IO_BASE) + AW_CPU1_PWROFF_REG);
    delay_us(1000);

    /* step4: de-assert core reset */
    writel(3, IO_ADDRESS(SW_PA_CPUCFG_IO_BASE) + CPUX_RESET_CTL(cpu));

    /* step5: assert DBGPWRDUP signal */
    pwr_reg = readl(IO_ADDRESS(SW_PA_CPUCFG_IO_BASE) + AW_CPUCFG_DBGCTL1);
    pwr_reg |= (1<<cpu);
    writel(pwr_reg, IO_ADDRESS(SW_PA_CPUCFG_IO_BASE) + AW_CPUCFG_DBGCTL1);

}

void close_cpuX(__u32 cpu)
{
    __u32 pwr_reg;

    while(!IS_WFI_MODE(cpu));
    /* step1: deassert cpu core reset */
    writel(0, IO_ADDRESS(SW_PA_CPUCFG_IO_BASE) + CPUX_RESET_CTL(cpu));

    /* step2: deassert DBGPWRDUP signal */
    pwr_reg = readl(IO_ADDRESS(SW_PA_CPUCFG_IO_BASE) + AW_CPUCFG_DBGCTL1);
    pwr_reg &= ~(1<<cpu);
    writel(pwr_reg, IO_ADDRESS(SW_PA_CPUCFG_IO_BASE) + AW_CPUCFG_DBGCTL1);

    /* step3: set up power-off signal */
    pwr_reg = readl(IO_ADDRESS(SW_PA_CPUCFG_IO_BASE) + AW_CPU1_PWROFF_REG);
    pwr_reg |= 1;
    writel(pwr_reg, IO_ADDRESS(SW_PA_CPUCFG_IO_BASE) + AW_CPU1_PWROFF_REG);
   
}



