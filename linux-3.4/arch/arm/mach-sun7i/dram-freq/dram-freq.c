/*
 * arch/arm/mach-sun7i/dram-freq/dram-freq.c
 *
 * Copyright (C) 2012 - 2016 Reuuimlla Limited
 * xiechuanrong <xiechr@reuuimllatech.com>
 *
 * sun7i dram frequency dynamic scaling driver
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 */

#include <linux/io.h>
#include <linux/slab.h>
#include <linux/mutex.h>
#include <linux/suspend.h>
#include <linux/platform_device.h>
#include <linux/module.h>
#include <linux/clk.h>
#include <linux/regulator/consumer.h>
#include <mach/clock.h>
#include <mach/system.h>
#include <mach/sys_config.h>
#include <linux/cpu.h>
#include <linux/earlysuspend.h>
#include <../pm/pm.h>
#include <mach/powernow.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/hardirq.h>
#include <mach/dram-freq.h>

typedef struct __dramfreq_info {
    standy_dram_para_t dram_para;  /* dram para, using for dram freq setting*/
    struct delayed_work work;      /* delayed work to setting dram while early suspend startting */
    spinlock_t spin_lock;
    unsigned int cur_freq;
    unsigned int target_freq;
    unsigned int mstime;
    unsigned int usb_status;
    struct mutex mutex;
    struct clk *clk_ddr;    /* ahb clock handler */
#ifdef CONFIG_DRAM_FREQ_DVFS
    unsigned int table_length;
    struct dramfreq_dvfs dvfs_table[HOTPLUG_DVFS_TABLE_LENGTH];
    unsigned int last_vdd; 
    struct regulator *sysvdd;
    struct clk *clk_ahb;    /* ahb clock handler */
#endif 
}dramfreq_info_t;

struct workqueue_struct *dram_freq_workqueue;
static dramfreq_info_t dram_info = {
    .cur_freq = 1,
    .target_freq = 1,
    .usb_status = 0,
    .mstime = DRAM_SUSPEND_TIME,
};

#if 0
#define DRAMFREQ_DBG(format,args...)   printk("[dram_freq]:"format,##args)
#else
#define DRAMFREQ_DBG(format,args...)   
#endif

BLOCKING_NOTIFIER_HEAD(dramfreq_notifier_list);

/*
 * register function to be called at sw_powernow mode changed.
 */
int dramfreq_register_notifier(struct notifier_block *nb)
{
	return blocking_notifier_chain_register(&dramfreq_notifier_list, nb);
}
EXPORT_SYMBOL(dramfreq_register_notifier);

int dramfreq_unregister_notifier(struct notifier_block *nb)
{
	return blocking_notifier_chain_unregister(&dramfreq_notifier_list, nb);
}
EXPORT_SYMBOL(dramfreq_unregister_notifier);


