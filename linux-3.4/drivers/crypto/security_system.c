#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/spinlock.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/timer.h>
#include <linux/err.h>
#include <linux/ctype.h>
#include <asm/io.h>
#include <linux/slab.h>
#include <mach/platform.h>
#include <asm/uaccess.h>
#include <mach/dma.h>
#include <linux/gfp.h>
#include <asm/dma.h>
#include <linux/dma-mapping.h>
#include <asm/dma-mapping.h>
#include <asm/cacheflush.h>
#include <linux/mutex.h>

#include "security_system.h"

#if 0
#define CCM_AHB_RST
#endif
#if 0
#define SS_DEBUG(fmt...) printk(fmt);
#define DEBUG_SS
#else
#define SS_DEBUG(fmt...)
#endif	


struct mutex ss_mutex;
const unsigned int buffsize =1024 * 1024;

unsigned char	userkey[32];
unsigned char	ivec[32];
unsigned int	tmp_buf[1024 * 1024 /4];

struct ss_dma_para {
    char *name;
    dma_addr_t dma_addr;
};

struct security_ss {
	/*main args*/
	unsigned long		in_length;
	unsigned long		out_length;
	unsigned char		*in_buf;
	unsigned char		*out_buf;
    struct ss_dma_para  dma_para_tx;
    struct ss_dma_para  dma_para_rx;
    dma_hdl_t           dma_hdl_tx;
    dma_hdl_t           dma_hdl_rx;
    dma_cb_t            dma_cb_tx;
    dma_cb_t            dma_cb_rx;
	char				cnt_size;
	char				cpu_dma;
	unsigned int		bits;
	char				iv_number;		/*4:md5 5:sha1*/
	char				de_en_crypt;	/*0:encrypt 1:decrypt*/
	char				key_select;		/*only aes/des/3des available */
	char				ss_mode;		/*000:aes 001:des 010:3des 011:sha-1 100:MD5 101:PRNG*/
	char				op_mode;		/*00:ecb 01:cbc 10:cnt*/
	char				prng_mode;		/*0: one-shot 1: contunue*/
	char				iv_mode;		/*0:const 1:Arbitraty only be used for md5 and sha/1*/
};

struct ss_classdev {
    const char      	*name;
    struct device   	*dev;
	struct security_ss	ss;
};

static struct ss_dma_para ss_dma_para_tx = {
	.name		= "SECURITY SYSTEM TX",
	.dma_addr	= SS_TXFIFO_PHY,
};

static struct ss_dma_para ss_dma_para_rx = {
	.name		= "SECURITY SYSTEM RX",
	.dma_addr	= SS_RXFIFO_PHY,
};



/* ****************************************************************************
//	void ss_init(void)
//  Description:	Start SS
//
//	Arguments:		none
//  
//	Return Value:	None
*************************************************************************** */
void ss_init(void)
{
	printk("ss init!\n");
}
/*
****************************************************************************
//	void ss_exit(void)
//  Description:	Disable SS
//
//	Arguments:		none
//  
//	Return Value:	None
**************************************************************************
*/
void ss_exit(void)
{
	unsigned int reg_val;

	reg_val = ss_read_w(SS_CTL);
	reg_val &= ~0x1;
	mutex_lock(&ss_mutex);
	ss_write_w(SS_CTL, reg_val);
	mutex_unlock(&ss_mutex);
}

/* ****************************************************************************
//	void ss_sys_open(unsigned int clock_source)
//  Description:	Request system source for SS
//
//	Arguments:		unsigned int clock_source 0: HOSC 24M, 1: PLL6
//
//	Return Value:	None
**************************************************************************
*/
void ss_sys_open(unsigned int clock_source)
{
	unsigned int reg_val;

	/*enable SS working clock*/
	reg_val = ss_read_w(CCM_SS_SCLK_CTRL);
	if (clock_source == 0){
		reg_val &= ~(0x3<<24);
		reg_val &= ~(0x3<<16);
		reg_val &= ~(0xf);
		reg_val |= 0x0<<16;
		reg_val |= 0;
	}else if(clock_source == 1 ){
		reg_val &= ~(0x3<<24);
		reg_val |= 0x1<<24;
		reg_val &= ~(0x3<<16);
		reg_val &= ~(0xf);	
		reg_val |= 0x0<<16;			/* /1*/
		reg_val |= (4 -1);			/* /4*/
	}else{
		reg_val &= ~(0x3<<24);
		reg_val |= 0x2<<24;
		reg_val &= ~(0x3<<16);
		reg_val &= ~(0xf);	
		reg_val |= 0x0<<16;			/* /1*/
		reg_val |= (8-1);
	}

	reg_val |= 0x1U<<31;
	mutex_lock(&ss_mutex);
	ss_write_w(CCM_SS_SCLK_CTRL, reg_val);
	mutex_unlock(&ss_mutex);
/*
	for(i=0; i<0x1000; i++){};
	enable SS AHB clock
*/
	reg_val = ss_read_w(CCM_AHB_GATE0_CTRL);
	reg_val |= 0x1<<5;		/*SS AHB clock on*/
	mutex_lock(&ss_mutex);
	ss_write_w(CCM_AHB_GATE0_CTRL, reg_val);
	mutex_unlock(&ss_mutex);

#ifdef CCM_AHB_RST
	/*release SS reset signal*/
	reg_val = ss_read_w(CCM_AHB1_RST_REG0);
	reg_val |= 0x1<<5;
	mutex_lock(&ss_mutex);
	ss_write_w(CCM_AHB1_RST_REG0, reg_val);
	mutex_unlock(&ss_mutex);
/*	for(i=0; i<0x1000; i++){};*/
#endif
}


/*
****************************************************************************
//	void ss_sys_close(void)
//  Description:	Release system source for SS
//
//	Arguments:		None
//
//	Return Value:	None
****************************************************************************/
void ss_sys_close(void)
{
	unsigned int reg_val;

#ifdef CCM_AHB_RST
	/*reset SS*/
	reg_val = ss_read_w(CCM_AHB1_RST_REG0);
	reg_val &= ~(0x1<<5);
	mutex_lock(&ss_mutex);
	ss_write_w(CCM_AHB1_RST_REG0, reg_val);
	mutex_unlock(&ss_mutex);
#endif

	/*disable SS AHB clock*/
	reg_val = ss_read_w(CCM_AHB_GATE0_CTRL);
	reg_val &= ~(0x1<<5);			/*SS AHB clock off*/
	mutex_lock(&ss_mutex);
	ss_write_w(CCM_AHB_GATE0_CTRL, reg_val);
	mutex_unlock(&ss_mutex);

	/*disable SS working clock*/
	reg_val = ss_read_w(CCM_SS_SCLK_CTRL);
	reg_val &= ~(0x1U<<31);		/*SS working clock off*/
	mutex_lock(&ss_mutex);
	ss_write_w(CCM_SS_SCLK_CTRL, reg_val);
	mutex_unlock(&ss_mutex);
/*	for(i=0; i<0x1000; i++){};*/
}


/*
	name: ss_setup_aes_des
	set control register when in aes or des mode
	args:
		unsigned int ss_method	0: AES 1: DES 2: 3DES
		unsigned int ss_opdir	0: encrypt 1: decrypt
		unsigned int ss_keysize	128/192/256 bit
		unsigned int ss_opmod	0: ECB 1: CBC 2: CNT
		unsigned int cnt_width	64/32/16
	return NULL;
*/
void ss_setup_aes_des(unsigned int ss_method, unsigned int ss_opdir,
		unsigned int ss_keysize, unsigned int ss_opmod, unsigned int cnt_width)
{
	unsigned int reg_val;
	SS_DEBUG("ss_keysize %d\n",ss_keysize);
	reg_val = ss_read_w(SS_CTL);
	/*set ss_method*/
	reg_val &= ~(0x7<<4);
	reg_val |= (ss_method&0x7)<<4;
	reg_val &= ~(0x1<<7);
	/*set decrypt or encrypt*/
	if(ss_opdir)
		reg_val |= 0x1<<7;
	else
		reg_val&= ~(0x1<<7);

	/*set key size*/
	reg_val &= ~(0x3<<8);
	if(ss_keysize == 128)
		reg_val |= 0x0<<8;			/*128 bits key*/
	else if(ss_keysize == 192)
		reg_val |= 0x1<<8;			/*192 bits key*/
	else
		reg_val |= 0x2<<8;			/*256 bits key*/

	/*set operation mode*/
	reg_val &= ~(0x3<<12);
	reg_val |= (ss_opmod&0x3)<<12;		/*operation mode(ECB, CBC, CNT)*/

	/*set CNT width for CNT mode*/
	reg_val &= ~(0x3<<10);
	if(cnt_width == 64)
		reg_val |= 0x2<<10;			/*64 bits CNT width for CNT mode*/
	else if(cnt_width == 32)
		reg_val |= 0x1<<10;			/*32 bits CNT width for CNT mode*/
	else
		reg_val |= 0x0<<10;			/*16 bits CNT width for CNT mode*/

	/*write register*/
	mutex_lock(&ss_mutex);
	ss_write_w(SS_CTL, reg_val);
	mutex_unlock(&ss_mutex);
}

