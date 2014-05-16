/*
 *  linux/arch/arm/mach-sun7i/hotplug.c
 *
 *  Copyright (C) 2012-2016 Allwinner Ltd.
 *  All Rights Reserved
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/smp.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <asm/cacheflush.h>
#include <asm/smp_plat.h>
#include <mach/platform.h>
#include <mach/hardware.h>

#include <linux/cpu.h>
#include <linux/jiffies.h>
#include <linux/mutex.h>
#include <linux/suspend.h>
#include <mach/powernow.h>


static cpumask_t dead_cpus;

#define IS_WFI_MODE(cpu)    (readl(IO_ADDRESS(SW_PA_CPUCFG_IO_BASE) + CPUX_STATUS(cpu)) & (1<<2))

int platform_cpu_kill(unsigned int cpu)
{
    int k;
    u32 pwr_reg;
    int tmp_cpu;

    if (cpu == 0)
    {
        pr_err("[hotplug]: try to kill cpu:%d failed!\n", cpu);
        return 0;
    }
    tmp_cpu = get_cpu();
    put_cpu();
    pr_info("[hotplug]: cpu(%d) try to kill cpu(%d)\n", tmp_cpu, cpu);

    for (k = 0; k < 1000; k++) {
        if (cpumask_test_cpu(cpu, &dead_cpus) && IS_WFI_MODE(cpu)) {

            /* step8: deassert cpu core reset */
            writel(0, IO_ADDRESS(SW_PA_CPUCFG_IO_BASE) + CPUX_RESET_CTL(cpu));

            /* step8: deassert DBGPWRDUP signal */
            pwr_reg = readl(IO_ADDRESS(SW_PA_CPUCFG_IO_BASE) + AW_CPUCFG_DBGCTL1);
            pwr_reg &= ~(1<<cpu);
            writel(pwr_reg, IO_ADDRESS(SW_PA_CPUCFG_IO_BASE) + AW_CPUCFG_DBGCTL1);

            /* step9: set up power-off signal */
            pwr_reg = readl(IO_ADDRESS(SW_PA_CPUCFG_IO_BASE) + AW_CPU1_PWROFF_REG);
            pwr_reg |= 1;
            writel(pwr_reg, IO_ADDRESS(SW_PA_CPUCFG_IO_BASE) + AW_CPU1_PWROFF_REG);
            mdelay(1);

            /* step10: active the power output clamp */
            writel(0x01, IO_ADDRESS(SW_PA_CPUCFG_IO_BASE) + AW_CPU1_PWR_CLAMP);
            writel(0x03, IO_ADDRESS(SW_PA_CPUCFG_IO_BASE) + AW_CPU1_PWR_CLAMP);
            writel(0x07, IO_ADDRESS(SW_PA_CPUCFG_IO_BASE) + AW_CPU1_PWR_CLAMP);
            writel(0x0f, IO_ADDRESS(SW_PA_CPUCFG_IO_BASE) + AW_CPU1_PWR_CLAMP);
            writel(0x1f, IO_ADDRESS(SW_PA_CPUCFG_IO_BASE) + AW_CPU1_PWR_CLAMP);
            writel(0x3f, IO_ADDRESS(SW_PA_CPUCFG_IO_BASE) + AW_CPU1_PWR_CLAMP);
            writel(0x7f, IO_ADDRESS(SW_PA_CPUCFG_IO_BASE) + AW_CPU1_PWR_CLAMP);
            writel(0xff, IO_ADDRESS(SW_PA_CPUCFG_IO_BASE) + AW_CPU1_PWR_CLAMP);
            pr_info("[hotplug]: cpu%d is killed!\n", cpu);

            return 1;
        }

        mdelay(1);
    }

    pr_err("[hotplug]: try to kill cpu:%d failed!\n", cpu);

    return 0;
}