#ifdef CONFIG_DRAM_FREQ_DVFS
static int __init_dvfs_syscfg(dramfreq_info_t *pdram_info)
{
    int i, ret = 0;
    char name[16] = {0};
    script_item_u val;
    script_item_value_type_e type;

    DRAMFREQ_DBG("%s %d!__init_dvfs_syscfg\n",__FILE__,__LINE__);
    type = script_get_item("dram_dvfs_table", "LV_count", &val);
    if (SCIRPT_ITEM_VALUE_TYPE_INT != type) {
        DRAMFREQ_DBG( "fetch LV_count from sysconfig failed\n");
        ret = -1;
        goto EXIT;
    }
    DRAMFREQ_DBG("LV_count value is %x\n", val.val);
    pdram_info->table_length = val.val;

    /* table_length_syscfg must be < TABLE_LENGTH */
    if(pdram_info->table_length > HOTPLUG_DVFS_TABLE_LENGTH){
        printk(KERN_ERR "LV_count from sysconfig is out of bounder\n");
        ret = -1;
        goto EXIT;
    }

    for (i = 1; (i <= pdram_info->table_length) && (ret == 0); i++){
        sprintf(name, "LV%d_freq", i);
        type = script_get_item("dram_dvfs_table", name, &val);
        if (SCIRPT_ITEM_VALUE_TYPE_INT != type) {
            DRAMFREQ_DBG("get LV%d_freq from sysconfig failed\n", i);
            ret = -1;
        }
        pdram_info->dvfs_table[i-1].dram_freq = val.val / 1000;

        sprintf(name, "LV%d_volt", i);
        type = script_get_item("dram_dvfs_table", name, &val);
        if (SCIRPT_ITEM_VALUE_TYPE_INT != type) {
            DRAMFREQ_DBG("get LV%d_volt from sysconfig failed\n", i);
            ret = -1;
        }
        pdram_info->dvfs_table[i-1].volt = val.val;
        
        sprintf(name, "LV%d_ahb", i);
        type = script_get_item("dram_dvfs_table", name, &val);
        if (SCIRPT_ITEM_VALUE_TYPE_INT != type) {
            DRAMFREQ_DBG("get LV%d_ahb from sysconfig failed\n", i);
            ret = -1;
        }
        pdram_info->dvfs_table[i-1].ahb_freq = val.val;
    }

EXIT:
    /* if get para from sysconfig err, use default setting*/
    DRAMFREQ_DBG("%s %d!ret:%d\n",__FILE__,__LINE__,ret);
    if (ret){
        pdram_info->table_length = 2;
        pdram_info->dvfs_table[0].dram_freq = 1;
        pdram_info->dvfs_table[0].volt = 1250;
        pdram_info->dvfs_table[0].ahb_freq = 150000000;
        
        pdram_info->dvfs_table[1].dram_freq = 0;
        pdram_info->dvfs_table[1].volt = 1100;
        pdram_info->dvfs_table[1].ahb_freq = 75000000;
    }
    DRAMFREQ_DBG("%s %d!table_length:%d\n",__FILE__,__LINE__,pdram_info->table_length);
    if (pdram_info->dvfs_table[pdram_info->table_length].dram_freq != 0){
        pdram_info->dvfs_table[pdram_info->table_length].dram_freq = 0;
    }
    return 0;
}


static inline unsigned int get_dvfs_config(unsigned int freq)
{
    struct dramfreq_dvfs *dvfs_inf = NULL;
    int index = 0;
    dvfs_inf = &dram_info.dvfs_table[0];

    while(dvfs_inf[index + 1].dram_freq >= freq && index + 1 < dram_info.table_length) {
        index++;
    }
    return index;
}
#endif

static int __dram_get_cur(void)
{
    unsigned long ddr_freq_point = dram_info.cur_freq;
    if (dram_info.clk_ddr){
        ddr_freq_point = clk_get_rate(dram_info.clk_ddr);
        DRAMFREQ_DBG("ddr_freq_point:%ld\n",ddr_freq_point);
        if (ddr_freq_point <= 240000000){
            ddr_freq_point = 0;
        }else{
            ddr_freq_point = 1;
        }
    }
    return ddr_freq_point;
}

/**
 *  freq_or_div:
 *  dfreq: freq div
 *  soft_switch: target freq
 */
