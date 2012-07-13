
INCLUDE_DIR := include
SOURCE_DIR  := source
OBJECTS_DIR := objects
TESTS_DIR   := tests

STATIC_LIB_NAME := librho.a

CC := g++
CC_FLAGS := -g -rdynamic -Wall -Werror -I $(INCLUDE_DIR)

SRC_FILES := $(shell find $(SOURCE_DIR) -name '*.cpp' -type f)
OBJ_FILES := $(patsubst $(SOURCE_DIR)/%.cpp,$(OBJECTS_DIR)/%.o,$(SRC_FILES))

all : $(OBJ_FILES)

install : ensure_root all
	@echo
	@cp -r $(INCLUDE_DIR)/* /usr/local/include
	@cp $(OBJECTS_DIR)/$(STATIC_LIB_NAME) /usr/local/lib
	@echo "Install successful."

ensure_root :
	$(if $(shell whoami | grep root),,\
	@echo 'You must be root to run "install".' && exit 1; \
	)

test :
	@$(TESTS_DIR)/RunTests.bash

clean :
	@rm -rf $(OBJECTS_DIR)
	@echo "Clean successful."

$(OBJECTS_DIR)/%.o : $(SOURCE_DIR)/%.cpp
	@echo "Compiling $< ..."
	@mkdir -p $(@D)
	@$(CC) $(CC_FLAGS) -c $< -o $@
	@ar crsv $(OBJECTS_DIR)/$(STATIC_LIB_NAME) $@
	@echo

.PHONY : all install ensure_root test clean
