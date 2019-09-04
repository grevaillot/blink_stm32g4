PROJECT = blink
BUILD_DIR = bin

CFILES = blink.c

DEVICE?=stm32g431vbe
OPENCM3_DIR=libopencm3

OOCD_INTERFACE=stlink
OOCD_TARGET=stm32g4x

include $(OPENCM3_DIR)/mk/genlink-config.mk
include rules.mk
include $(OPENCM3_DIR)/mk/genlink-rules.mk

tags:
	ctags -R .