void platform_cpu_die(unsigned int cpu)
{
    unsigned long actlr;

    /* notify platform_cpu_kill() that hardware shutdown is finished */
    cpumask_set_cpu(cpu, &dead_cpus);

    /* step1: disable cache */
    asm("mrc    p15, 0, %0, c1, c0, 0" : "=r" (actlr) );
    actlr &= ~(1<<2);
    asm("mcr    p15, 0, %0, c1, c0, 0\n" : : "r" (actlr));

    /* step2: clean and ivalidate L1 cache */
    flush_cache_all();

    /* step3: execute a CLREX instruction */
    asm("clrex" : : : "memory", "cc");

    /* step4: switch cpu from SMP mode to AMP mode, aim is to disable cache coherency */
    asm("mrc    p15, 0, %0, c1, c0, 1" : "=r" (actlr) );
    actlr &= ~(1<<6);
    asm("mcr    p15, 0, %0, c1, c0, 1\n" : : "r" (actlr));

    /* step5: execute an ISB instruction */
    isb();
    /* step6: execute a DSB instruction  */
    dsb();

    /* step7: execute a WFI instruction */
    while(1) {
        asm("wfi" : : : "memory", "cc");
    }
}

int platform_cpu_disable(unsigned int cpu)
{
    cpumask_clear_cpu(cpu, &dead_cpus);
    /*
     * we don't allow CPU 0 to be shutdown (it is still too special
     * e.g. clock tick interrupts)
     */
    return cpu == 0 ? -EPERM : 0;
}

#define FANTASY_DEBUG_HOTPLUG 1
#define LOAD_INT(x) ((x) >> FSHIFT)
#define LOAD_FRAC(x) LOAD_INT(((x) & (FIXED_1-1)) * 100)

#define DEF_CPU_UP_LOADING        ((1<<FSHIFT) + (60)*(FIXED_1-1)/100)
#define DEF_CPU_DOWN_LOADING      ((1<<FSHIFT) + (80)*(FIXED_1-1)/100) 
#define DEF_CPU_UP_ACTIVE_TASK    4
#define DEF_CPU_DOWN_ACTIVE_TASK  1 
#define DEF_CPU_HOTPLUG_SAMPLIMG_RATE 300000

/*
 * define data structure for dbs
 */
struct cpu_hotplug_info_s {
    struct delayed_work work;
    int cpu;                    /* current cpu number           */
    struct work_struct up_work;     /* cpu plug-in processor    */
    struct work_struct down_work;   /* cpu plug-out processer   */
    /*
     * percpu mutex that serializes governor limit change with
     * do_dbs_timer invocation. We do not want do_dbs_timer to run
     * when user is changing the governor or limits.
     */
    struct mutex timer_mutex;   /* semaphore for protection     */
    
    unsigned int sampling_rate;     /* dvfs sample rate                             */

    /* pegasusq tuners */
    atomic_t hotplug_lock;          /* lock cpu online number, disable plug-in/out  */
    unsigned int cpulock_powernow;  /* cpu lock status of power now mode            */
    unsigned int cpulock_setting;   /* cpu lock status of user setting              */
    unsigned int dvfs_debug;        /* dvfs debug flag, print dbs information       */
};

static struct cpu_hotplug_info_s cpu_hotplug_info = {
    .sampling_rate = DEF_CPU_HOTPLUG_SAMPLIMG_RATE,
    .hotplug_lock = ATOMIC_INIT(0),
    .cpulock_powernow = 0,
    .cpulock_setting = 0,
    .dvfs_debug = 0,
};

static struct workqueue_struct *hotplug_workqueue;

/*
 * lock cpu number, the number of onlie cpu should less then num_core
 */
int cpu_hotplug_set_lock(int num_core)
{
    int online;
    struct work_struct *work;

    if (num_core < 0 || num_core > num_possible_cpus()) {
        return -EINVAL;
    }
    
    mutex_lock(&cpu_hotplug_info.timer_mutex);
    cpu_hotplug_info.cpulock_setting = num_core;
    if (num_core == 0 && cpu_hotplug_info.cpulock_powernow != 0){
        num_core = cpu_hotplug_info.cpulock_powernow;
    }
    atomic_set(&cpu_hotplug_info.hotplug_lock, num_core);
    cancel_work_sync(&cpu_hotplug_info.up_work);
    cancel_work_sync(&cpu_hotplug_info.down_work);
    online = num_online_cpus();
    if (num_core != online){
        work = (num_core - online) > 0 ? &cpu_hotplug_info.up_work : &cpu_hotplug_info.down_work;
        queue_work_on(0, hotplug_workqueue, work);
    }
    mutex_unlock(&cpu_hotplug_info.timer_mutex);

    return 0;
}

