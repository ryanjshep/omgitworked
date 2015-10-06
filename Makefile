CFLAGS = -I ./include
##LIB = ./libggfonts.so
LFLAGS =$(LIB) -lrt -lX11 -lGLU -lGL -pthread -lm #-lXrandr

all: lab1

lab1: lab1.cpp
		g++ $(CFLAGS) lab1.cpp libggfonts.a -Wall $(LFLAGS) -olab1 -lX11 -lGL -lGLU -lm

clean:
		rm -f lab1
		rm -f *.o