/*
	name: ssio_setup_sha1_md5
		set up sha1 or md5 mode
	args:
		unsigned int mode: 		0: SHA1, 1: MD5
		unsigned int iv_mode:	0:constants 1: arbitrary
	return:
		null
*/
void ssio_setup_sha1_md5(unsigned int mode, unsigned int iv_mode)
{
	unsigned int reg_val;
	reg_val = ss_read_w(SS_CTL);
	reg_val &= ~(0x7<<4);
	if((mode&0x7) == 0)
		reg_val |= 0x3<<4;			/*SHA1*/
	else if((mode&0x7) == 1)
		reg_val |= 0x4<<4;			/*MD5*/
	else{
		printk("err mode! \nthe function only be used to sha1 md5!\n");
		return;
	}
	reg_val &= ~(0x1<<14);
	if(iv_mode)
		reg_val |= 0x1<<14;			/*IV steady of its constants*/
	mutex_lock(&ss_mutex);
	ss_write_w(SS_CTL, reg_val);
	mutex_unlock(&ss_mutex);
}

/*
	name: ssio_set_key
		set key to hardware
	args:
		const unsigned char *userkey: key
		const int bits:               key_size
	return:
		success 0, fail -1
*/
int ssio_set_key(const unsigned char *userkey, const int bits)
{
	unsigned int i;
	unsigned int io_buf[8];
	if (!userkey)
        return -1;
	for(i=0; i<(bits/32); i++){
		io_buf[i] = *(userkey + (4 * i));
		io_buf[i] |= *(userkey + (4 * i)+ 1) << 8;
		io_buf[i] |= *(userkey + (4 * i)+ 2) << 16;
		io_buf[i] |= *(userkey + (4 * i)+ 3) << 24;
	}
	mutex_lock(&ss_mutex);
	for(i=0; i<(bits/32); i++){
		SS_DEBUG("key %d is 0x%x\n",i,io_buf[i]);
		ss_write_w(SS_KEY + 4*i, io_buf[i]);
	}
	mutex_unlock(&ss_mutex);;
	return 0;
}

/*
	name: ssio_set_iv
		set iv to hardware
	args:
		unsigned char iv_number	: number of iv
		unsigned int *iv_buf	: buf which store iv
	return:
		null
*/
void ssio_set_iv(unsigned char iv_number,unsigned int *iv_buf )
{
	int k,i;
	k = iv_number;			/*IV number in word unit*/
	mutex_lock(&ss_mutex);
	if(k > 4){
		for(i=0; i<4; i++){
			ss_write_w(SS_IV + 4*i, *(iv_buf + i));
			/*aw_delay(0x100);*/
		}
		ss_write_w(SS_CNT, *(iv_buf + 5));
		/*aw_delay(0x100);*/
	}else{
		for(i=0; i<k; i++){
			printk("iv_buf%d: %x\n",i,*(iv_buf + i));
			ss_write_w(SS_IV + 4*i, *(iv_buf + i));
			/*aw_delay(0x100);*/
		}
	}
	mutex_unlock(&ss_mutex);
}

/*
	name: ssio_set_cnt
		start up ss
	args:
		unsigned int *cnt_buf: buf of cnt
	return:
		null
*/
void ssio_set_cnt(unsigned int cnt_number, unsigned int *cnt_buf)
{
	int k,i;
	k = cnt_number/16;			/*CNT number in word unit*/
	mutex_lock(&ss_mutex);
	for(i=0; i<k; i++){
		ss_write_w(SS_CNT + 4*i, *(cnt_buf + i));
		/*aw_delay(0x100);*/
	}
	mutex_unlock(&ss_mutex);
}

/*
	name: ssio_start
		start up ss
	args:
		null
	return:
		null
*/
void ssio_start(void)
{
	unsigned int reg_val;
	reg_val = ss_read_w(SS_CTL);
	reg_val |= 0x1;
	mutex_lock(&ss_mutex);
	ss_write_w(SS_CTL, reg_val);
	mutex_unlock(&ss_mutex);
}

/*
	name: ssio_stop
		stop ss
	args:
		null
	return:
		null
*/
void ssio_stop(void)
{
	unsigned int reg_val;
	reg_val = ss_read_w(SS_CTL);
	reg_val &= ~0x1;
	mutex_lock(&ss_mutex);
	ss_write_w(SS_CTL, reg_val);
	mutex_unlock(&ss_mutex);
}

/*
	name: ssio_enable_dma
		ss dma enalbe
	args:
		null
	return:
		null
*/
void ssio_enable_dma(void)
{
	unsigned int reg_val;
	reg_val = ss_read_w(SS_ICSR);
	reg_val |= 0x1<<4;
	mutex_lock(&ss_mutex);
	ss_write_w(SS_ICSR, reg_val);
	mutex_unlock(&ss_mutex);
}

/*
	name: ssio_disable_dma
		ss dma disalbe
	args:
		null
	return:
		null
*/
void ssio_disable_dma(void)
{
	unsigned int reg_val;
	reg_val = ss_read_w(SS_ICSR);
	reg_val &= ~(0x1<<4);
	mutex_lock(&ss_mutex);
	ss_write_w(SS_ICSR, reg_val);
	mutex_unlock(&ss_mutex);
}

/*
	name: ssio_tx_int
		set ss tx_fifo int bit
	args:
		unsigned int flags: 0:disable 1:enable 
	return:
		null
*/
void ssio_tx_int(unsigned int flags)
{
	unsigned int reg_val;
	reg_val = ss_read_w(SS_ICSR);
	if(flags)
		reg_val |= 0x1;
	else
		reg_val &= ~0x1;

	mutex_lock(&ss_mutex);
	ss_write_w(SS_ICSR, reg_val);
	mutex_unlock(&ss_mutex);
}

/*
	name: ssio_rx_int
		set ss rx_fifo int bit
	args:
		unsigned int flags: 0:disable 1:enable 
	return:
		null
*/
void ssio_rx_int(unsigned int flags)
{
	unsigned int reg_val;
	reg_val = ss_read_w(SS_ICSR);
	if(flags)
		reg_val |= 0x1 << 2 ;
	else
		reg_val &= ~ (0x1 << 2);
	mutex_lock(&ss_mutex);
	ss_write_w(SS_ICSR, reg_val);
	mutex_unlock(&ss_mutex);
}

/*
	name: ssio_rx_trigger_level
		set rx int trigger level
	args:
		unsigned int level: trigger level
	return:
		null
*/
void ssio_rx_trigger_level(unsigned int level)
{
	unsigned int reg_val;
	reg_val = ss_read_w(SS_FCSR);
	reg_val &= ~(0x1f<<8);
	reg_val |= ((level - 1)&0x1f)<<8;
	mutex_lock(&ss_mutex);
	ss_write_w(SS_FCSR, reg_val);
	mutex_unlock(&ss_mutex);
}

/*
	name: ssio_tx_trigger_level
		set tx int trigger level
	args:
		unsigned int level: trigger level 
	return:
		null
*/
void ssio_tx_trigger_level(unsigned int level)
{
	unsigned int reg_val;
	reg_val = ss_read_w(SS_FCSR);
	reg_val &= ~0x1f;
	reg_val |= (level - 1)&0x1f;
	mutex_lock(&ss_mutex);
	ss_write_w(SS_FCSR, reg_val);
	mutex_unlock(&ss_mutex);
}

/*
	name: ssio_setup_random
		ssio setup random mode
	args:
		unsigned int mode: 0: one-shot 1:continue 
	return:
		null
*/
void ssio_setup_random(unsigned int mode)
{
	unsigned int reg_val;
	reg_val = ss_read_w(SS_CTL);
	reg_val &= ~(0x7<<4);		
	reg_val |= 0x5<<4;			/*random data*/
	reg_val &= ~(0x1<<15);
	if(mode)
		reg_val |= 0x1<<15;		/*0: one shot mode, 1: continue mode*/
	else
		reg_val &= ~(0x1<<15);
	mutex_lock(&ss_mutex);
	ss_write_w(SS_CTL, reg_val);
	mutex_unlock(&ss_mutex);
}

