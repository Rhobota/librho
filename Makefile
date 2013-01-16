
INCLUDE_DIR := include
SRC_DIR     := source
OBJ_DIR     := objects
TESTS_DIR   := tests

STATIC_LIB_NAME := librho.a

CC := $(TARGET)g++
AR := $(TARGET)ar
CC_FLAGS += -g -Wall -Werror -I $(INCLUDE_DIR)

ifeq ($(shell uname),Linux)
	# Linux stuff:
	CC_FLAGS += -rdynamic
else
ifeq ($(shell uname),Darwin)
	# OSX stuff:
	CC_FLAGS  += -rdynamic
	PRE_STEP  := 'osx_pre_step'
	POST_STEP := 'osx_post_step'
else
	# Mingw stuff:
endif
endif

CPP_SRC_FILES := $(shell find $(SRC_DIR) -name '*.cpp' -type f)
CPP_OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(CPP_SRC_FILES))

MM_SRC_FILES := $(shell find $(SRC_DIR) -name '*.mm' -type f)
MM_OBJ_FILES := $(patsubst $(SRC_DIR)/%.mm,$(OBJ_DIR)/%.o,$(MM_SRC_FILES))

all : $(PRE_STEP) $(CPP_OBJ_FILES) $(MM_OBJ_FILES) $(POST_STEP)

install : ensure_root all
	@echo
	@cp -r $(INCLUDE_DIR)/* /usr/local/include
	@cp $(OBJ_DIR)/$(STATIC_LIB_NAME) /usr/local/lib
	@echo "Install successful."

ensure_root :
	$(if $(shell whoami | grep root),,\
	@echo 'You must be root to run to perform that operation.' && exit 1; \
	)

test : all
	@$(TESTS_DIR)/RunTests.bash

clean :
	@rm -rf $(OBJ_DIR)
	@echo "Clean successful."

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.cpp
	@echo "Compiling $< ..."
	@mkdir -p $(@D)
	$(CC) $(CC_FLAGS) -c $< -o $@
	$(AR) crsv $(OBJ_DIR)/$(STATIC_LIB_NAME) $@
	@echo

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.mm
	@echo "Compiling $< ..."
	@mkdir -p $(@D)
	$(CC) $(CC_FLAGS) -c $< -o $@
	$(AR) crsv $(OBJ_DIR)/$(STATIC_LIB_NAME) $@
	@echo

osx_pre_step :
	@mv source/rho/img/tImageCapFactory.cpp \
		source/rho/img/tImageCapFactory.mm

osx_post_step :
	@mv source/rho/img/tImageCapFactory.mm \
		source/rho/img/tImageCapFactory.cpp

.PHONY : all install ensure_root test clean osx_pre_step osx_post_step
