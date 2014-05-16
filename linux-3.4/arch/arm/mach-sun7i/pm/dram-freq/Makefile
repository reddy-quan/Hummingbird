#makefile for dram_freq_entry.code

INCLUDE   = -I. \
	-I$(KDIR)/include \
	-I$(KDIR)/arch/arm/mach-sun7i/include

DRAM_OBJ =  dram_freq_entry.o       \
			./../mem_serial.o		\
			./../mem_printk.o  		\
			./../mem_divlibc.o		\
			./../mem_divlib.o	    \
			./../mem_delay.o        \
			./../mem_timing.o       \
			./../mem_int.o			\
			./../mem_mmu_pc_asm.o   \
			./../standby/dram/dram.o      	\
			./../standby/dram/dram_init.o       

CC = $(CROSS_COMPILE)gcc

CFLAGS = -g -c -nostdlib -march=armv7-a -marm -D__STANDBY_MODULE__ -fno-unwind-tables -fno-asynchronous-unwind-tables -mlittle-endian -O2 --min_array_alignment=4 --no_unaligned_access
CFLAGS_DRAM_VADDR = -D DRAM_USE_VA_ADDR

LD_FLAGS = -static --gc-sections

all:
	$(CC) $(INCLUDE) $(CFLAGS) -e dram_freq_main dram_freq_entry.c           		-o dram_freq_entry.o
	$(CC) $(INCLUDE) $(CFLAGS) $(CFLAGS_DRAM_VADDR) -O0 ./../standby/dram/dram.c        -o ./../standby/dram/dram.o
	$(CC) $(INCLUDE) $(CFLAGS) $(CFLAGS_DRAM_VADDR) -O0 ./../standby/dram/dram_init.c   -o ./../standby/dram/dram_init.o
	$(CC) $(INCLUDE) $(CFLAGS) $(CFLAGS_DRAM_VADDR) ./../mem_divlib.S               -o ./../mem_divlib.o 
	$(CC) $(INCLUDE) $(CFLAGS) $(CFLAGS_DRAM_VADDR) ./../mem_divlibc.c              -o ./../mem_divlibc.o 
	$(CC) $(INCLUDE) $(CFLAGS) $(CFLAGS_DRAM_VADDR) ./../mem_timing.c    		    -o ./../mem_timing.o 
	$(CC) $(INCLUDE) $(CFLAGS) $(CFLAGS_DRAM_VADDR) ./../mem_delay.S                -o ./../mem_delay.o
	$(CC) $(INCLUDE) $(CFLAGS) $(CFLAGS_DRAM_VADDR) ./../mem_mmu_pc_asm.S           -o ./../mem_mmu_pc_asm.o
	$(CC) $(INCLUDE) $(CFLAGS) $(CFLAGS_DRAM_VADDR) ./../mem_printk.c               -o ./../mem_printk.o 
	$(CC) $(INCLUDE) $(CFLAGS) $(CFLAGS_DRAM_VADDR) ./../mem_serial.c               -o ./../mem_serial.o 
	$(CC) $(INCLUDE) $(CFLAGS) $(CFLAGS_DRAM_VADDR) ./../mem_int.c               -o ./../mem_int.o 


	$(CROSS_COMPILE)ld -T dram_freq.xn $(LD_FLAGS) $(LIBS) -EL  $(DRAM_OBJ) -o dram_freq_entry.elf  -Map dram_freq_entry.map
	$(CROSS_COMPILE)objdump -D dram_freq_entry.elf > dram_freq_entry.lst
	$(CROSS_COMPILE)objcopy -O binary dram_freq_entry.elf dram_freq_entry.bin

	rm -rf *.o ../*.o
	cp dram_freq_entry.bin dram_freq_entry.code

clean:
	-rm *.code *.map *.lst *.bin *.elf