/*
	name: ssio_random_oneshot_start
		ssio setup random mode
	args:
		null 
	return:
		null
*/
void ssio_random_oneshot_start(void)
{
	unsigned int reg_val;
	reg_val = ss_read_w(SS_CTL);
	reg_val |= 0x1<<1;
	mutex_lock(&ss_mutex);
	ss_write_w(SS_CTL, reg_val);
	mutex_unlock(&ss_mutex);
}

/*
	name: ssio_key_select
		ssio setup random mode
	args:
		unsigned int key_number :0:ss_keyx 1:rkey 2:bkey 3-10:internal key
	return:
		null
*/
void ssio_key_select(unsigned int key_number)
{
	unsigned int reg_val;
	reg_val = ss_read_w(SS_CTL);
	reg_val &= ~(0xf<<24);
	reg_val |= (key_number<<24);
	mutex_lock(&ss_mutex);
	ss_write_w(SS_CTL, reg_val);
	mutex_unlock(&ss_mutex);
}

/* ****************************************************************************
//	unsigned int ss_get_rxfifo_room_size(void)
//  Description:	Get RX FIFO empty room in word unit
//								The 32 words RX FIFO is used for sending data (plaintext/ciphertext)
//                to SS engine
//	Arguments:		None
//
//
//	Return Value:	RX FIFO empry room size in word unit
*************************************************************************** */
unsigned int  ss_get_rxfifo_room_size(void)
{
	unsigned int  reg_val;

	reg_val = ss_read_w(SS_FCSR);
	reg_val = reg_val>>24;
	reg_val &= 0x3f;

	return reg_val;
}

/* ****************************************************************************
//	unsigned int   ss_get_txfifo_data_cnt(void)
//  Description:	Get TX FIFO available data counter in word unit
//								The 32 words TX FIFO is used for receiving data (plaintext/ciphertext)
//                from SS engine
//	Arguments:		None
//
//
//	Return Value:	TX FIFO available data counter in word unit
*************************************************************************** */
unsigned int  ss_get_txfifo_data_cnt(void)
{
	unsigned int   reg_val;

	reg_val = ss_read_w(SS_FCSR);
	reg_val = reg_val>>16;
	reg_val &= 0x3f;

	return reg_val;
}

/* ****************************************************************************
	void ss_send_data(unsigned int  data)
  Description:	
								Sending one word data (plaintext/ciphertext) to SS engine
	Arguments:		unsigned int  data	: data which will send


	Return Value:	void
***************************************************************************
*/
void ss_send_data(unsigned int data)
{
	mutex_lock(&ss_mutex);
	ss_write_w(SS_RXFIFO, data);
	mutex_unlock(&ss_mutex);
}

//*****************************************************************************
//	void ss_receive_data(unsigned int data)
//  Description:	
//								Sending one word data (plaintext/ciphertext) to SS engine
//	Arguments:		unsigned int* data : the point 	which point at 
//                
//  
//	Return Value:	void
//*****************************************************************************
void ss_receive_data(unsigned int* data)
{
	*data = ss_read_w(SS_TXFIFO);
}

/* ****************************************************************************
//	void ss_sha1md5_dataend(void)
//  Description:	
//								Set data end flag for SHA1/MD5 engine
//	Arguments:		None
//
//  
//	Return Value:	void
**************************************************************************** */
void ss_sha1md5_dataend(void)
{
	unsigned int reg_val;

	/*set end flag for SHA1/MD5*/
	reg_val = ss_read_w(SS_CTL);
	reg_val |= 0x1<<2;
	mutex_lock(&ss_mutex);
	ss_write_w(SS_CTL, reg_val);
	mutex_unlock(&ss_mutex);
	/*check whether SHA1/MD5 has finished operation*/
	while(ss_read_w(SS_CTL)&(0x1<<2)) {};
/*	for(i=0; i<0x10000; i++){};	delay for data end*/
}

/* ****************************************************************************
//	void ss_get_md(unsigned int* md_buf)
//  Description:	Get Message Digest (MD) from SHA1/MD5 engine
//	Arguments:		unsigned int* md_buf    Buffer for storing MD
//
//
//	Return Value:	void
*************************************************************************** */
void ss_get_md(unsigned int* md_buf)
{
		unsigned int i;
		
		for(i=0; i<5; i++)
			md_buf[i] = ss_read_w(SS_MD + i*4);
}

/* ****************************************************************************
//	void ss_send_text_to_sha1md5(unsigned int data_size, unsigned int total_size, unsigned int last, unsigned int* text)
//  Description:	Send text to SHA1/MD5 engine for generating message digest 
//                and padding_buf bits are added by this function
//	Arguments:		unsigned int data_size
//                       text number of text buffer (in byte unit, <=64 bytes)
//                       If it is not the last text, data_size should be 64.
//                unsigned int total_size	 
//                       total text number (in byte unit)
//                unsigned int last
//                       text last flag (0: not last text, 1: last text)
//                unsigned int* text
//                       pointer to text buffer
//
//	Return Value:	void
 ****************************************************************************/

unsigned int aw_endian4(unsigned int data)
{
	unsigned int d1, d2, d3, d4;
	d1= (data&0xff)<<24;
	d2= (data&0xff00)<<8;
	d3= (data&0xff0000)>>8;
	d4= (data&0xff000000)>>24;
	
	return (d1|d2|d3|d4);
}

void ss_send_text_to_sha1md5(unsigned int data_size, unsigned int total_size, unsigned int last, unsigned int* text)
{
	unsigned int i;
	unsigned int padding_buf[16];
	char *ptext, *ptext_in;
	unsigned int reg_val;
	unsigned int md5_flag;

	reg_val = ss_read_w(SS_CTL);
	reg_val >>= 4;
	reg_val &= 0x7;
	if(reg_val == 4)
		md5_flag = 1;
	else
		md5_flag = 0;

	if( (last == 1) && (data_size == 64) )
	{
		//send 512-bits text
		for(i=0; i<16; i++)
		{
			while(ss_get_rxfifo_room_size()== 0){};
			ss_send_data(text[i]);
		}
		//sending 512-bits padding_buf to SHA1/MD5
		for(i=0; i<16; i++)
			padding_buf[i] = 0x0;
		padding_buf[0] = 0x00000080;
		if(md5_flag)
		{
			padding_buf[14] = total_size<<3;
			padding_buf[15] = total_size>>29;
		}
		else
		{
			padding_buf[14] = total_size>>29;
			padding_buf[15] = total_size<<3;
			padding_buf[14] = aw_endian4(padding_buf[14]);
			padding_buf[15] = aw_endian4(padding_buf[15]);
		}
		for(i=0; i<16; i++)
		{
			while(ss_get_rxfifo_room_size()== 0){};
			ss_send_data(padding_buf[i]);
		}
	}
	else if( (last == 1) && (data_size < 56) )
	{
		//send text with padding_buf bits (total 512 bits) to SHA1/MD5
		for(i=0; i<16; i++)
			padding_buf[i] = 0x0;
		ptext = (char*)padding_buf;
		ptext_in = (char*)text;
		for(i=0; i<data_size; i++)
			ptext[i] = ptext_in[i];
		ptext[data_size] = 0x80;
		if(md5_flag)
		{
			padding_buf[14] = total_size<<3;
			padding_buf[15] = total_size>>29;
		}
		else
		{
			padding_buf[14] = total_size>>29;
			padding_buf[15] = total_size<<3;
			padding_buf[14] = aw_endian4(padding_buf[14]);
			padding_buf[15] = aw_endian4(padding_buf[15]);
		}
		for(i=0; i<16; i++)
		{
			while(ss_get_rxfifo_room_size()== 0){};
			ss_send_data(padding_buf[i]);
		}
	}
	else if( (last == 1) && (data_size >= 56) )
	{
		//send text with padding_buf to SHA1/MD5
		for(i=0; i<16; i++)
			padding_buf[i] = 0x0;
		ptext = (char*)padding_buf;
		ptext_in = (char*)text;
		for(i=0; i<data_size; i++)
			ptext[i] = ptext_in[i];
		ptext[data_size] = 0x80;
		for(i=0; i<16; i++)
		{
			while(ss_get_rxfifo_room_size()== 0){};
			ss_send_data(padding_buf[i]);
		}
		//send last 512-bits text to SHA1/MD5
		for(i=0; i<16; i++)
			padding_buf[i] = 0x0;
		if(md5_flag)
		{
			padding_buf[14] = total_size<<3;
			padding_buf[15] = total_size>>29;
		}
		else
		{
			padding_buf[14] = total_size>>29;
			padding_buf[15] = total_size<<3;
			padding_buf[14] = aw_endian4(padding_buf[14]);
			padding_buf[15] = aw_endian4(padding_buf[15]);
		}
		for(i=0; i<16; i++)
		{
			while(ss_get_rxfifo_room_size()== 0){};
			ss_send_data(padding_buf[i]);
		}
	}
	else //not last text
	{
		//send 512-bits text
		for(i=0; i<16; i++)
		{
			while(ss_get_rxfifo_room_size()== 0){};
			ss_send_data(text[i]);
		}
	}
}

