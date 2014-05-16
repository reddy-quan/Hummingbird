#ifndef   _SECURITY_SYSTEM_H   
#define   _SECURITY_SYSTEM_H

#define SS_BASE				0xf1C15000
#define SS_BASE_PHY			0x01C15000

#define SS_CTL				(SS_BASE + 0x00)
#define SS_KEY				(SS_BASE + 0x04)  
#define SS_IV				(SS_BASE + 0x24)
#define SS_CNT				(SS_BASE + 0x34)  
#define SS_FCSR				(SS_BASE + 0x44)  
#define SS_ICSR				(SS_BASE + 0x48)
#define SS_MD				(SS_BASE + 0x4C)
#define SS_RXFIFO			(SS_BASE + 0x200)
#define SS_TXFIFO			(SS_BASE + 0x204)
#define SS_RXFIFO_PHY		(SS_BASE_PHY + 0x200)
#define SS_TXFIFO_PHY		(SS_BASE_PHY + 0x204)
#define ss_read_w(n)   		(*((volatile unsigned int *)(n)))          
#define ss_write_w(n,c) 	(*((volatile unsigned int *)(n)) = (c))

//#define SS_VIRTURE_ADDR (SW_PA_SSE_IO_BASE + 0xf0000000)
#define CCM_SS_SCLK_CTRL (SW_PA_CCM_IO_BASE  + 0x9c + 0xf0000000)
#define CCM_AHB_GATE0_CTRL (SW_PA_CCM_IO_BASE  + 0x60 + 0xf0000000)
#define SIZE_LIMIT (64 * 1024 / 4)
#endif  //_SECURITY_SYSTEM_H
