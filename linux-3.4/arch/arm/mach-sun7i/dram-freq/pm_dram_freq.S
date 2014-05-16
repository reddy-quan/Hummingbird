	.globl	dfreq_bin_start
dfreq_bin_start:
	.incbin	"arch/arm/mach-sun7i/pm/dram-freq/dram_freq_entry.code"
	.globl	dfreq_bin_end
dfreq_bin_end:
	.align	2
