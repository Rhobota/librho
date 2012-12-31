
SOURCE_FILES = $(wildcard *.cpp)
EXECUTABLES = $(patsubst %.cpp,%.exec,$(SOURCE_FILES))


ifeq ($(shell uname),Darwin)
	LIB_FLAGS = \
		-lrho \
		-lglfw \
		-framework Cocoa \
		-framework OpenGL \
		-framework Foundation \
		-framework IOKit
else
	LIB_FLAGS = \
		-lrho \
		-lglfw \
		-lGLU
endif


all : $(EXECUTABLES)

%.exec : %.cpp
	@g++ -Wall -Werror $^ -o $@ $(LIB_FLAGS)

clean :
	@rm -f *.exec
	@echo "Clean successful."

.PHONY : all clean