/*
 * lock cpu number, the number of onlie cpu should less then num_core
 */
int cpu_hotplug_get_lock(void)
{
    return cpu_hotplug_info.cpulock_setting;
}


/*
 * cpu hotplug, just plug in one cpu
 */
static void cpu_up_work(struct work_struct *work)
{
    int cpu, nr_up, online, hotplug_lock;
    struct cpu_hotplug_info_s *hotplug_info =
        container_of(work, struct cpu_hotplug_info_s, up_work);

//    mutex_lock(&hotplug_info->timer_mutex);
    online = num_online_cpus();
    hotplug_lock = atomic_read(&hotplug_info->hotplug_lock);

    if (hotplug_lock) {
        nr_up = (hotplug_lock - online) > 0? (hotplug_lock-online) : 0;
    } else {
        nr_up = 1;
    }

    for_each_cpu_not(cpu, cpu_online_mask) {
        if (cpu == 0)
            continue;

        if (nr_up-- == 0)
            break;

        printk(KERN_DEBUG "cpu up:%d\n", cpu);
        cpu_up(cpu);
    }
//    mutex_unlock(&hotplug_info->timer_mutex);
}

/*
 * cpu hotplug, cpu plugout
 */
static void cpu_down_work(struct work_struct *work)
{
    int cpu, nr_down, online, hotplug_lock;
    struct cpu_hotplug_info_s *hotplug_info =
        container_of(work, struct cpu_hotplug_info_s, down_work);
    
//    mutex_lock(&hotplug_info->timer_mutex);
    online = num_online_cpus();
    hotplug_lock = atomic_read(&hotplug_info->hotplug_lock);

    if (hotplug_lock) {
        nr_down = (online - hotplug_lock) > 0? (online-hotplug_lock) : 0;
    } else {
        nr_down = 1;
    }

    for_each_online_cpu(cpu) {
        if (cpu == 0)
            continue;

        if (nr_down-- == 0)
            break;

        cpu_down(cpu);
    }
//    mutex_unlock(&hotplug_info->timer_mutex);
}


/*
 * check if need plug in one cpu core
 */
static int check_hotplug(struct cpu_hotplug_info_s *hotplug_info)
{
    unsigned long load[3];
    int online;
    int ncpus;
    int hotplug_lock = atomic_read(&cpu_hotplug_info.hotplug_lock);

    online = num_online_cpus();
    ncpus = online;

    /* hotplug has been locked, do nothing */
    if (hotplug_lock > 0){
        ncpus = hotplug_lock;
        goto __do_hotplug;
    }
    
    get_avenrun_sec(load, FIXED_1/200, 0);
    if (unlikely(cpu_hotplug_info.dvfs_debug & FANTASY_DEBUG_HOTPLUG)) {
        printk("load0:%lx,(%lu.%02lu); load1:%lx,(%lu.%02lu), active task:%lu\n", 
                load[0], LOAD_INT(load[0]), LOAD_FRAC(load[0]),
                load[1], LOAD_INT(load[1]), LOAD_FRAC(load[1]),
                load[2]);
    }

    if (load[2] >= DEF_CPU_UP_ACTIVE_TASK && online < num_possible_cpus())
    {
        ncpus = online + 1;
    }
    else if (load[2] <= DEF_CPU_DOWN_ACTIVE_TASK 
        && load[0] < DEF_CPU_DOWN_LOADING && online > 1){
        ncpus = online - 1;
    }
    
__do_hotplug:
    if (ncpus > online){
        queue_work_on(0, hotplug_workqueue, &hotplug_info->up_work);
    }else if (ncpus < online) {
        queue_work_on(0, hotplug_workqueue, &hotplug_info->down_work);
    }
    
    return 0;
}

