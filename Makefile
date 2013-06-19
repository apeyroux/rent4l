all:
	gcc -DDEBUG -Wall -g -L"/usr/local/lib" -ljansson -lm -lhpdf -o rent4l main.c rent4l.c

pdf:
	#rm ./rapport.pdf
	gcc -DDEBUG -Wall -g -L"/usr/local/lib" -ljansson -lm -lhpdf -o rent4l main.c rent4l.c
	./rent4l -t tpl/quittance.json -o rapport.pdf
	zathura ./rapport.pdf
