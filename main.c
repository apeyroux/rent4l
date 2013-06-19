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
    char *reportPath = NULL;
    report_t *report_p = NULL;
	
	while(1) {
		int option_index = 0;
		static struct option long_options[] = {
			{"template", required_argument, 0, 't'},
			{"report", required_argument, 0, 'o'},
			{"verbose", no_argument, 0, 'v'},
			{0, 0, 0, 0}
		};

		c = getopt_long(ac, av, "vt:o:", long_options, &option_index);

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
			case 'o' :
				if(NULL == (reportPath = malloc(sizeof(char) * strlen(optarg) + 1))) {
					printf("ERROR: cannot load report file object.\n");
					exit(EXIT_FAILURE);
				}
				strncpy(reportPath, optarg, strlen(optarg)+1);
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

	if(NULL == tplPath || NULL == reportPath)
		usage();

    report_p = (report_t *) malloc(sizeof(report_t));
    if(NULL == (report_p = initReportFromJsonTplFile(tplPath))) {
        fprintf(stderr, "errno: can't init report_p.\n");
        exit(EXIT_FAILURE);
    }

#ifdef DEBUG
    switch (report_p->type) {
        case EDF :
            printf("EDF\n");
            break;
        case WATTER :
            printf("watter report\n");
            break;
        case RENT_RECEIPT :
            printf("rent receipt report\n");
            break;
        default:
            printf("Can't find type.\n");
            break;
    }
    printf("amount: %.2f €\n", report_p->rent_amount);
    printf("--- OWNER ---\n");
    printf("%s %s %s %s\n",     report_p->owner_p->address->number, 
                                report_p->owner_p->address->street, 
                                report_p->owner_p->address->city, 
                                report_p->owner_p->address->cp);
    printf("--- TENANT ---\n");
    printf("%s\n", report_p->tenant_p->name);
    printf("%s %s %s %s\n",     report_p->tenant_p->address->number, 
                                report_p->tenant_p->address->street, 
                                report_p->tenant_p->address->city, 
                                report_p->tenant_p->address->cp);
#endif 

    report2pdf(report_p, reportPath);

    freeReport(report_p);

	exit(EXIT_SUCCESS);
}