void ss_sha1md5_text(unsigned int total_size, unsigned int* in)
{
	unsigned int i;
	unsigned int k, q;
	char* p_byte;

	k = total_size/64;
	q = total_size%64;
	p_byte = (char*)in;

	if(q==0)
	{
		for(i=0; i< (k-1); i++)
		{
			ss_send_text_to_sha1md5(64, total_size, 0, (unsigned int*)p_byte);
			p_byte +=64;
		}
		ss_send_text_to_sha1md5(64, total_size, 1, (unsigned int*)p_byte);
	}else{
		for(i=0; i< k; i++)
		{
			ss_send_text_to_sha1md5(64, total_size, 0, (unsigned int*)p_byte);
			p_byte +=64;
		}
		ss_send_text_to_sha1md5(q, total_size, 1, (unsigned int*)p_byte);
	}

}

unsigned int sha1md5_padding(unsigned int md5_flag, unsigned int data_size, char* text)
{
	unsigned int i;
	unsigned int k, q;
	unsigned int size;
	unsigned int padding_buf[16];
	char *ptext;	

	k = data_size/64;
	q = data_size%64;

	ptext = (char*)padding_buf;
	if(q==0)
	{
		for(i=0; i<16; i++)
			padding_buf[i] = 0x0;
		padding_buf[0] = 0x00000080;
		if(md5_flag)
		{
			padding_buf[14] = data_size<<3;
			padding_buf[15] = data_size>>29;
		}
		else
		{
			padding_buf[14] = data_size>>29;
			padding_buf[15] = data_size<<3;
			padding_buf[14] = aw_endian4(padding_buf[14]);
			padding_buf[15] = aw_endian4(padding_buf[15]);
		}
		for(i=0; i<64; i++)
			text[k*64 + i] = ptext[i];
		size = (k + 1)*64;
	}
	else if(q<56)
	{
		for(i=0; i<16; i++)
			padding_buf[i] = 0x0;
		for(i=0; i<q; i++)
			ptext[i] = text[k*64 + i];
		ptext[q] = 0x80;
		if(md5_flag)
		{
			padding_buf[14] = data_size<<3;
			padding_buf[15] = data_size>>29;
		}
		else
		{
			padding_buf[14] = data_size>>29;
			padding_buf[15] = data_size<<3;
			padding_buf[14] = aw_endian4(padding_buf[14]);
			padding_buf[15] = aw_endian4(padding_buf[15]);
		}
		for(i=0; i<64; i++)
			text[k*64 + i] = ptext[i];
		size = (k + 1)*64;
	}
	else
	{
		for(i=0; i<16; i++)
			padding_buf[i] = 0x0;
		for(i=0; i<q; i++)
			ptext[i] = text[k*64 + i];
		ptext[q] = 0x80;
		for(i=0; i<64; i++)
			text[k*64 + i] = ptext[i];

		//send last 512-bits text to SHA1/MD5
		for(i=0; i<16; i++)
			padding_buf[i] = 0x0;
		if(md5_flag)
		{
			padding_buf[14] = data_size<<3;
			padding_buf[15] = data_size>>29;
		}
		else
		{
			padding_buf[14] = data_size>>29;
			padding_buf[15] = data_size<<3;
			padding_buf[14] = aw_endian4(padding_buf[14]);
			padding_buf[15] = aw_endian4(padding_buf[15]);
		}
		for(i=0; i<64; i++)
			text[(k + 1)*64 + i] = ptext[i];
		size = (k + 2)*64;
	}

	return size;	
}

/* ****************************************************************************
//	void ss_check_randomend(void)
//  Description:	
//								Check whether random end in one shot mode
//	Arguments:		None
//
//
//	Return Value:	void
****************************************************************************
*/
void ss_check_randomend(void)
{
	/*check whether one group random data is OK in one shot mode*/
	while(ss_read_w(SS_CTL)&(0x1<<1)) {};
}

void ss_get_random(unsigned int* random_buf)
{
	unsigned int i;

	for(i=0; i<5; i++)
		random_buf[i] = ss_read_w(SS_MD + i*4);
}




static volatile int rx_dma_done_flag=0;
static volatile int tx_dma_done_flag=0;
void dma_buffdone_rx(dma_hdl_t dma_hdl, void *parg)
{
	SS_DEBUG("dma_buffdone_rx,size=%ld\n",*parg);
	rx_dma_done_flag = 1;
}

void dma_buffdone_tx(dma_hdl_t dma_hdl, void *parg)
{
	SS_DEBUG("dma_buffdone_tx,size=%ld\n",*parg);
	tx_dma_done_flag = 1;
}

