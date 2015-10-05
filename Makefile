all: lab1

lab1: lab1.cpp
		g++ lab1.cpp -Wall -olab1 -lX11 -lGL -lGLU -lm

clean:
		rm -f lab1
		rm -f *.o