extern int sw_pause_ehci(__u32 usbc_no);
extern int sw_restart_ehci(__u32 usbc_no);
extern __u32 pm_enable_watchdog(void);
extern void pm_disable_watchdog(__u32 dogMode);
unsigned int dram_setting_counter = 0;
static int __dramfreq_set(unsigned int dram_freq_ponit)
{
    int (*soft_switch_main)(unsigned int freq_point, standy_dram_para_t *pdram_para, void*);
    unsigned long flags;
    unsigned long long getfreq_time_usecs = 0;
    ktime_t calltime, delta, rettime;
    int index = 0;
    int new_vdd, last_vdd;
    int dogmod = 0;
    
    DRAMFREQ_DBG("%s %d!\n",__FILE__,__LINE__);
    if (__dram_get_cur() == dram_freq_ponit){
        return 0;
    }
    
#ifdef CONFIG_DRAM_FREQ_DVFS
    index = get_dvfs_config(dram_freq_ponit);
    new_vdd = dram_info.dvfs_table[index].volt;
    last_vdd = dram_info.last_vdd;
    if (dram_info.sysvdd && (new_vdd > last_vdd)) {
        int ret = -1;
        int counts = 5;
        while(counts > 0 && ret != 0){
            ret = regulator_set_voltage(dram_info.sysvdd, new_vdd*1000, new_vdd*1000);
        }
        if (ret < 0){
            new_vdd = last_vdd;
        }
        dram_info.last_vdd = new_vdd;
    }
    if (dram_info.clk_ahb) {
        clk_set_rate(dram_info.clk_ahb, dram_info.dvfs_table[index].ahb_freq);
    }
#endif
    soft_switch_main = (void *)SRAM_MDFS_START;

    /* move code to sram */
    memcpy((void *)SRAM_MDFS_START, (void *)&dfreq_bin_start, (int)&dfreq_bin_end - (int)&dfreq_bin_start);

    spin_lock_irqsave(&dram_info.spin_lock, flags);
    dogmod = pm_enable_watchdog();
    calltime = ktime_get();

    start_switch_dram:
    soft_switch_main(dram_freq_ponit, &dram_info.dram_para, (void *)&&start_switch_dram);
    dram_info.cur_freq = dram_freq_ponit;

    rettime = ktime_get();
    delta = ktime_sub(rettime, calltime);
    getfreq_time_usecs = ktime_to_ns(delta) >> 10;

    pm_disable_watchdog(dogmod);
    spin_unlock_irqrestore(&dram_info.spin_lock, flags);

#ifdef CONFIG_DRAM_FREQ_DVFS
    if (dram_info.clk_ahb) {
        clk_set_rate(dram_info.clk_ahb, dram_info.dvfs_table[index].ahb_freq);
    }
    if (dram_info.sysvdd && (new_vdd < last_vdd)) {
        int ret = -1;
        int counts = 5;
        while(counts > 0 && ret != 0){
            ret = regulator_set_voltage(dram_info.sysvdd, new_vdd*1000, new_vdd*1000);
            counts--;
        }
        if (ret < 0) {
            new_vdd = last_vdd;
        }
        dram_info.last_vdd = new_vdd;
    }
#endif
    dram_setting_counter++;
    printk("[switch time]: %Ld usecs, counter:%d\n", getfreq_time_usecs, dram_setting_counter);

    return 0;
}

static ssize_t dram_freq_show(struct device *dev, struct device_attribute *attr,
            char *buf)
{
    return 0;
}

static DEVICE_ATTR(dram_freq, S_IRUGO, dram_freq_show, NULL);

static const struct attribute *dramfreq_attrib[] = {
    &dev_attr_dram_freq.attr,
    NULL
};

static void do_dramfreq_set(struct work_struct *work)
{
    dramfreq_info_t *dram_info;
    unsigned int target_freq;

    dram_info = container_of(work, dramfreq_info_t, work.work);
    if (!dram_info->usb_status){
        target_freq = dram_info->target_freq;
    }else if (dram_info->usb_status){
        target_freq = DRAM_USBLOCK_FREQ;
    }
    else{
        goto exit;
    }
    DRAMFREQ_DBG("%s:%s start freq:%d\n", __FILE__, __func__, dram_info->cur_freq);
    if (__dram_get_cur() == target_freq){
        goto exit;
    }
    if (in_interrupt()){
        //queue_delayed_work(dram_freq_workqueue, &dram_info.work, msecs_to_jiffies(2));
        queue_work(dram_freq_workqueue, &dram_info->work.work);
        DRAMFREQ_DBG("warning: setting dram freq in irq and cpu1 is online\n");
        goto exit;
    }
    mutex_lock(&dram_info->mutex);
    cpu_hotplug_set_lock(1);
    while(num_online_cpus() != 1){
        msleep(10);
    }
    sw_pause_ehci(1);
    sw_pause_ehci(2);
    blocking_notifier_call_chain(&dramfreq_notifier_list,
            DRAMFREQ_NOTIFY_PREPARE, (void *)target_freq);
    __dramfreq_set(target_freq);
    blocking_notifier_call_chain(&dramfreq_notifier_list,
            DRAMFREQ_NOTIFY_DONE, (void *)target_freq);
    sw_restart_ehci(1);
    sw_restart_ehci(2);
    if(__dram_get_cur() == DRAM_NORMAL_FREQ){
        cpu_hotplug_set_lock(0);
    }
    mutex_unlock(&dram_info->mutex);
exit:
    DRAMFREQ_DBG("%s:%s done freq:%d\n", __FILE__, __func__, dram_info->cur_freq);
    return;
}