unsigned int aes_des_dma_stable(struct ss_classdev *ss)
{
	int ret;
    dma_config_t ss_dma_cfg_rx;
    dma_config_t ss_dma_cfg_tx;

	SS_DEBUG("dma_mode \n");
	if(!ss->ss.in_buf)
		printk("in_buf none !\n");
	if(!ss->ss.out_buf)
		printk("out_buf none !\n");

	SS_DEBUG("in_buf  %p out_buf %p!\n",ss->ss.in_buf,ss->ss.out_buf);

	ss->ss.dma_hdl_rx = sw_dma_request(ss->ss.dma_para_rx.name, CHAN_DEDICATE);
	ss->ss.dma_hdl_tx = sw_dma_request(ss->ss.dma_para_tx.name, CHAN_DEDICATE);
    if (NULL == ss->ss.dma_hdl_rx) {
        printk(KERN_ERR "failed to request ss rx dma handle\n");
        return -EINVAL;
    }
    if (NULL == ss->ss.dma_hdl_tx) {
        printk(KERN_ERR "failed to request ss tx dma handle\n");
        return -EINVAL;
    }

    memset(&ss->ss.dma_cb_rx, 0, sizeof(ss->ss.dma_cb_rx));
    memset(&ss->ss.dma_cb_tx, 0, sizeof(ss->ss.dma_cb_tx));

    ss->ss.dma_cb_rx.func = dma_buffdone_rx;
    ss->ss.dma_cb_rx.parg = &ss->ss.in_length;
    ss->ss.dma_cb_tx.func = dma_buffdone_tx;
    ss->ss.dma_cb_tx.parg = &ss->ss.out_length;

    if (0 != sw_dma_ctl(ss->ss.dma_hdl_rx, DMA_OP_SET_FD_CB, (void *)&(ss->ss.dma_cb_rx))) {
		printk(KERN_ERR "failed to set ss dma buffer done!!!\n");
		sw_dma_release(ss->ss.dma_hdl_rx);
		return -EINVAL;
	}
    if (0 != sw_dma_ctl(ss->ss.dma_hdl_tx, DMA_OP_SET_FD_CB, (void *)&(ss->ss.dma_cb_tx))) {
		printk(KERN_ERR "failed to set ss dma buffer done!!!\n");
		sw_dma_release(ss->ss.dma_hdl_tx);
		return -EINVAL;
	}

    memset(&ss_dma_cfg_rx, 0, sizeof(ss_dma_cfg_rx));
    memset(&ss_dma_cfg_tx, 0, sizeof(ss_dma_cfg_tx));
    ss_dma_cfg_rx.xfer_type.src_data_width  = DATA_WIDTH_32BIT;
    ss_dma_cfg_rx.xfer_type.src_bst_len     = DATA_BRST_4;
    ss_dma_cfg_rx.xfer_type.dst_data_width  = DATA_WIDTH_32BIT;
    ss_dma_cfg_rx.xfer_type.dst_bst_len     = DATA_BRST_4;
    ss_dma_cfg_rx.address_type.src_addr_mode= DDMA_ADDR_LINEAR;
    ss_dma_cfg_rx.address_type.dst_addr_mode= DDMA_ADDR_IO;
    ss_dma_cfg_rx.src_drq_type              = D_SRC_SDRAM;
    ss_dma_cfg_rx.dst_drq_type              = D_DST_SS_TX;
    ss_dma_cfg_rx.bconti_mode               = false;
    ss_dma_cfg_rx.irq_spt                   = CHAN_IRQ_FD;
    if (0 != sw_dma_config(ss->ss.dma_hdl_rx, &ss_dma_cfg_rx)) {
        return -EINVAL;
    }

    ss_dma_cfg_tx.xfer_type.src_data_width  = DATA_WIDTH_32BIT;
    ss_dma_cfg_tx.xfer_type.src_bst_len     = DATA_BRST_4;
    ss_dma_cfg_tx.xfer_type.dst_data_width  = DATA_WIDTH_32BIT;
    ss_dma_cfg_tx.xfer_type.dst_bst_len     = DATA_BRST_4;
    ss_dma_cfg_tx.address_type.src_addr_mode= DDMA_ADDR_IO;
    ss_dma_cfg_tx.address_type.dst_addr_mode= DDMA_ADDR_LINEAR;
    ss_dma_cfg_tx.src_drq_type              = D_SRC_SS_RX;
    ss_dma_cfg_tx.dst_drq_type              = D_DST_SDRAM;
    ss_dma_cfg_tx.bconti_mode               = false;
    ss_dma_cfg_tx.irq_spt                   = CHAN_IRQ_FD;
    if (0 != sw_dma_config(ss->ss.dma_hdl_tx, &ss_dma_cfg_tx)) {
        return -EINVAL;
    }

	__cpuc_flush_dcache_area(ss->ss.in_buf, ss->ss.in_length + (1 << 5) * 2 - 2);
	__cpuc_flush_dcache_area(ss->ss.out_buf, ss->ss.out_length + (1 << 5) * 2 - 2);

	ret	=sw_dma_enqueue(ss->ss.dma_hdl_rx,(dma_addr_t)ss->ss.in_buf,(dma_addr_t)ss->ss.dma_para_rx.dma_addr,ss->ss.in_length);
    while(ret) {
        printk("sw_dma_enqueue err! \n");
        return -1;
    }
	ret	=sw_dma_enqueue(ss->ss.dma_hdl_tx,(dma_addr_t)ss->ss.dma_para_tx.dma_addr,(dma_addr_t)ss->ss.out_buf,ss->ss.out_length);
    while(ret) {
        printk("sw_dma_enqueue err! \n");
        return -1;
    }

	ssio_rx_trigger_level(1);
	ssio_tx_trigger_level(1);
	/*
	ssio_enable_dma();
	*/
	sw_dma_ctl(ss->ss.dma_hdl_rx,DMA_OP_START,NULL);
	sw_dma_ctl(ss->ss.dma_hdl_tx,DMA_OP_START,NULL);

#ifdef DEBUG_SS
	printk("loop before! \n");
#endif
	while(!rx_dma_done_flag);
	while(!tx_dma_done_flag);
#ifdef DEBUG_SS
	printk("loop after! \n");
#endif

	rx_dma_done_flag =0;
	tx_dma_done_flag =0;
	sw_dma_ctl(ss->ss.dma_hdl_rx,DMA_OP_STOP,NULL); /* first stop */
	sw_dma_ctl(ss->ss.dma_hdl_tx,DMA_OP_STOP,NULL); /* first stop */

	sw_dma_release(ss->ss.dma_hdl_rx);
    sw_dma_release(ss->ss.dma_hdl_tx);
	ssio_disable_dma();

	return 0;
}

unsigned int sha1_md5_dma_stable(struct ss_classdev *ss,unsigned long size,unsigned char *p_byte)
{
	int ret;
    dma_config_t ss_dma_cfg;
	printk("sha1_md5 dma_mode \n");
	if(!ss->ss.in_buf)
		printk("in_buf none !\n");

	SS_DEBUG("in_buf  %p!\n",ss->ss.in_buf);

	ss->ss.dma_hdl_rx = sw_dma_request(ss->ss.dma_para_rx.name, CHAN_DEDICATE);

    memset(&ss->ss.dma_cb_rx, 0, sizeof(ss->ss.dma_cb_rx));
    ss->ss.dma_cb_rx.func = dma_buffdone_rx;
    ss->ss.dma_cb_rx.parg = 0;

    if (0 != sw_dma_ctl(ss->ss.dma_hdl_rx, DMA_OP_SET_FD_CB, (void *)&(ss->ss.dma_cb_rx))) {
		printk(KERN_ERR "failed to set ss dma buffer done!!!\n");
		sw_dma_release(ss->ss.dma_hdl_rx);
		return -EINVAL;
	}

    memset(&ss_dma_cfg, 0, sizeof(ss_dma_cfg));
	ss_dma_cfg.xfer_type.src_data_width  = DATA_WIDTH_16BIT;
    ss_dma_cfg.xfer_type.src_bst_len     = DATA_BRST_4;
    ss_dma_cfg.xfer_type.dst_data_width  = DATA_WIDTH_16BIT;
    ss_dma_cfg.xfer_type.dst_bst_len     = DATA_BRST_4;
    ss_dma_cfg.address_type.src_addr_mode= NDMA_ADDR_INCREMENT;
    ss_dma_cfg.address_type.dst_addr_mode= NDMA_ADDR_NOCHANGE;
    ss_dma_cfg.src_drq_type              = N_SRC_SDRAM;
    ss_dma_cfg.dst_drq_type              = D_SRC_SS_RX;
    ss_dma_cfg.bconti_mode               = false;
    ss_dma_cfg.irq_spt                   = CHAN_IRQ_FD;
    if (0 != sw_dma_config(ss->ss.dma_hdl_rx, &ss_dma_cfg)) {
        return -EINVAL;
    }

	__cpuc_flush_dcache_area(p_byte,size + (1 << 5) * 2 - 2);

	ret	=sw_dma_enqueue(ss->ss.dma_hdl_rx,(dma_addr_t)p_byte,(dma_addr_t)ss->ss.dma_para_rx.dma_addr,ss->ss.out_length);
    while(ret) {
        printk("sw_dma_enqueue err! \n");
        return -1;
    }

	ssio_rx_trigger_level(1);
	sw_dma_ctl(ss->ss.dma_hdl_rx,DMA_OP_START,NULL);


	while(!rx_dma_done_flag);
	rx_dma_done_flag =0;
	
	sw_dma_ctl(ss->ss.dma_hdl_rx,DMA_OP_STOP,NULL);

	sw_dma_release(ss->ss.dma_hdl_rx);

	ssio_disable_dma();

	return 0;
}


unsigned int aes_stable(struct ss_classdev *ss)
{
	int i;
	unsigned int text;
	unsigned int tmp_ivec[5];
	if(ss->ss.out_buf)
		kfree(ss->ss.out_buf);
	ss->ss.out_buf =kmalloc(ss->ss.out_length, GFP_KERNEL);
	if(!ss->ss.out_buf){
		printk("kmalloc ss->ss.out_buf err!\n");
		return -1;
	}
	ssio_key_select(ss->ss.key_select);
	if(!ss->ss.key_select)
		ssio_set_key(userkey,ss->ss.bits);

	ss_setup_aes_des(ss->ss.ss_mode,ss->ss.de_en_crypt,ss->ss.bits,
	ss->ss.op_mode,ss->ss.cnt_size);
	
	if(ss->ss.op_mode == 1){
		for(i=0;i<ss->ss.iv_number /4;i++){
			tmp_ivec[i]	=	*(ivec + i*4);
			tmp_ivec[i]	|=	*(ivec + i*4 + 1) << 8;
			tmp_ivec[i]	|=	*(ivec + i*4 + 2) << 16;
			tmp_ivec[i]	|=	*(ivec + i*4 + 3) << 24;
			printk("tmp_ivec[%d] %x\n",i,tmp_ivec[i]);
		}
		ssio_set_iv(ss->ss.iv_number /4,tmp_ivec);
	}else if(ss->ss.op_mode == 2){
	printk("set cnt_size !\n");
		for(i=0;i<4;i++){
			tmp_ivec[i]	=	*(ivec + i*4);
			tmp_ivec[i]	|=	*(ivec + i*4 + 1) << 8;
			tmp_ivec[i]	|=	*(ivec + i*4 + 2) << 16;
			tmp_ivec[i]	|=	*(ivec + i*4 + 3) << 24;
		}
		ssio_set_cnt(ss->ss.cnt_size,tmp_ivec);
	}else printk("ecb mode !\n");
	if(ss->ss.cpu_dma)
		ssio_enable_dma();
		
	ssio_start();

	if(ss->ss.cpu_dma)
		aes_des_dma_stable(ss);
	else{
		printk("cpu_mode! \n");
		for(i=0;i< ss->ss.in_length/4;i++){
			text = *(ss->ss.in_buf + i*4);
			text |= *(ss->ss.in_buf + 1 + i*4) << 8;
			text |= *(ss->ss.in_buf + 2 + i*4) << 16;
			text |= *(ss->ss.in_buf + 3 + i*4) << 24;
			printk("text[%d]: 0x%x\n",i,text);
			ss_send_data(text);
		}

		for(i=0; i< ss->ss.out_length/4; i++){
			while(ss_get_txfifo_data_cnt()== 0){};
			ss_receive_data(&text);
			*(ss->ss.out_buf + i*4)= (char) (text & 0xff);
			*(ss->ss.out_buf + i*4 + 1)= (char) ((text >> 8) & 0xff);
			*(ss->ss.out_buf + i*4 + 2)= (char) ((text >> 16) & 0xff);
			*(ss->ss.out_buf + i*4 + 3)= (char) ((text >> 24) & 0xff);
		}
	}
	ssio_stop();
	ss_exit();
	return 0;
}

