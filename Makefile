all:
	#gcc -DDEBUG -Wall -g -L"/usr/local/lib" -ljansson -lm -lhpdf -o rent4l main.c rent4l.c
	clang -g -Wall main.c rent4l.c -o rent4l -lhpdf -ljansson -lm

bin:
	#gcc -Wall -L"/usr/local/lib" -ljansson -lm -lhpdf -o rent4l main.c rent4l.c
	clang main.c rent4l.c -o rent4l -lhpdf -ljansson -lm

pdf:
	#rm ./rapport.pdf
	clang main.c rent4l.c -o rent4l -lhpdf -ljansson -lm
	#gcc -DDEBUG -Wall -g -L"/usr/local/lib" -ljansson -lm -lhpdf -o rent4l main.c rent4l.c
	./rent4l -t tpl/quittance.json -o rapport.pdf
	zathura ./rapport.pdf
