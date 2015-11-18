#CROSS ?= /opt/arm-2013.05/bin/arm-none-eabi
CROSS ?= /opt/gcc-arm-none-eabi-4_9-2015q3/bin/arm-none-eabi
CC := $(CROSS)-gcc

PROJECTNAME := maple-leaf-iotest

CFLAGS := -g -O0 -Wall -mlittle-endian -mthumb -mcpu=cortex-m3 -mthumb-interwork
CFLAGS += -fdata-sections -ffunction-sections
CFLAGS += -std=gnu99
CFLAGS += -DUSE_HW_SPI
CFLAGS += -DGIT_BASE_REVISION=\"`git rev-parse --short HEAD`\"
#CFLAGS += -fomit-frame-pointer
LDFLAGS := -lgcc -Wl,-TSTM32F103RB_FLASH.ld
#LDFLAGS +=  -Wl,--print-gc-sections
LDFLAGS += -Xlinker --gc-sections

CFLAGS += -Iinc -Iutil/inc

OBJS := \
        src/startup_stm32f10x_md.o \
        src/system_stm32f10x.o \
        src/main.o \
  	    src/leds.o \
        src/syscalls.o \
#        src/uart.o \
#        util/src/log.o \
#        src/cli/cli.o \
#        src/cli/cmd_free.o \
#        src/cli/cmd_info.o \
#        src/cli/cmd_ps.o \
#        src/cli/cmd_uname.o

include Makefile.stdperiph
include Makefile.mw

ALL_DEPS := $(patsubst %.o,%.d,$(OBJS))

all: $(PROJECTNAME).bin

clean:
	rm -f $(PROJECTNAME).bin $(PROJECTNAME).elf $(PROJECTNAME).map $(OBJS) $(ALL_DEPS)

flash: $(PROJECTNAME).bin $(PROJECTNAME).elf
	openocd -f scripts/stlink-v2.cfg -f scripts/stm32f1x_stlink.cfg \
		-c init \
		-c "reset halt" \
		-c "reset init" \
		-c "flash write_image erase $(PROJECTNAME).elf" \
		-c "verify_image $(PROJECTNAME).elf" \
		-c "reset run" \
		-c shutdown

%.bin:%.elf
	@$(CROSS)-objcopy -O binary $< $@
	@echo "Firmware size: "
	@du -hs $@

$(PROJECTNAME).elf: $(OBJS)
	@echo "LD $@"
	@$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) -Wl,-Map=$(PROJECTNAME).map

%.o:%.c
	@echo "CC $<"
	@$(CC) -MM -MF $(subst .o,.d,$@) -MP -MT "$@ $(subst .o,.d,$@)" $(CFLAGS) $<
	@$(CC) $(CFLAGS) -c -o $@ $<

%.o:%.s
	@echo "CC $<"
	@$(CC) -MM -MF $(subst .o,.d,$@) -MP -MT "$@ $(subst .o,.d,$@)" $(CFLAGS) $<
	@$(CC) $(CFLAGS) -c -o $@ $<	

ifneq ("$(MAKECMDGOALS)","clean")
-include $(ALL_DEPS)
endif
