
SOURCE_FILES = $(wildcard *.cpp)
EXECUTABLES = $(patsubst %.cpp,%.exec,$(SOURCE_FILES))


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


all : $(EXECUTABLES)

%.exec : %.cpp
	@g++ -Wall -Werror $^ -o $@ $(LIB_FLAGS)

clean :
	@rm -f *.exec
	@echo "Clean successful."

.PHONY : all clean