struct cpu_workqueue_struct *get_work_cwq(struct work_struct *work);

static void dramfreq_early_suspend(struct early_suspend *h)
{
    int delay;
    int ret;
    delay = msecs_to_jiffies(dram_info.mstime);
    DRAMFREQ_DBG("%s:%s:%d\n", __FILE__, __func__,__LINE__);
    dram_info.target_freq = DRAM_LOWER_FREQ;
    ret = queue_delayed_work(dram_freq_workqueue, &dram_info.work, delay);
}

static void dramfreq_late_resume(struct early_suspend *h)
{
    DRAMFREQ_DBG("%s:%s:%d\n", __FILE__, __func__,__LINE__);
    cancel_delayed_work_sync(&dram_info.work);
    if (__dram_get_cur() != DRAM_NORMAL_FREQ){
        dram_info.target_freq = DRAM_NORMAL_FREQ;
        do_dramfreq_set(&dram_info.work.work);
        DRAMFREQ_DBG("%s:%s done\n", __FILE__, __func__);
    }
}

static struct early_suspend dramfreq_earlysuspend =
{
    .level   = EARLY_SUSPEND_LEVEL_DISABLE_FB + 100,
    .suspend = dramfreq_early_suspend,
    .resume  = dramfreq_late_resume,
};

static int dramfreq_powernow_notifier_call(struct notifier_block *nfb,
                    unsigned long mode, void *cmd)
{
    unsigned int usb_stat_bak = dram_info.usb_status;
    
    switch (mode){
    case SW_POWERNOW_USB:
        dram_info.usb_status = 1;
        break;
    case SW_POWERNOW_EXTREMITY:
    case SW_POWERNOW_MAXPOWER:
    case SW_POWERNOW_PERFORMANCE:
    case SW_POWERNOW_NORMAL:
        dram_info.usb_status = 0;
        break;
    default:
        dram_info.usb_status = 0;
        break;
    }
    if (usb_stat_bak != dram_info.usb_status){
        DRAMFREQ_DBG("%s:%s:%d:%d-%d\n", __FILE__, __func__,__LINE__,usb_stat_bak,dram_info.usb_status);
        cancel_delayed_work_sync(&dram_info.work);
        do_dramfreq_set(&dram_info.work.work);
    }
    return NOTIFY_OK;
}

static struct notifier_block dram_freq_powernow_notifier = {
    .notifier_call = dramfreq_powernow_notifier_call,
};

static int dram_pm_notify(struct notifier_block *nb, unsigned long event,
				void *data)
{
    dramfreq_late_resume(0);
    return 0;
}
static struct notifier_block dram_pm_notifier = {
    .notifier_call = dram_pm_notify,
    .priority = 0xFFFF,
};

