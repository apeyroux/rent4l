all:
	gcc -DDEBUG -Wall -g -L"/usr/local/lib" -ljansson -lm -lhpdf -o rent4l main.c rent4l.c
