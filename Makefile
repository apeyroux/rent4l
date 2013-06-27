all:
	#gcc main.c rent4l.c -lhpdf -ljansson -o rent4l
	clang -g -Wall main.c rent4l.c -lhpdf -ljansson -o rent4l

bin:
	gcc -Wall -L"/usr/local/lib" -ljansson -lm -lhpdf -o rent4l main.c rent4l.c
	#clang main.c rent4l.c -o rent4l -lhpdf -ljansson -lm

valgind:
	valgind ./rent4l

pdf:
	rm ./rapport.pdf
	#clang main.c rent4l.c -o rent4l -lhpdf -ljansson -lm
	gcc -DDEBUG -Wall -g -L"/usr/local/lib" -ljansson -lm -lhpdf -o rent4l main.c rent4l.c
	./rent4l -t tpl/quittance.json -o rapport.pdf
	zathura ./rapport.pdf