unsigned int des_stable(struct ss_classdev *ss)
{
	int i,k;
	unsigned int text;
	unsigned int tmp_ivec[5];
	if(ss->ss.out_buf)
		kfree(ss->ss.out_buf);
	ss->ss.out_buf =kmalloc(ss->ss.out_length, GFP_KERNEL);
	if(!ss->ss.out_buf){
		printk("kmalloc ss->ss.out_buf err!\n");
		return -1;
	}
	ssio_key_select(ss->ss.key_select);
	if(ss->ss.ss_mode == 2){	/*3DES mode*/
		k = 6;
	}else if(ss->ss.ss_mode == 1){	/*DES mode*/
		k = 2;
	}else {
		printk("ss_mode set err!\n");
		return -1;
	}
	if(!ss->ss.key_select)
		ssio_set_key(userkey,k * 32);
	ss_setup_aes_des(ss->ss.ss_mode,ss->ss.de_en_crypt,ss->ss.bits,
	ss->ss.op_mode,ss->ss.cnt_size);
	if(ss->ss.op_mode == 1){
		for(i=0;i<ss->ss.iv_number/4;i++){
			tmp_ivec[i]	=	*(ivec + i*4);
			tmp_ivec[i]	|=	*(ivec + i*4 + 1) << 8;
			tmp_ivec[i]	|=	*(ivec + i*4 + 2) << 16;
			tmp_ivec[i]	|=	*(ivec + i*4 + 3) << 24;
		}
		ssio_set_iv(ss->ss.iv_number/4,tmp_ivec);
	}else if(ss->ss.op_mode == 2){
		for(i=0;i<ss->ss.cnt_size/4;i++){
			tmp_ivec[i]	=	*(ivec + i*4);
			tmp_ivec[i]	|=	*(ivec + i*4 + 1) << 8;
			tmp_ivec[i]	|=	*(ivec + i*4 + 2) << 16;
			tmp_ivec[i]	|=	*(ivec + i*4 + 3) << 24;
		}
		ssio_set_cnt(ss->ss.cnt_size,tmp_ivec);
	}
	if(ss->ss.cpu_dma)
		ssio_enable_dma();
	ssio_start();
	if(ss->ss.cpu_dma)
		aes_des_dma_stable(ss);
	else{
		printk("cpu_mode!\n");
		for(i=0;i< ss->ss.in_length/4;i++){
			text = *(ss->ss.in_buf + i*4);
			text |= *(ss->ss.in_buf + 1 + i*4) << 8;
			text |= *(ss->ss.in_buf + 2 + i*4) << 16;
			text |= *(ss->ss.in_buf + 3 + i*4) << 24;
			SS_DEBUG("in_length text%d is %x\n",i,text);
			ss_send_data(text);
		}
		SS_DEBUG("out_length %ld in_length %ld\n",ss->ss.out_length,ss->ss.in_length);
		for(i=0; i< ss->ss.out_length/4; i++)
		{
			while(ss_get_txfifo_data_cnt()== 0){};
			ss_receive_data(&text);
			SS_DEBUG("out_length text%d is %x\n",i,text);
			*(ss->ss.out_buf + i*4)= (char) (text & 0xff);
			*(ss->ss.out_buf + i*4 + 1)= (char) ((text >> 8) & 0xff);
			*(ss->ss.out_buf + i*4 + 2)= (char) ((text >> 16) & 0xff);
			*(ss->ss.out_buf + i*4 + 3)= (char) ((text >> 24) & 0xff);
		}
	}
    ssio_stop();
    ss_exit();
    return 0;
}

/*unsigned int mode (0: SHA1, 1: MD5)*/
unsigned int sha1md5_stable(struct ss_classdev *ss,unsigned int mode)
{
	int i;
	unsigned int tmp_ivec[ss->ss.iv_number/4];
	unsigned int tmp_out[5];
	unsigned long tmp_size;
	unsigned char *p_byte;
	tmp_size = ss->ss.in_length / 64;
	tmp_size += 2;
	p_byte=kmalloc(tmp_size * 64, GFP_KERNEL);
	if(!p_byte){
		printk("kmalloc p_byte err!\n");
		return -1;
	}

	if(mode)
		ss->ss.out_length = 16;
	else
		ss->ss.out_length = 20;

	if(ss->ss.out_buf)
		kfree(ss->ss.out_buf);

	ss->ss.out_buf =kmalloc(ss->ss.out_length, GFP_KERNEL);

	if(!ss->ss.out_buf){
		printk("kmalloc ss->ss.out_buf err!\n");
		return -1;
	}
	ssio_setup_sha1_md5(mode, ss->ss.iv_mode);

	for(i=0;i< ss->ss.iv_number/4;i++){
		tmp_ivec[i]	=	*(ivec + i*4);
		tmp_ivec[i]	|=	*(ivec + i*4 + 1) << 8;
		tmp_ivec[i]	|=	*(ivec + i*4 + 2) << 16;
		tmp_ivec[i]	|=	*(ivec + i*4 + 3) << 24;
		}
		ssio_set_iv(ss->ss.iv_number/4,tmp_ivec);

	if(ss->ss.cpu_dma)
		ssio_enable_dma();
	ssio_start();
	if(ss->ss.cpu_dma){
		memcpy(p_byte,ss->ss.in_buf,ss->ss.in_length);
		tmp_size = sha1md5_padding(mode, ss->ss.in_length, p_byte);		
		sha1_md5_dma_stable(ss,tmp_size,p_byte);
	}else{
		ss_sha1md5_text(ss->ss.in_length, (unsigned int*)ss->ss.in_buf);
	}
		ss_sha1md5_dataend();
		ss_get_md(tmp_out);

		for(i=0; i< (ss->ss.out_length /4); i++){
			*(ss->ss.out_buf + i*4)		= tmp_out[i] & 0xff;
			*(ss->ss.out_buf + i*4+1)	= (tmp_out[i] >> 8) & 0xff;
			*(ss->ss.out_buf + i*4+2)	= (tmp_out[i] >> 16) & 0xff;
			*(ss->ss.out_buf + i*4+3)	= (tmp_out[i] >> 24) & 0xff;
			SS_DEBUG("tmp_out[%d] %x\n",i,tmp_out[i]);
		}
	ssio_stop();
	ss_exit();
	kfree(p_byte);
	return 0;
}
static struct class *ss_class;

static inline struct ss_classdev *pdev_to_ss(struct platform_device *dev)
{
	return platform_get_drvdata(dev);
}

static void ss_release (struct device *dev)
{
	SS_DEBUG("ss_release good !\n");
}

static ssize_t exec_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	return sprintf(buf,"start button!\n");
}

static ssize_t exec_store(struct device *dev,
        struct device_attribute *attr, const char *buf, size_t size)
{
	struct ss_classdev *ss = dev_get_drvdata(dev);
	char *after;
	int tmp = simple_strtoul(buf, &after, 10);
	int ret=0;
	switch(tmp){
		case 1:
			ret	= aes_stable(ss);/* aes*/
			break;
		case 2:
			ret	= des_stable(ss);/* des*/
			break;
		case 3:
			ret = sha1md5_stable(ss,0);/* sha*/
			break;
		case 4:
			ret = sha1md5_stable(ss,1);/* md5*/
			break;
		default:
			break;
	}
	if(ret){
		printk("take place err !\n");
	}
	return size;
}

