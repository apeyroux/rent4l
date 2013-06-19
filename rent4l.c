#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jansson.h>

#include "rent4l.h"

report_t *
initReportFromJsonTplFile(char *tplPath)
{
    json_t *json_p = NULL;
   	json_error_t jsonError;
    report_t *report_p = NULL;
    person_t *owner_p = NULL;
    person_t *tenant_p = NULL;
    const char *typeBuf = NULL;
    
    if(NULL == (json_p = malloc(sizeof(json_p)))) {
        fprintf(stderr, "error: can't malloc json_p\n");
        return NULL;
    }
    json_p = json_load_file(tplPath, 0, &jsonError);

	if(!json_p) {
		fprintf(stderr, "error: %s line : %d column : %d [%s]\n", jsonError.source, 
                                                                    jsonError.line, 
                                                                    jsonError.column, 
                                                                    jsonError.text);
		return NULL;
	}

    if(NULL == (report_p = (report_t *) malloc(sizeof(report_t)))) {
        fprintf(stderr, "error: can't malloc report_p\n");
        return NULL;
    }

    if(JSON_STRING == json_is_string(json_object_get(json_p, "type"))) {
		fprintf(stderr, "error: not match type in json template.\n");
        return NULL;
    }
    if(NULL == (typeBuf = json_string_value(json_object_get(json_p, "type")))) {
        fprintf(stderr, "error: can't init report_p->type\n");
        return NULL;
    }

    if(strncmp(typeBuf, "edf", strlen("edf")+1))
        report_p->type = EDF;
    if(strncmp(typeBuf, "watter", strlen("edf")+1))
        report_p->type = WATTER;
    if(strncmp(typeBuf, "edf", strlen("rent_receipt")+1))
        report_p->type = RENT_RECEIPT;

    if(JSON_REAL == json_is_real(json_object_get(json_p, "amount"))) {
		fprintf(stderr, "error: not match amount in json template.\n");
        return NULL;
    }
    if(0.0 == (report_p->amount = json_real_value(json_object_get(json_p, "amount")))) {
        fprintf(stderr, "error: can't init report_p->amount\n");
        return NULL;
    }

    owner_p = initPersonFromJsonTpl(json_p, OWNER);
    tenant_p = initPersonFromJsonTpl(json_p, TENANT);

    report_p->owner_p = owner_p;
    report_p->tenant_p = tenant_p;

    return report_p;
}

person_t *
initPersonFromJsonTpl(json_t *json_p, person_type_t type)
{
    char *jsonTypePath = NULL;
    person_t *person_p = NULL;

    if(NULL == (person_p = (person_t *) malloc(sizeof(person_t)))) {
        fprintf(stderr, "error: can't malloc person_p.\n");
        return NULL;
    }

    if(NULL == (person_p->address = (address_t *) malloc(sizeof(address_t )))) {
        fprintf(stderr, "error: can't malloc address_p.\n");
        return NULL;
    }
   
    switch (type) {
        case OWNER :
            if(NULL == (jsonTypePath = (char *) malloc(strlen("owner")+1))) {
                fprintf(stderr, "error: can't malloc jsonTypePath\n");
            }
            strcpy(jsonTypePath, "owner");
            person_p->type = OWNER;
            break;
        case TENANT :
            if(NULL == (jsonTypePath = (char *) malloc(strlen("tenant")+1))) {
                fprintf(stderr, "error: can't malloc jsonTypePath\n");
            }
            strcpy(jsonTypePath, "tenant");
            person_p->type = TENANT;
            break;
        default:
            fprintf(stderr, "error: person type note found in init person from json.\n");
            return NULL;
    }

    //champ address/number
   if(JSON_STRING == json_is_string(json_object_get(json_object_get(json_object_get(json_p, jsonTypePath), "address"), "number"))) {
		fprintf(stderr, "error: not match %s/number in json template.\n", jsonTypePath);
        return NULL;
    }
    if(NULL == (person_p->address->number = json_string_value(json_object_get(json_object_get(json_object_get(json_p, jsonTypePath), "address"), "number")))) {
        fprintf(stderr, "error: can't init person_p->address->number\n");
        return NULL;
    }

    //champ address/street
   if(JSON_STRING == json_is_string(json_object_get(json_object_get(json_object_get(json_p, jsonTypePath), "address"), "street"))) {
		fprintf(stderr, "error: not match %s/street in json template.\n", jsonTypePath);
        return NULL;
    }
    if(NULL == (person_p->address->street = json_string_value(json_object_get(json_object_get(json_object_get(json_p, jsonTypePath), "address"), "street")))) {
        fprintf(stderr, "error: can't init person_p->address->street\n");
        return NULL;
    }

    //champ address/city
   if(JSON_STRING == json_is_string(json_object_get(json_object_get(json_object_get(json_p, jsonTypePath), "address"), "city"))) {
		fprintf(stderr, "error: not match %s/city in json template.\n", jsonTypePath);
        return NULL;
    }
    if(NULL == (person_p->address->city = json_string_value(json_object_get(json_object_get(json_object_get(json_p, jsonTypePath), "address"), "city")))) {
        fprintf(stderr, "error: can't init person_p->address->city\n");
        return NULL;
    }

   //champ address/cp
   if(JSON_STRING == json_is_string(json_object_get(json_object_get(json_object_get(json_p, jsonTypePath), "address"), "cp"))) {
		fprintf(stderr, "error: not match %s/cp in json template.\n", jsonTypePath);
        return NULL;
    }
    if(NULL == (person_p->address->cp = json_string_value(json_object_get(json_object_get(json_object_get(json_p, jsonTypePath), "address"), "cp")))) {
        fprintf(stderr, "error: can't init person_p->address->cp\n");
        return NULL;
    }

    //champ name
    if(JSON_STRING == json_is_string(json_object_get(json_object_get(json_p, jsonTypePath), "name"))) {
		fprintf(stderr, "error: not match %s/name in json template.\n", jsonTypePath);
        return NULL;
    }
    if(NULL == (person_p->name = json_string_value(json_object_get(json_object_get(json_p, jsonTypePath), "name")))) {
        fprintf(stderr, "error: can't init person_p->name\n");
        return NULL;
    }

    //champ firstname
    if(JSON_STRING == json_is_string(json_object_get(json_object_get(json_p, jsonTypePath), "firstname"))) {
		fprintf(stderr, "error: not match %s/firstname in json template.\n", jsonTypePath);
        return NULL;
    }
    if(NULL == (person_p->firstname = json_string_value(json_object_get(json_object_get(json_p, jsonTypePath), "firstname")))) {
        fprintf(stderr, "error: can't init person_p->firstname\n");
        return NULL;
    }

    //champ tel
    if(JSON_STRING == json_is_string(json_object_get(json_object_get(json_p, jsonTypePath), "tel"))) {
		fprintf(stderr, "error: not match %s/tel in json template.\n", jsonTypePath);
        return NULL;
    }
    if(NULL == (person_p->tel = json_string_value(json_object_get(json_object_get(json_p, jsonTypePath), "tel")))) {
        fprintf(stderr, "error: can't init person_p->tel\n");
        return NULL;
    }

    //champ mail
    if(JSON_STRING == json_is_string(json_object_get(json_object_get(json_p, jsonTypePath), "mail"))) {
		fprintf(stderr, "error: not match %s/mail in json template.\n", jsonTypePath);
        return NULL;
    }
    if(NULL == (person_p->mail = json_string_value(json_object_get(json_object_get(json_p, jsonTypePath), "mail")))) {
        fprintf(stderr, "error: can't init person_p->mail\n");
        return NULL;
    }

    return person_p;
}