static void do_hotplug_timer(struct work_struct *work)
{
    struct cpu_hotplug_info_s *hotplug_info =
        container_of(work, struct cpu_hotplug_info_s, work.work);
    int delay;

    mutex_lock(&hotplug_info->timer_mutex);
    check_hotplug(hotplug_info);
    delay = usecs_to_jiffies(hotplug_info->sampling_rate);
    queue_delayed_work(hotplug_workqueue, &hotplug_info->work, delay);
    mutex_unlock(&hotplug_info->timer_mutex);
}

static int powernow_notifier_call(struct notifier_block *nfb,
                    unsigned long mode, void *cmd)
{
    int retval = NOTIFY_DONE;
    int cpu_lock = -1;
    int hotplug_lock;
    
    switch (mode) {
    case SW_POWERNOW_EXTREMITY:
    case SW_POWERNOW_MAXPOWER:
        cpu_lock = num_possible_cpus();
        break;
                
    case SW_POWERNOW_PERFORMANCE:
    case SW_POWERNOW_NORMAL:
        cpu_lock = 0;
        break;
        
    default:
        retval = -EINVAL;
        break;
    }
    if (cpu_lock >= 0){
        cpu_hotplug_info.cpulock_powernow = cpu_lock;
        hotplug_lock = atomic_read(&cpu_hotplug_info.hotplug_lock);
        if (hotplug_lock == 0 && cpu_hotplug_info.cpulock_powernow){
            atomic_set(&cpu_hotplug_info.hotplug_lock, cpu_lock);
            queue_work_on(0, hotplug_workqueue, &cpu_hotplug_info.up_work);
        }
    }
    retval = retval ? NOTIFY_DONE:NOTIFY_OK;
    return retval;
}
static struct notifier_block hotplug_powernow_notifier = {
    .notifier_call = powernow_notifier_call,
};

static int hotplug_pm_notify(struct notifier_block *nb, unsigned long event, void *dummy)
{
    struct cpu_hotplug_info_s *hotplug_info = &cpu_hotplug_info;
    int delay;

    if (event == PM_SUSPEND_PREPARE) {
        cancel_delayed_work_sync(&hotplug_info->work);
        cancel_work_sync(&hotplug_info->up_work);
        cancel_work_sync(&hotplug_info->down_work);
        cpu_down_work(&hotplug_info->down_work);
    } else if (event == PM_POST_SUSPEND) {
        delay = usecs_to_jiffies(hotplug_info->sampling_rate);
        queue_delayed_work(hotplug_workqueue, &hotplug_info->work, delay);
    }

    return NOTIFY_OK;
}

static struct notifier_block hotplug_pm_notifier = {
    .notifier_call = hotplug_pm_notify,
    .priority = 0xFF,
};


static int __init sunxi_hotplug_initcall(void)
{
    int ret = 0;
    int delay = usecs_to_jiffies(cpu_hotplug_info.sampling_rate);

    /* create dvfs daemon */
    hotplug_workqueue = create_singlethread_workqueue("hotplug_workqueue");
    if (!hotplug_workqueue) {
        pr_err("%s cannot create workqueue\n", __func__);
        ret = -ENOMEM;
        goto err_queue;
    }

    INIT_WORK(&cpu_hotplug_info.up_work, cpu_up_work);
    INIT_WORK(&cpu_hotplug_info.down_work, cpu_down_work);
    mutex_init(&cpu_hotplug_info.timer_mutex);
    
    INIT_DELAYED_WORK_DEFERRABLE(&cpu_hotplug_info.work, do_hotplug_timer);
    queue_delayed_work(hotplug_workqueue, &cpu_hotplug_info.work, delay);
    
    register_sw_powernow_notifier(&hotplug_powernow_notifier);
    register_pm_notifier(&hotplug_pm_notifier);
    return ret;

err_queue:
    return ret;
}

late_initcall(sunxi_hotplug_initcall);