static ssize_t in_show(
            struct file *filp,
            struct kobject *kobj, struct bin_attribute *a,
			char *buf, loff_t off, size_t count) 
{
	struct device *ssdev = container_of(kobj, struct device, kobj);
	struct ss_classdev *ss = dev_get_drvdata(ssdev);
	SS_DEBUG("in size is %ld \n",ss->ss.in_length);
	SS_DEBUG("count is %d \n",count);
	if(!ss->ss.in_buf){
		printk("in_buf no set!\n");
		return count;
	}
	
	memcpy(buf,ss->ss.in_buf,ss->ss.in_length);

	if(off + count > ss->ss.in_length)
		count= ss->ss.in_length - off;
	SS_DEBUG("count is %d \n",count);
	return count;
}

static ssize_t in_store(
            struct file *filp,
            struct kobject *kobj, struct bin_attribute *a,
            char *src, loff_t src_off, size_t size)
{
	struct device *ssdev = container_of(kobj, struct device, kobj);
	struct ss_classdev *ss = dev_get_drvdata(ssdev);

	if(buffsize < ss->ss.in_length){
		printk("the in_length too large!");
		return size;
	}

	if(ss->ss.in_buf)
		kfree(ss->ss.in_buf);
	ss->ss.in_buf=kmalloc(ss->ss.in_length, GFP_KERNEL);
	if(!ss->ss.in_buf){
		printk("kmalloc ss.in_buf err !\n");
		return size;
	}

	memset(ss->ss.in_buf,0,ss->ss.in_length);
	memcpy(ss->ss.in_buf,src,ss->ss.in_length);

	return size;
}

static ssize_t out_show(
            struct file *filp,
            struct kobject *kobj, struct bin_attribute *a,
             char *buf, loff_t off, size_t count) 
{
	struct device *ssdev = container_of(kobj, struct device, kobj);
	struct ss_classdev *ss = dev_get_drvdata(ssdev);
	SS_DEBUG("out size is %ld \n",ss->ss.out_length);
	
	memcpy(buf,ss->ss.out_buf,ss->ss.out_length);

	if(off + count > ss->ss.out_length)
		count= ss->ss.out_length - off;

	return count;
}

static ssize_t out_store(
            struct file *filp,
            struct kobject *kobj, struct bin_attribute *a,
            char *src, loff_t src_off, size_t size) 
{
	struct device *ssdev = container_of(kobj, struct device, kobj);
	struct ss_classdev *ss = dev_get_drvdata(ssdev);

	SS_DEBUG("out_length is %ld \n",ss->ss.out_length);
	SS_DEBUG("size is %d \n",size);
	if(buffsize < ss->ss.out_length){
		printk("the out_length too large!");
		return size;
	}
	memset(ss->ss.out_buf,0,ss->ss.out_length);
	memcpy(ss->ss.out_buf,src,ss->ss.out_length);

	return size;
}

static ssize_t key_show(
            struct file *filp,
            struct kobject *kobj, struct bin_attribute *a,
             char *buf, loff_t off, size_t count) 
{

	struct device *ssdev = container_of(kobj, struct device, kobj);
	struct ss_classdev *ss = dev_get_drvdata(ssdev);
	SS_DEBUG("keysize is %d \n",(ss->ss.bits / 8));
	
	memcpy(buf,userkey,(ss->ss.bits / 8));

	if(off + count > (ss->ss.bits / 8))
		count= (ss->ss.bits / 8) - off;

	return count;
}

static ssize_t key_store(
            struct file *filp,
            struct kobject *kobj, struct bin_attribute *a,
            char *src, loff_t src_off, size_t size) 
{
	struct device *ssdev = container_of(kobj, struct device, kobj);
	struct ss_classdev *ss = dev_get_drvdata(ssdev);

	SS_DEBUG("keysize is %d \n",(ss->ss.bits / 8));
	memset(userkey,0,32);
	memcpy(userkey,src,(ss->ss.bits / 8));

	return size;
}

static ssize_t iv_show(
            struct file *filp,
            struct kobject *kobj, struct bin_attribute *a,
             char *buf, loff_t off, size_t count) 
{

	struct device *ssdev = container_of(kobj, struct device, kobj);
	struct ss_classdev *ss = dev_get_drvdata(ssdev);
	int i;
	SS_DEBUG("iv_number is %d \n", (ss->ss.iv_number));
	for(i=0;i<4;i++){
		ivec[i*4]	= (char) (ss_read_w(SS_IV + 4*i) & 0xff);
		ivec[i*4+1]	= (char) ((ss_read_w(SS_IV + 4*i) >> 8 )& 0xff);
		ivec[i*4+2]	= (char) ((ss_read_w(SS_IV + 4*i) >> 16 )& 0xff);
		ivec[i*4+3]	= (char) ((ss_read_w(SS_IV + 4*i) >> 24 )& 0xff);
	}
		memcpy(buf,ivec,(ss->ss.iv_number));
	
	for(i=0;i<20;i++)
		printk("ivec[%d] 0x%2x\n",i,ivec[i]);
	if(off + count > (ss->ss.iv_number))
		count= (ss->ss.iv_number) - off;

	return count;
}

static ssize_t iv_store(
            struct file *filp,
            struct kobject *kobj, struct bin_attribute *a,
            char *src, loff_t src_off, size_t size) 
{
	struct device *ssdev = container_of(kobj, struct device, kobj);
	struct ss_classdev *ss = dev_get_drvdata(ssdev);
	SS_DEBUG("iv_number is %d \n",ss->ss.iv_number);
	memset(ivec,0,32);
	memcpy(ivec,src,(ss->ss.iv_number));
	return size;
}


static ssize_t bits_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct ss_classdev *ss = dev_get_drvdata(dev);
	return sprintf(buf,"%u\n",ss->ss.bits);
}

static ssize_t bits_store(struct device *dev,
        struct device_attribute *attr, const char *buf, size_t size)
{
	struct ss_classdev *ss = dev_get_drvdata(dev);
	ss->ss.bits =	*buf;
	ss->ss.bits |=	(*(buf+1)) << 8;
	ss->ss.bits |=	(*(buf+2)) << 16;
	ss->ss.bits |=	(*(buf+3)) << 24;
	SS_DEBUG("key bits is %d\n",ss->ss.bits);
	return size;
}

static ssize_t iv_number_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct ss_classdev *ss = dev_get_drvdata(dev);
	return sprintf(buf,"%u\n",ss->ss.iv_number);
}

static ssize_t iv_number_store(struct device *dev,
        struct device_attribute *attr, const char *buf, size_t size)
{
	struct ss_classdev *ss = dev_get_drvdata(dev);
	SS_DEBUG("iv_number is %d\n",*buf);
	ss->ss.iv_number= *buf;
	return size;
}

static ssize_t de_en_crypt_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct ss_classdev *ss = dev_get_drvdata(dev);
	return sprintf(buf,"%u\n",ss->ss.de_en_crypt);
}

static ssize_t de_en_crypt_store(struct device *dev,
        struct device_attribute *attr, const char *buf, size_t size)
{
	struct ss_classdev *ss = dev_get_drvdata(dev);
	SS_DEBUG("de_en_crypt is %d\n",*buf);
	ss->ss.de_en_crypt= *buf;
	return size;
}

static ssize_t key_select_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct ss_classdev *ss = dev_get_drvdata(dev);
	return sprintf(buf,"%u\n",ss->ss.key_select);
}

static ssize_t key_select_store(struct device *dev,
        struct device_attribute *attr, const char *buf, size_t size)
{
	struct ss_classdev *ss = dev_get_drvdata(dev);
	SS_DEBUG("key is select %d\n",*buf);
	ss->ss.key_select= *buf;
	return size;
}

static ssize_t ss_mode_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct ss_classdev *ss = dev_get_drvdata(dev);
	return sprintf(buf,"%u\n",ss->ss.ss_mode);
}

static ssize_t ss_mode_store(struct device *dev,
        struct device_attribute *attr, const char *buf, size_t size)
{
	struct ss_classdev *ss = dev_get_drvdata(dev);
	SS_DEBUG("ss_mode is %d\n",*buf);
	ss->ss.ss_mode= *buf;
	return size;
}

