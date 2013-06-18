#include <stdio.h>
#include <stdlib.h>
#include <getopt.h> 
#include <string.h>

#include "rent4l.h"

void
usage()
{
    fprintf(stderr, "\nusage :\n\trent4l -t [path/template.json] -o [file.pdf]\n\n");
    exit(EXIT_FAILURE);
}

int
main(int ac, char **av)
{
	int c;
	char *tplPath = NULL;
	
	while(1) {
		int option_index = 0;
		static struct option long_options[] = {
			{"template", required_argument, 0, 't'},
			{"verbose", no_argument, 0, 'v'},
			{0, 0, 0, 0}
		};

		c = getopt_long(ac, av, "vt:", long_options, &option_index);

		if(c == -1)
			break;

		switch(c) {
			case 't' :
				if(NULL == (tplPath = malloc(sizeof(char) * strlen(optarg) + 1))) {
					printf("ERROR: cannot load template file object.\n");
					exit(EXIT_FAILURE);
				}
				strncpy(tplPath, optarg, strlen(optarg)+1);
				break;
			case 'v' :
				printf("verbose mode\n");
				break;
			default :
				printf("?? getopt returned character code 0%o ??\n", c);
		}
	}

	if (optind < ac) {
		printf("non-option ARGV-elements: ");
		while (optind < ac)
			printf("%s ", av[optind++]);
		printf("\n");
	}

	if(NULL == tplPath)
		usage();

    useTemplate(tplPath);

	exit(EXIT_SUCCESS);
}