static __devinit int sun7i_dramfreq_probe(struct platform_device *pdev)
{
    int err = 0;
    int retval = 0;

    retval = fetch_dram_para(&dram_info.dram_para);
    if (retval != 0) {
        printk(KERN_ERR "fetch_dram_para from sysconfig failed\n");
        return -ENODEV;
    }

    err = sysfs_create_files(&pdev->dev.kobj, dramfreq_attrib);
    if (err) {
        printk(KERN_ERR "create sysfs file failed\n");
        goto exit;
    }

    dram_info.clk_ddr = clk_get(NULL, CLK_SYS_PLL5P);
    if (IS_ERR(dram_info.clk_ddr)) {
        printk(KERN_ERR "%s: could not get clock(s)\n", __func__);
        dram_info.clk_ddr = NULL;
    }

#ifdef CONFIG_DRAM_FREQ_DVFS
    {
        err = __init_dvfs_syscfg(&dram_info);
        if(err) {
            printk(KERN_ERR "init V-F Table failed\n");
            goto err_create_files;
        }

#if 0
        dram_info.sysvdd = regulator_get(NULL, "axp20_buck3");
        if (IS_ERR(dram_info.sysvdd)) {
            printk(KERN_ERR "some error happen, fail to get regulator!");
            dram_info.sysvdd = 0;
        } else {
            dram_info.last_vdd = regulator_get_voltage(dram_info.sysvdd) / 1000;
            DRAMFREQ_DBG("last_vdd=%d\n", dram_info.last_vdd);
        }
#else
        dram_info.sysvdd = 0;
#endif
        dram_info.clk_ahb = clk_get(NULL, CLK_SYS_AHB);
        if (IS_ERR(dram_info.clk_ahb)) {
            printk(KERN_ERR "%s: could not get clock(s)\n", __func__);
            dram_info.clk_ahb = NULL;
        }
    }
#endif
    register_early_suspend(&dramfreq_earlysuspend);
    register_sw_powernow_notifier(&dram_freq_powernow_notifier);
    register_pm_notifier(&dram_pm_notifier);

    DRAMFREQ_DBG("sun7i dramfreq probe ok!\n");

    return 0;

err_create_files:
    sysfs_remove_files(&pdev->dev.kobj, dramfreq_attrib);
exit:
    return err;
}

static __devexit int sun7i_dramfreq_remove(struct platform_device *pdev)
{
    unregister_pm_notifier(&dram_freq_powernow_notifier);
    unregister_early_suspend(&dramfreq_earlysuspend);
#ifdef CONFIG_DRAM_FREQ_DVFS
    if (dram_info.sysvdd){
        regulator_put(dram_info.sysvdd);
    }
    if (dram_info.clk_ahb){
        clk_put(dram_info.clk_ahb);
    }
#endif
    return 0;
}


static struct platform_driver sun7i_dramfreq_driver = {
    .probe  = sun7i_dramfreq_probe,
    .remove	= sun7i_dramfreq_remove,
    .driver = {
        .name   = "sun7i-dramfreq",
        .owner  = THIS_MODULE,
    },
};

struct platform_device sun7i_dramfreq_device = {
    .name   = "sun7i-dramfreq",
    .id     = -1,
};

static int __init sun7i_dramfreq_init(void)
{
    int ret = 0;
    struct __workqueue_struct__ *tmp_workqueue;

    mutex_init(&dram_info.mutex);

    /* create dvfs daemon */
    dram_freq_workqueue = create_singlethread_workqueue("dramfreq");
    if (!dram_freq_workqueue) {
        pr_err("%s cannot create workqueue\n", __func__);
        ret = -ENOMEM;
        goto err_queue;
    }
    tmp_workqueue = (void *)dram_freq_workqueue;
    INIT_DELAYED_WORK_DEFERRABLE(&dram_info.work, do_dramfreq_set);
    
    ret = platform_device_register(&sun7i_dramfreq_device);
    if (ret) {
        printk(KERN_ERR "dramfreq device init failed!\n");
        goto out;
    }

    ret = platform_driver_register(&sun7i_dramfreq_driver);
    if (ret) {
        printk(KERN_ERR "dramfreq driver init failed!\n");
        goto out;
    }
    return 0;
out:
    destroy_workqueue(dram_freq_workqueue);
err_queue:
    return ret;
}
late_initcall(sun7i_dramfreq_init);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("sun7i dramfreq driver");
MODULE_AUTHOR("xiechr");
