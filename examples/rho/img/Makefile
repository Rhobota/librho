
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

all : a.out run

a.out : tImageCapFactoryExample.cpp
	g++ tImageCapFactoryExample.cpp \
		$(LIB_FLAGS)

run :
	./a.out

clean :
	rm a.out

.PHONY : all run clean
