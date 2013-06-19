#include <jansson.h>

#define TENANT 0
#define OWNER 1

#define EDF 0
#define WATTER 1
#define RENT_RECEIPT 2

typedef int person_type_t;
typedef int report_type_t;
typedef struct report_s report_t;
typedef struct person_s person_t;
typedef struct address_s address_t;

struct report_s {
    report_type_t type;
    double rent_amount;
    double edf_amount;
    double water_amount;
    person_t *owner_p;
    person_t *tenant_p;
};

struct person_s {
    person_type_t type;
    const char *firstname;
    const char *name;
    const char *mail;
    const char *tel;
    address_t *address;
};

struct address_s {
    const char *number;
    const char *street;
    const char *city;
    const char *cp;
};

report_t *initReportFromJsonTplFile(char *tplPath);
person_t *initPersonFromJsonTpl(json_t *json_p, person_type_t type);
int report2pdf(report_t *report_p, char *reportPath);
void freeReport(report_t *report_p);
