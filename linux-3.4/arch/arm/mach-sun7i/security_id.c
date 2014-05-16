/*
 * arch/arm/mach-sun7i/security_id.c
 * (C) Copyright 2010-2015
 * Reuuimlla Technology Co., Ltd. <www.reuuimllatech.com>
 * liugang <liugang@reuuimllatech.com>
 *
 * security id module
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/kdev_t.h>
#include <linux/clk.h>
#include <linux/io.h>
#include <linux/slab.h>
#include <linux/miscdevice.h>

#include <mach/includes.h>

/* to fix, 2013-1-15 */
enum sw_ic_ver sw_get_ic_ver(void)
{
	u32 val;
	enum sw_ic_ver version = MAGIC_VER_NULL;

	pr_info("%s(%d) err: to fix\n", __func__, __LINE__);
	if(version != MAGIC_VER_NULL)
		return version;

	/* gating ahb for ss */
	val = readl(SW_VA_CCM_IO_BASE + 0x60);
	val |= 1<<5;
	writel(val, SW_VA_CCM_IO_BASE + 0x60);
	/* gating special clk for ss */
	val = readl(SW_VA_CCM_IO_BASE + 0x9c);
	val |= 1<<31;
	writel(val, SW_VA_CCM_IO_BASE + 0x9c);

	val = readl(SW_VA_SS_IO_BASE);
	switch((val>>16)&0x07) { /* bit16~18, die bonding id */
		case 0:
			val = readl(SW_VA_SID_IO_BASE+0x08); /* sid root key2 reg */
			val = (val>>12) & 0x0f;
			if((val == 0x3) || (val == 0)) {
				val = readl(SW_VA_SID_IO_BASE+0x00); /* sid root key0 reg */
				val = (val>>8)&0xffffff;
				if((val == 0x162541) || (val == 0))
					version = MAGIC_VER_A12A;
				else if(val == 0x162542)
					version = MAGIC_VER_A12B;
				else
					version = MAGIC_VER_UNKNOWN;
			} else if(val == 0x07) {
				val = readl(SW_VA_SID_IO_BASE+0x00);
				val = (val>>8)&0xffffff;
				if((val == 0x162541) || (val == 0))
					version = MAGIC_VER_A10SA;
				else if(val == 0x162542)
					version = MAGIC_VER_A10SB;
				else
					version = MAGIC_VER_UNKNOWN;
			} else
				version = MAGIC_VER_UNKNOWN;
			break;
		case 1:
			val = readl(SW_VA_SID_IO_BASE+0x00); /* sid root key0 reg */
			val = (val>>8)&0xffffff;
			if((val == 0x162541) || (val == 0x162565) || (val == 0))
				version = MAGIC_VER_A13A;
			else if(val == 0x162542)
				version = MAGIC_VER_A13B;
			else
				version = MAGIC_VER_UNKNOWN;
			break;
		default:
			version = MAGIC_VER_UNKNOWN;
			break;
	}

	return version;
}
EXPORT_SYMBOL(sw_get_ic_ver);

int sw_get_chip_id(struct sw_chip_id *chip_id)
{
	chip_id->sid_rkey0 = readl(SW_VA_SID_IO_BASE);
	chip_id->sid_rkey1 = readl(SW_VA_SID_IO_BASE+0x04);
	chip_id->sid_rkey2 = readl(SW_VA_SID_IO_BASE+0x08);
	chip_id->sid_rkey3 = readl(SW_VA_SID_IO_BASE+0x0C);

	return 0;
}
EXPORT_SYMBOL(sw_get_chip_id);

unsigned int _hex2dec(unsigned int hex)
{
    unsigned int dec;

    if (hex >= 0x48 && hex <= 0x49) {
        dec = hex - 0x48;
    } else if (hex >= 0x50 && hex <= 0x57) {
        dec = hex - 0x50 + 2;
    } else if (hex >= 0x65 && hex <= 0x69) {
        dec = hex - 0x65 + 10;
    } else if (hex >= 0x70 && hex <= 0x79) {
        dec = hex - 0x70 + 15;
    } else if (hex >= 0x80 && hex <= 0x89) {
        dec = hex - 0x80 + 25;
    } else if (hex == 0x90) {
        dec = 36;
    } else {
        pr_err("something wrong in chip id\n");
        dec = 0;
    }

    return dec;
}

/**
 * Get 19bit hex value of chip id. The following bit is valid.
 * sid_rkey0[0:31], 8bit
 * sid_rkey1[0:31], 8bit
 * sid_rkey2[0:2], 3bit
 */
int sw_get_chip_id2(struct sw_chip_id *chip_id)
{
	unsigned int sid_rkey0;
	unsigned int sid_rkey1;
	unsigned int sid_rkey2;
	unsigned int sid_rkey3;

	sid_rkey0 = readl(SW_VA_SID_IO_BASE);
	sid_rkey1 = readl(SW_VA_SID_IO_BASE+0x04);
	sid_rkey2 = readl(SW_VA_SID_IO_BASE+0x08);
	sid_rkey3 = readl(SW_VA_SID_IO_BASE+0x0c);

    if (sid_rkey0 == 0 && sid_rkey1 == 0 &&
        sid_rkey2 == 0 && sid_rkey3 == 0) {
        chip_id->sid_rkey0 = 0;
        chip_id->sid_rkey1 = 0;
        chip_id->sid_rkey2 = 0;
        chip_id->sid_rkey3 = 0;
        return 0;
    }

    chip_id->sid_rkey0 = sid_rkey3;

    chip_id->sid_rkey1 = _hex2dec((sid_rkey1 >> 24) & 0xff);
    chip_id->sid_rkey1 |= _hex2dec((sid_rkey1 >> 16) & 0xff) << 6;
    chip_id->sid_rkey1 |= _hex2dec((sid_rkey1 >> 8) & 0xff) << 12;
    chip_id->sid_rkey1 |= _hex2dec(sid_rkey1 & 0xff) << 18;
    chip_id->sid_rkey1 |= _hex2dec((sid_rkey2 >> 24) & 0xff) << 24;
    chip_id->sid_rkey1 |= (_hex2dec((sid_rkey2 >> 16) & 0xff) & 0x03) << 30;

    chip_id->sid_rkey2 = _hex2dec((sid_rkey2 >> 16) & 0xff) >> 2;
    chip_id->sid_rkey2 |= (sid_rkey0 & 0xff) << 4;

	return 0;
}
EXPORT_SYMBOL(sw_get_chip_id2);

