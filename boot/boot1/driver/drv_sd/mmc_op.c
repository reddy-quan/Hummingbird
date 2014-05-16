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
#include "mmc_def.h"
#include "mmc.h"

static unsigned bootcard_offset=0;

extern int sunxi_mmc_init(int sdc_no, unsigned bus_width);
extern int sunxi_mmc_exit(int sdc_no);

__s32 SDMMC_PhyInit(__u32 card_no, __u32 bus_width)
{
	int ret = 0;
	ret = sunxi_mmc_init(card_no, bus_width);
	if ( ret <= 0) {
		mmcdbg("Init SD/MMC card failed !\n");
		return -1;
	}

	return 0;
}

__s32 SDMMC_PhyExit(__u32 card_no)
{
	sunxi_mmc_exit(card_no);
    return 0;
}

__s32 SDMMC_PhyRead(__u32 start_sector, __u32 nsector, void *buf, __u32 card_no)
{
	__u32 sector;
	sector = mmc_bread(card_no, start_sector, nsector, buf);
    if(sector!=nsector)
       return -1;
    else
       return 0;
}

__s32 SDMMC_PhyWrite(__u32 start_sector, __u32 nsector, void *buf, __u32 card_no)
{
	 __u32 sector;
	sector = mmc_bwrite(card_no, start_sector, nsector, buf);
    if(sector!=nsector)
       return -1;
    else
       return 0;
}

__s32 SDMMC_PhyDiskSize(__u32 card_no)
{
	struct mmc *mmc = find_mmc_device(card_no);
	return mmc->lba;
}

__s32 SDMMC_PhyErase(__u32 block, __u32 nblock, __u32 card_no)
{
	 __u32 sector;
	 sector = mmc_berase(card_no, block, nblock);
     if(sector!=nblock)
        return -1;
     else
        return 0;
}

__s32 SDMMC_LogicalInit(__u32 card_no, __u32 card_offset, __u32 bus_width)
{
	bootcard_offset = card_offset;
    return SDMMC_PhyInit(card_no, bus_width);
}

__s32 SDMMC_LogicalExit(__u32 card_no)
{
	bootcard_offset = 0;
    return SDMMC_PhyExit(card_no);
}

__s32 SDMMC_LogicalRead(__u32 start_sector, __u32 nsector, void *buf, __u32 card_no)
{
	 __u32 sector;
     sector=mmc_bread(card_no, start_sector + bootcard_offset, nsector, buf);
     if(sector!=nsector)
       return -1;
    else
       return 0;
}

__s32 SDMMC_LogicalWrite(__u32 start_sector, __u32 nsector, void *buf, __u32 card_no)
{
     __u32 sector;
	sector= mmc_bwrite(card_no, start_sector + bootcard_offset, nsector, buf);
    if(sector!=nsector)
       return -1;
    else
       return 0;
}

__s32 SDMMC_LogicalDiskSize(__u32 card_no)
{
	return SDMMC_PhyDiskSize(card_no) - bootcard_offset;
}

__s32 SDMMC_LogicaErase(__u32 block, __u32 nblock, __u32 card_no)
{
    __u32 sector;
	sector = mmc_berase(card_no, block + bootcard_offset, nblock);
    if(sector!=nblock)
       return -1;
    else
       return 0;
}

void OSAL_SDCARD_CacheRangeFlush(void*Address, __u32 Length, __u32 Flags)
{
    wlibc_CleanFlushDCacheRegion(Address,Length);
	
}

