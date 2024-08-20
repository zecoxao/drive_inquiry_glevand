CELL_MK_DIR ?= $(CELL_SDK)/samples/mk

include $(CELL_MK_DIR)/sdk.makedef.mk

PPU_SRCS = main.c
PPU_TARGET = EBOOT.ELF

GCC_PPU_INCDIRS += -I../common/include
PPU_LDLIBS = -lm -lio_stub -lfs_stub -lsysmodule_stub -lsysutil_stub -lsysutil_game_stub

include $(CELL_MK_DIR)/sdk.target.mk
