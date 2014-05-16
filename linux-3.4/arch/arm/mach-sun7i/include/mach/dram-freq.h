/*
 * arch/arm/mach-sun7i/include/mach/dram-freq.h
 *
 * Copyright (C) 2013 - 2017 Reuuimlla Limited
 * xie chuanrong <xiechr@reuuimllatech.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 */

#ifndef __DRAM_FREQ_H__
#define __DRAM_FREQ_H__

#define SRAM_MDFS_START         (0xf0000000)
#define DRAM_SUSPEND_TIME       5000
#define DRAM_LOWER_FREQ         0
#define DRAM_NORMAL_FREQ        1
#define DRAM_USBLOCK_FREQ       1

#define CCM_AHB_REG     (IO_ADDRESS(AW_CCM_BASE) + 0x54)
#define CONFIG_DRAM_FREQ_DVFS

extern int cpu_hotplug_set_lock(int num_core);

#ifdef CONFIG_DRAM_FREQ_DVFS
#define HOTPLUG_DVFS_TABLE_LENGTH (8)
struct dramfreq_dvfs {
    unsigned int    dram_freq;       /* dram frequency, based on KHz */
    unsigned int    volt;            /* voltage for the frequency, based on mv */
    unsigned int    ahb_freq;        /* ahb freq */
};
#endif 


extern char *dfreq_bin_start;
extern char *dfreq_bin_end;

#define DRAMFREQ_NOTIFY_PREPARE		0x00000001 /* device added */
#define DRAMFREQ_NOTIFY_DONE		0x00000002 /* device removed */

#ifdef CONFIG_SW_DRAM_FREQ
/*
 * register function to be called at dram freq setting.
 */
extern int dramfreq_register_notifier(struct notifier_block *nb);

extern int dramfreq_unregister_notifier(struct notifier_block *nb);
#else
static inline dramfreq_register_notifier(struct notifier_block *nb)
{
    return 0;
}
static inline dramfreq_unregister_notifier(struct notifier_block *nb)
{
    return 0;
}
#endif

#endif /* __DRAM_FREQ_H__ */
