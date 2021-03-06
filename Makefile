
INCLUDE_DIR := include
SRC_DIR     := source
OBJ_DIR     := objects
TESTS_DIR   := tests

STATIC_LIB_NAME := librho.a

C  := $(TARGET)gcc
CC := $(TARGET)g++
AR := $(TARGET)ar
C_FLAGS_LOCAL := $(CC_FLAGS) \
	-g -O2 -fvisibility=hidden -Wall -Wextra -Werror -pedantic \
	-Wno-unused-parameter -Wno-long-long -Wno-sign-conversion \
	-D_FILE_OFFSET_BITS=64 \
	-maes \
	-I $(INCLUDE_DIR)
CC_FLAGS_LOCAL := $(CC_FLAGS) \
	-g -O2 -fvisibility=hidden -Wall -Wextra -Werror -pedantic \
	-Wswitch-default -Wcast-qual -Wcast-align -Wconversion \
	-Wno-unused-parameter -Wno-long-long -Wno-sign-conversion \
	-D_FILE_OFFSET_BITS=64 \
	-maes \
	-I $(INCLUDE_DIR)  # consider: -Wold-style-cast -Wshadow

ifeq ($(shell uname),Linux)
	# Linux stuff:
	CC_FLAGS_LOCAL += -rdynamic -Wdouble-promotion
else
ifeq ($(shell uname),Darwin)
	# OSX stuff:
	CC_FLAGS_LOCAL += -I /usr/local/include
	PRE_STEP  := osx_pre_step
	POST_STEP := osx_post_step
else
	# Mingw and Cygwin stuff:
endif
endif

C_SRC_FILES = $(shell find $(SRC_DIR) -name '*.c' -type f)
C_OBJ_FILES = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(C_SRC_FILES))

CPP_SRC_FILES = $(shell find $(SRC_DIR) -name '*.cpp' -type f)
CPP_OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(CPP_SRC_FILES))

MM_SRC_FILES = $(shell find $(SRC_DIR) -name '*.mm' -type f)
MM_OBJ_FILES = $(patsubst $(SRC_DIR)/%.mm,$(OBJ_DIR)/%.o,$(MM_SRC_FILES))

all : $(PRE_STEP) $(C_OBJ_FILES) $(CPP_OBJ_FILES) $(MM_OBJ_FILES) $(POST_STEP)

install : ensure_root uninstall all
	@echo
	@cp -r $(INCLUDE_DIR)/* /usr/local/include
	@cp $(OBJ_DIR)/$(STATIC_LIB_NAME) /usr/local/lib
	@echo "Install successful."

uninstall : ensure_root
	@(cd $(INCLUDE_DIR) && for i in *; do rm -rf /usr/local/include/$$i; done)

ensure_root :
	$(if $(shell whoami | grep root),,\
	@echo 'You must be root to run to perform that operation.' && exit 1; \
	)

test : all
	@$(TESTS_DIR)/RunTests.bash

clean :
	@rm -rf $(OBJ_DIR)
	@echo "Clean successful."

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.c
	@echo "Compiling $< ..."
	@mkdir -p $(@D)
	$(C) $(C_FLAGS_LOCAL) -c -o $@ $<
	$(AR) crsv $(OBJ_DIR)/$(STATIC_LIB_NAME) $@
	@echo

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.cpp
	@echo "Compiling $< ..."
	@mkdir -p $(@D)
	$(CC) $(CC_FLAGS_LOCAL) -c -o $@ $<
	$(AR) crsv $(OBJ_DIR)/$(STATIC_LIB_NAME) $@
	@echo

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.mm
	@echo "Compiling $< ..."
	@mkdir -p $(@D)
	$(CC) $(CC_FLAGS_LOCAL) -c -o $@ $<
	$(AR) crsv $(OBJ_DIR)/$(STATIC_LIB_NAME) $@
	@echo

osx_pre_step :
	@mv source/rho/img/tImageCapFactory.cpp \
		source/rho/img/tImageCapFactory.mm

osx_post_step :
	@mv source/rho/img/tImageCapFactory.mm \
		source/rho/img/tImageCapFactory.cpp

.PHONY : all install uninstall ensure_root test clean osx_pre_step osx_post_step
