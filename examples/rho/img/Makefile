
SOURCE_FILES = $(wildcard *.cpp)
EXECUTABLES = $(patsubst %.cpp,%.exec,$(SOURCE_FILES))


ifeq ($(shell uname),Darwin)
	LIB_FLAGS = \
		-framework GLUT \
		-framework OpenGL \
		-framework Foundation \
		-framework AVFoundation \
		-framework CoreVideo \
		-framework CoreMedia
else
	LIB_FLAGS = \
		-lglut \
		-lGLU
endif


all : $(EXECUTABLES)

%.exec : %.cpp
	$(TARGET)g++ -fvisibility=hidden -Wall -Werror -I../../../include $^ ../../../objects/librho.a $(LIB_FLAGS) -o $@
	@echo

clean :
	@rm -f *.exec
	@echo "Clean successful."

.PHONY : all clean
