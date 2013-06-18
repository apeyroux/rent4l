#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jansson.h>

#include "rent4l.h"

int
useTemplate(char *tplPath)
{
    json_t *json_p = NULL;
   	json_error_t jsonError;
    person_t *owner_p;
    
    if(NULL == (json_p = malloc(sizeof(json_p)))) {
        fprintf(stderr, "error: can't malloc json_p\n");
        return -1;
    }
    json_p = json_load_file(tplPath, 0, &jsonError);

	if(!json_p) {
		fprintf(stderr, "error: %s line : %d column : %d [%s]\n", jsonError.source, 
                                                                    jsonError.line, 
                                                                    jsonError.column, 
                                                                    jsonError.text);
		return -1;
	}

    owner_p = initPersonFromJsonTpl(json_p, OWNER);

#ifdef DEBUG
    printf("%d name:%s\n", owner_p->type, owner_p->name);
    printf("%d number:%s\n", owner_p->type, owner_p->address->number);
#endif

    free(owner_p->address);
    free(owner_p); //TODO:faire un freePerson pour free les char*

    return 0;
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
