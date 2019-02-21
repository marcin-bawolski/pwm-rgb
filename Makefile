##
## $URL: https://repo1.mydevil.net/svn/priv/bawolski/stm8/trunk/tig_controller/Makefile $      
## $Revision: 31 $ 

CC=sdcc

STVP="C:/Program Files (x86)/STMicroelectronics/st_toolset/stvp/STVP_CmdLine.exe"

ROOT_PROJ=$(strip $(CURDIR))
INCLUDE_DIRS=-I$(ROOT_PROJ) -I$(ROOT_PROJ)/include  -I$(ROOT_PROJ)/misc/ -I$(ROOT_PROJ)/mode2T/ \
-I$(ROOT_PROJ)/STM8S_StdPeriph_Driver/inc -I$(ROOT_PROJ)/STM8S_StdPeriph_Driver/

TARGET:=led

BUILD_DIR :=
OUT_FOLDER := build
OBJS=
C_SRCS=
C_SRCS += main.c

CFLAGS= -DSTM8S103 --all-callee-saves --debug --stack-auto --fverbose-asm \
--fomit-frame-pointer --opt-code-size --allow-unsafe-read    



-include ./misc/misc.mk
-include ./STM8S_StdPeriph_Driver/src/lib.mk

OBJS :=   $(C_SRCS:.c=.rel)
vpath %.c $(sort $(dir $(C_SRCS)))

.PHONY: all flash program 

all:  $(TARGET).elf $(TARGET).hex   size $(OBJS) Makefile
 
$(TARGET).hex: $(OBJS) Makefile
	$(CC) -o $@ $(CFLAGS)  -mstm8 $(INCLUDE_DIRS) --out-fmt-ihx $(OBJS)  
	
$(TARGET).elf: $(OBJS) Makefile
	$(CC) -o $@ $(CFLAGS)  -mstm8 $(INCLUDE_DIRS) --debug  --out-fmt-elf $(OBJS)
	
%.rel: %.c Makefile
	$(CC) -c -mstm8  $(CFLAGS) -o "$@" $(INCLUDE_DIRS) $< 

size: $(TARGET).elf Makefile
	@echo 
	stm8-size $<

flash: $(TARGET).hex Makefile
	stm8flash -c stlinkv2 -p stm8s103?3  -w $^

program:  $(TARGET).hex Makefile
	$(STVP)  -BoardName=ST-LINK -Tool_ID=0 -Device=STM8S103F3 -Port=USB -ProgMode=SWIM -no_loop   -FileProg=$< 


Makefile: 	./misc/misc.mk ./STM8S_StdPeriph_Driver/src/lib.mk
	@echo Makefile target done
		
clean: 
	rm -rf $(TARGET).hex 
	rm -rf $(TARGET).ls
	rm -rf $(TARGET).map
	rm -rf $(TARGET).rel
	rm -rf $(TARGET).sym
	rm -rf $(TARGET).rst
	rm -rf $(TARGET).cdb
	rm -rf $(TARGET).elf
	rm -rf $(TARGET).lk
	rm -rf $(OBJS)
	rm -rf $(OBJS:.rel=.rst)
	rm -rf $(OBJS:.rel=.sym)
	rm -rf $(OBJS:.rel=.lst)
	rm -rf $(OBJS:.rel=.asm)
	rm -rf $(OBJS:.rel=.adb)
#	rmdir $(BUILD_DIR)

	
