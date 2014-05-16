PWD=$(shell pwd)

obj-$(CONFIG_LYCHEE_HDMI_SUN7I) += hdmi.o
hdmi-objs := aw/hdmi_core.o aw/hdmi_hal.o aw/hdmi_interface.o aw/hdmi_edid.o drv_hdmi.o dev_hdmi.o

obj-$(CONFIG_LYCHEE_HDMI_HDCP_SUN7I) += hdcp.o
hdcp-objs := aw/hdmi_core_hdcp.o aw/hdmi_hal.o aw/hdmi_interface.o aw/hdmi_edid.o drv_hdmi.o dev_hdmi.o aw/libhdcp.a

install: build
	cp hdmi.ko $(LICHEE_MOD_DIR)/hdmi.ko
	cp hdcp.ko $(LICHEE_MOD_DIR)/hdcp.ko
build:
	@echo $(LICHEE_KDIR)
	cp aw/libhdcp aw/libhdcp.a
	$(MAKE) -C $(LICHEE_KDIR) M=$(PWD)

clean:
	@rm -rf *.o *.ko .*.cmd *.mod.c *.order *.symvers .tmp_versions *~