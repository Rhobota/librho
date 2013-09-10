
all : test.exec

%.exec : %.cpp
	g++ -Wall -Wextra -Werror -Wno-unused-parameter -I ~/rhepo/libs/librho/include $< ~/rhepo/libs/librho/objects/librho.a -lglfw3 -lXrandr -lXi -lGL -o $@

clean :
	rm -rf *.exec

.PHONY : all clean
