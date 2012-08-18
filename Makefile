
INCLUDE_DIR := include
SRC_DIR     := source
OBJ_DIR     := objects
TESTS_DIR   := tests

STATIC_LIB_NAME := librho.a

CC := g++
CC_FLAGS := -g -rdynamic -Wall -Werror -I $(INCLUDE_DIR)

CPP_SRC_FILES := $(shell find $(SRC_DIR) -name '*.cpp' -type f)
CPP_OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(CPP_SRC_FILES))

MM_SRC_FILES := $(shell find $(SRC_DIR) -name '*.mm' -type f)
MM_OBJ_FILES := $(patsubst $(SRC_DIR)/%.mm,$(OBJ_DIR)/%.o,$(MM_SRC_FILES))

all : $(CPP_OBJ_FILES) $(MM_OBJ_FILES)

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
	@$(CC) $(CC_FLAGS) -c $< -o $@
	@ar crsv $(OBJ_DIR)/$(STATIC_LIB_NAME) $@
	@echo

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.mm
	@echo "Compiling $< ..."
	@mkdir -p $(@D)
	@$(CC) $(CC_FLAGS) -c $< -o $@
	@ar crsv $(OBJ_DIR)/$(STATIC_LIB_NAME) $@
	@echo

.PHONY : all install ensure_root test clean
