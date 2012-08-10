
SOURCE_FILES = tImageCapFactoryExample.cpp
OBJECT_FILE = viewer


ifeq ($(shell uname),Darwin)
	LIB_FLAGS = \
		-lrho \
		-framework GLUT \
		-framework OpenGL \
		-framework Foundation \
		-framework AVFoundation \
		-framework CoreVideo \
		-framework CoreMedia
else
	LIB_FLAGS = \
		-lrho \
		-lglut \
		-lGLU
endif


all : $(OBJECT_FILE) run clean

$(OBJECT_FILE) : $(SOURCE_FILES)
	@g++ $^ -o $@ $(LIB_FLAGS)

run :
	-@./$(OBJECT_FILE)

clean :
	@rm -f $(OBJECT_FILE)
	@echo "Clean successful."

.PHONY : all run clean
