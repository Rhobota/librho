
SRC_DIR     := .
OBJ_DIR     := ../../../../objects/rho/crypt/aesni

STATIC_LIB_NAME := ../../../objects/librho.a

CC    := $(TARGET)gcc
ASMCC := yasm
AR    := $(TARGET)ar

CC_FLAGS_LOCAL := $(CC_FLAGS) \
	-g -O3 -fvisibility=hidden -Wall -Wextra -Werror -pedantic \
	-Wswitch-default -Wcast-qual -Wcast-align -Wconversion \
	-Wno-unused-parameter -Wno-long-long -Wno-sign-conversion

ASMCC_FLAGS_LOCAL := -D__linux__ -g dwarf2 -f elf64

ifeq ($(shell uname),Linux)
	# Linux stuff:
	CC_FLAGS_LOCAL += -rdynamic -Wdouble-promotion
else
ifeq ($(shell uname),Darwin)
	# OSX stuff:
	CC_FLAGS_LOCAL += -mmacosx-version-min=10.6 -I /usr/local/include
else
	# Mingw and Cygwin stuff:
endif
endif

C_SRC_FILES = $(shell find $(SRC_DIR) -name '*.c' -type f)
C_OBJ_FILES = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(C_SRC_FILES))

S_SRC_FILES = $(shell find $(SRC_DIR) -name '*.s' -type f)
S_OBJ_FILES = $(patsubst $(SRC_DIR)/%.s,$(OBJ_DIR)/%.o,$(S_SRC_FILES))

all : $(C_OBJ_FILES) $(S_OBJ_FILES)

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.c
	@echo "Compiling $< ..."
	@mkdir -p $(@D)
	$(CC) $(CC_FLAGS_LOCAL) -c -o $@ $<
	$(AR) crsv $(STATIC_LIB_NAME) $@
	@echo

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.s
	@echo "Compiling $< ..."
	@mkdir -p $(@D)
	$(ASMCC) $(ASMCC_FLAGS_LOCAL) -c -o $@ $<
	$(AR) crsv $(STATIC_LIB_NAME) $@
	@echo

.PHONY : all

#yasm -D__linux__ -g dwarf2 -f elf32 asm/x86/iaesx86.s -o obj/x86/iaesx86.o