static ssize_t op_mode_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct ss_classdev *ss = dev_get_drvdata(dev);
	return sprintf(buf,"%u\n",ss->ss.op_mode);
}

static ssize_t op_mode_store(struct device *dev,
        struct device_attribute *attr, const char *buf, size_t size)
{
	struct ss_classdev *ss = dev_get_drvdata(dev);
	SS_DEBUG("op_mode is %d\n",*buf);
	ss->ss.op_mode= *buf;
	return size;
}

static ssize_t prng_mode_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct ss_classdev *ss = dev_get_drvdata(dev);
	return sprintf(buf,"%u\n",ss->ss.prng_mode);
}

static ssize_t prng_mode_store(struct device *dev,
        struct device_attribute *attr, const char *buf, size_t size)
{
	struct ss_classdev *ss = dev_get_drvdata(dev);
	SS_DEBUG("prng_mode is %d\n",*buf);
	ss->ss.prng_mode= *buf;
	return size;
}

static ssize_t iv_mode_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct ss_classdev *ss = dev_get_drvdata(dev);
	return sprintf(buf,"%u\n",ss->ss.iv_mode);
}

static ssize_t iv_mode_store(struct device *dev,
        struct device_attribute *attr, const char *buf, size_t size)
{
	struct ss_classdev *ss = dev_get_drvdata(dev);
	SS_DEBUG("iv_mode is %d\n",*buf);
	ss->ss.iv_mode= *buf;
	return size;
}

static ssize_t cnt_size_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct ss_classdev *ss = dev_get_drvdata(dev);
	return sprintf(buf,"%u\n",ss->ss.cnt_size);
}

static ssize_t cnt_size_store(struct device *dev,
        struct device_attribute *attr, const char *buf, size_t size)
{
	struct ss_classdev *ss = dev_get_drvdata(dev);
	printk("cnt_size is %d\n",*buf);
	ss->ss.cnt_size= *buf;
	return size;
}

static ssize_t cpu_dma_store(struct device *dev,
        struct device_attribute *attr, const char *buf, size_t size)
{
	struct ss_classdev *ss = dev_get_drvdata(dev);
	SS_DEBUG("cpu_dma is %d\n",*buf);
	ss->ss.cpu_dma= *buf;
	return size;
}
static ssize_t cpu_dma_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct ss_classdev *ss = dev_get_drvdata(dev);
	return sprintf(buf,"%u\n",ss->ss.cpu_dma);
}

static ssize_t in_length_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct ss_classdev *ss = dev_get_drvdata(dev);
	return sprintf(buf,"%lu\n",ss->ss.in_length);
}

static ssize_t in_length_store(struct device *dev,
        struct device_attribute *attr, const char *buf, size_t size)
{
	struct ss_classdev *ss = dev_get_drvdata(dev);
	ss->ss.in_length =	*buf;
	ss->ss.in_length |=	(*(buf+1)) << 8;
	ss->ss.in_length |=	(*(buf+2)) << 16;
	ss->ss.in_length |=	(*(buf+3)) << 24;
    SS_DEBUG("in_length is %ld\n",ss->ss.in_length);
	return size;
}

static ssize_t out_length_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct ss_classdev *ss = dev_get_drvdata(dev);
	return sprintf(buf,"%lu\n",ss->ss.out_length);
}

static ssize_t out_length_store(struct device *dev,
        struct device_attribute *attr, const char *buf, size_t size)
{
	struct ss_classdev *ss = dev_get_drvdata(dev);
	ss->ss.out_length =		*buf;
	ss->ss.out_length |=	(*(buf+1)) << 8;
	ss->ss.out_length |=	(*(buf+2)) << 16;
	ss->ss.out_length |=	(*(buf+3)) << 24;
    SS_DEBUG("out_length is %ld\n",ss->ss.out_length);
	return size;
}

static struct device_attribute ss_class_attrs[] = {
	__ATTR(exec,0644,exec_show,exec_store),
	__ATTR(bits,0644,bits_show,bits_store),
	__ATTR(iv_number,0644,iv_number_show,iv_number_store),
	__ATTR(de_en_crypt,0644,de_en_crypt_show,de_en_crypt_store),
	__ATTR(key_select,0644,key_select_show,key_select_store),
	__ATTR(in_length,0644,in_length_show,in_length_store),
	__ATTR(out_length,0644,out_length_show,out_length_store),
	__ATTR(ss_mode,0644,ss_mode_show,ss_mode_store),
	__ATTR(op_mode,0644,op_mode_show,op_mode_store),
	__ATTR(prng_mode,0644,prng_mode_show,prng_mode_store),
	__ATTR(iv_mode,0644,iv_mode_show,iv_mode_store),
	__ATTR(cnt_size,0644,cnt_size_show,cnt_size_store),
	__ATTR(cpu_dma,0644,cpu_dma_show,cpu_dma_store),
	__ATTR_NULL,
};

static struct bin_attribute ss_class_buff_attrs[] ={
	{
		.attr = {
			.name = "in_buf",
			.mode = 0644,
		},
		.size = 1024 * 1024,
		.read = in_show,
		.write= in_store,
	},
	{
		.attr = {
			.name = "out_buf",
			.mode = 0644,
		},
		.size = 1024 * 1024,
		.read = out_show,
		.write= out_store,
	},
	{
		.attr = {
			.name = "key",
			.mode = 0644,
		},
		.size = 32,
		.read = key_show,
		.write= key_store,
	},
	{
		.attr = {
			.name = "iv",
			.mode = 0644,
		},
		.size = 32,
		.read = iv_show,
		.write= iv_store,
	},
};

static int __devexit ss_remove(struct platform_device *dev)
{
    struct ss_classdev *ss_dev    = platform_get_drvdata(dev);
	ss_sys_close();
    device_unregister(ss_dev->dev);
	if(ss_dev->ss.in_buf)
		kfree(ss_dev->ss.in_buf);
	if(ss_dev->ss.out_buf)
		kfree(ss_dev->ss.out_buf);
    kfree(ss_dev);
    SS_DEBUG("kfree ok!");
    return 0;
}

static int __devinit ss_probe(struct platform_device *dev)
{
	struct ss_classdev *ss_dev;
	int i,ret;
	ss_dev = kzalloc(sizeof(struct ss_classdev), GFP_KERNEL);
	if (ss_dev == NULL) {
		dev_err(&dev->dev, "No memory for device\n");
		return -ENOMEM;
	}

	platform_set_drvdata(dev, ss_dev);

	ss_dev->ss.in_buf	=	NULL;
	ss_dev->ss.out_buf	=	NULL;

    ss_dev->ss.dma_para_tx = ss_dma_para_tx;
    ss_dev->ss.dma_para_rx = ss_dma_para_rx;
	ss_dev->dev = device_create(ss_class, &dev->dev, 0, ss_dev,"ss");
	for(i=0; i< ARRAY_SIZE(ss_class_buff_attrs); i++){
		ret = device_create_bin_file(&dev->dev, &ss_class_buff_attrs[i]);
		if(ret){
			printk("device_create_bin_file[%d] fail\n",i);
		}
	}
	if (IS_ERR(ss_dev->dev)){
		kfree(ss_dev);
		return PTR_ERR(ss_dev->dev);
	}
	ss_sys_open(1);
	return 0;
}

static struct platform_driver ss_driver = {
	.probe		= ss_probe,
	.remove		= ss_remove,
	.driver		= {
		.name		= "sunxi_ss",
		.owner		= THIS_MODULE,
	},
};
static struct platform_device ss_sunxi = {
    .name   = "sunxi_ss",
    .dev    = {
        .release    = ss_release,
    },
};

static int __init ssystem_init(void)
{
	ss_class = class_create(THIS_MODULE, "security_ss");
	if (IS_ERR(ss_class))
		return PTR_ERR(ss_class);
	ss_class->dev_attrs 	= ss_class_attrs;
	mutex_init(&ss_mutex);
	platform_device_register(&ss_sunxi);
	platform_driver_register(&ss_driver);
	return 0;
}

static void __exit ssystem_exit(void)
{
	mutex_destroy(&ss_mutex);
	platform_driver_unregister(&ss_driver);
	platform_device_unregister(&ss_sunxi);
	class_destroy(ss_class);
}

module_init(ssystem_init);
module_exit(ssystem_exit);
MODULE_AUTHOR("allwinner");
MODULE_DESCRIPTION("security_system driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:security_system");
