
SOURCE_DIR  := source
INCLUDE_DIR := include
EXPORT_DIR  := objects
TESTS_DIR   := tests

STATIC_LIB_NAME := librho.a

CC := g++
CC_FLAGS := -g -rdynamic -Wall -Werror -I $(INCLUDE_DIR)

SRC_FILES := $(shell find $(SOURCE_DIR) -name '*.cpp' -type f)
OBJ_FILES := $(patsubst $(SOURCE_DIR)/%.cpp,$(EXPORT_DIR)/%.o,$(SRC_FILES))

all : $(OBJ_FILES)

install : ensure_root all
	@echo
	@cp -r $(INCLUDE_DIR)/* /usr/local/include
	@cp $(EXPORT_DIR)/$(STATIC_LIB_NAME) /usr/local/lib
	@echo "Install successful."

ensure_root :
	$(if $(shell whoami | grep root),,\
	@echo 'You must be root to run "install".' && exit 1; \
	)

test :
	@$(TESTS_DIR)/RunTests.bash

clean :
	@rm -rf $(EXPORT_DIR)
	@echo "Clean successful."

$(EXPORT_DIR)/%.o : $(SOURCE_DIR)/%.cpp
	@echo "Compiling $< ..."
	@mkdir -p $(@D)
	@$(CC) $(CC_FLAGS) -c $< -o $@
	@ar crsv $(EXPORT_DIR)/$(STATIC_LIB_NAME) $@
	@echo

.PHONY : all install ensure_root test clean
