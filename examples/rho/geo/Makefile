
SOURCE_FILES = $(wildcard *.cpp)
EXECUTABLES = $(patsubst %.cpp,%.exec,$(SOURCE_FILES))


ifeq ($(shell uname),Linux)
	# Linux stuff:
	LIB_FLAGS = \
		-lrho \
		-lglfw \
		-lpthread \
		-lGLU \
		-lGL
else
ifeq ($(shell uname),Darwin)
	# OSX stuff:
	LIB_FLAGS = \
		-lrho \
		-lglfw \
		-lpthread \
		-framework Foundation \
		-framework Cocoa \
		-framework OpenGL \
		-framework IOKit
else
	# Mingw stuff:
	LIB_FLAGS = \
		-lrho \
		-lglfw \
		-lpthread \
		-lglu32 \
		-lopengl32 \
		-lws2_32 \
		-lwsock32
endif
endif


all : $(EXECUTABLES)

%.exec : %.cpp
	$(TARGET)g++ -Wall -Werror $^ $(LIB_FLAGS) -o $@

clean :
	@rm -f *.exec
	@echo "Clean successful."

.PHONY : all clean
