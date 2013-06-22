#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <jansson.h>
#include <hpdf.h>

#include "rent4l.h"

void 
freeReport(report_t *report_p)
{
    free(report_p->owner_p->address);
    free(report_p->tenant_p->address);
    free(report_p->owner_p);
    free(report_p->tenant_p);
    free(report_p);
}

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
    if(strncmp(typeBuf, "water", strlen("edf")+1))
        report_p->type = WATTER;
    if(strncmp(typeBuf, "edf", strlen("rent_receipt")+1))
        report_p->type = RENT_RECEIPT;

    if(JSON_REAL == json_is_real(json_object_get(json_p, "rent_amount"))) {
		fprintf(stderr, "error: not match rent_amount in json template.\n");
        return NULL;
    }
    if(0.0 == (report_p->rent_amount = json_real_value(json_object_get(json_p, "rent_amount")))) {
        fprintf(stderr, "error: can't init report_p->rent_amount\n");
        return NULL;
    }

    if(JSON_REAL == json_is_real(json_object_get(json_p, "edf_amount"))) {
		fprintf(stderr, "error: not match edf_amount in json template.\n");
        return NULL;
    }
    if(0.0 == (report_p->edf_amount = json_real_value(json_object_get(json_p, "edf_amount")))) {
        fprintf(stderr, "error: can't init report_p->edf_amount\n");
        return NULL;
    }

    if(JSON_REAL == json_is_real(json_object_get(json_p, "water_amount"))) {
		fprintf(stderr, "error: not match water_amount in json template.\n");
        return NULL;
    }
    if(0.0 == (report_p->water_amount = json_real_value(json_object_get(json_p, "water_amount")))) {
        fprintf(stderr, "error: can't init report_p->water_amount\n");
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

void
draw_line (HPDF_Page page, float x, float y, const char  *label) 
{
        HPDF_Page_BeginText (page);
        HPDF_Page_MoveTextPos (page, x, y - 10);
        HPDF_Page_ShowText (page, label);
        HPDF_Page_EndText (page);

        HPDF_Page_MoveTo (page, x, y - 15);
        HPDF_Page_LineTo (page, x + 495, y - 15);
        HPDF_Page_Stroke (page);
}

void 
printAddress(HPDF_Page page, report_t *report_p){
        char buf[16];
        int len;
        char *addr;
        if((len = snprintf(buf, 16, "%s %s\n%s %s", report_p->tenant_p->address->number, report_p->tenant_p->address->street, report_p->tenant_p->address->cp,  report_p->tenant_p->address->city)))
                    if((addr = malloc((len+1)*sizeof(char))))
                        snprintf(addr, len+1, "%s %s\n%s %s", report_p->tenant_p->address->number, report_p->tenant_p->address->street, report_p->tenant_p->address->cp, report_p->tenant_p->address->city);
        HPDF_Page_TextRect(page ,55,670,530,430, addr, HPDF_TALIGN_LEFT, NULL);
}

void
printRecap(HPDF_Page page, report_t *report_p) {
    char buf[16];
    int len;
    char *rentAmount;
    char *edfAmount;
    char *waterAmount;

    if((len = snprintf(buf, 16, "- Loyer : %.2f Euro\n", report_p->rent_amount)))
        if((rentAmount = malloc((len+1)*sizeof(char))))
            snprintf(rentAmount, len+1, "- Loyer : %.2f Euro\n", report_p->rent_amount);

    if((len = snprintf(buf, 16, "- EDF : %.2f Euro\n", report_p->edf_amount)))
        if((edfAmount = malloc((len+1)*sizeof(char))))
            snprintf(edfAmount, len+1, "- EDF : %.2f Euro\n", report_p->edf_amount);

    if((len = snprintf(buf, 16, "- Eau : %.2f Euro\n", report_p->water_amount)))
        if((waterAmount = malloc((len+1)*sizeof(char))))
            snprintf(waterAmount, len+1, "- Eau : %.2f Euro\n", report_p->water_amount);

	HPDF_Page_TextRect (page ,95,380,530,430, rentAmount, HPDF_TALIGN_LEFT, NULL);
	HPDF_Page_TextRect (page ,95,360,530,430, edfAmount , HPDF_TALIGN_LEFT, NULL);
	HPDF_Page_TextRect (page ,95,340,530,430, waterAmount , HPDF_TALIGN_LEFT, NULL);
}

int
report2pdf(report_t *report_p, char *reportPath)
{
    
    HPDF_Doc pdf = NULL;
    HPDF_Page mainPage = NULL;
    HPDF_Font fontBold = NULL;
    HPDF_Font font = NULL;

	const char *fontName = NULL;
	const char *fontNameBold = NULL;
	const char *pageTitle = NULL;
    char *content = NULL;

	if ((pdf = HPDF_New(NULL, NULL)) == NULL) {
		printf ("ERROR: cannot create pdf object.\n");
		return 1;
	}
	HPDF_UseUTFEncodings(pdf); 

	if((mainPage = HPDF_AddPage(pdf)) == NULL) {
		printf("ERROR: cannot create main page object.\n");
		return 1;
	}

	if((fontName = HPDF_LoadTTFontFromFile(pdf, "/usr/share/fonts/truetype/freefont/FreeSans.ttf", HPDF_TRUE)) == NULL) {
		printf("ERROR: cannot load font object.\n");
		return 1;
	}

	if((fontNameBold = HPDF_LoadTTFontFromFile(pdf, "/usr/share/fonts/truetype/freefont/FreeSansBold.ttf", HPDF_TRUE)) == NULL) {
		printf("ERROR: cannot load font object.\n");
		return 1;
	}

	if((font = HPDF_GetFont(pdf, fontName, "UTF-8")) == NULL) {
		printf("ERROR: cannot create font object.\n");
		return 1;
	}

	if((fontBold = HPDF_GetFont(pdf, fontNameBold, "UTF-8")) == NULL) {
		printf("ERROR: cannot create font object.\n");
		return 1;
	}

	switch(report_p->type) {
		case RENT_RECEIPT:
			pageTitle = "Quittance de loyer";
            char buf[16];
            int len;
            if ((len = snprintf(buf, 16, "        Nous, %s %s bailleur du %s %s à %s (%s) reconnaissons avoir bien reçu un chèque d'un montant de %0.2f Euro de la part de %s %s afin d'acquitter le montant du loyer du mois de XXX et lui en donne quittance, sous réserve de tous nos droits. Ce montant ce réparti de la façon suivante :", report_p->owner_p->firstname,  
                            report_p->owner_p->name, 
                            report_p->tenant_p->address->number,
                            report_p->tenant_p->address->street,
                            report_p->tenant_p->address->city,
                            report_p->tenant_p->address->cp,
                            report_p->rent_amount+report_p->water_amount+report_p->edf_amount, 
                            report_p->tenant_p->firstname, 
                            report_p->tenant_p->name)) >= 16) 
                if((content = malloc((len + 1) * sizeof(char))))
                    snprintf(content, len + 1, "        Nous, %s %s bailleur du %s %s à %s (%s) reconnaissons avoir bien reçu un chèque d'un montant de %0.2f Euro de la part de %s %s afin d'acquitter le montant du loyer du mois de XXX et lui en donne quittance, sous réserve de tous nos droits. Ce montant ce réparti de la façon suivante :", report_p->owner_p->firstname,  
                            report_p->owner_p->name, 
                            report_p->tenant_p->address->number,
                            report_p->tenant_p->address->street,
                            report_p->tenant_p->address->city,
                            report_p->tenant_p->address->cp,
                            report_p->rent_amount+report_p->water_amount+report_p->edf_amount, 
                            report_p->tenant_p->firstname, 
                            report_p->tenant_p->name); 
            break;
		case EDF:
            pageTitle = "Facture d'électricté";
			break;
		case WATTER:
            pageTitle = "Facture d'eau";
			break;
		default:
            pageTitle = "Facture";
			break;
	}

	HPDF_Page_SetSize(mainPage, HPDF_PAGE_SIZE_A4, HPDF_PAGE_PORTRAIT);
	/* title */
	draw_line (mainPage, 50, 780, NULL);
	HPDF_Page_SetFontAndSize(mainPage, fontBold, 24);
	HPDF_Page_BeginText(mainPage);
	HPDF_Page_TextOut(mainPage, (HPDF_Page_GetWidth(mainPage) - HPDF_Page_TextWidth (mainPage, pageTitle)) / 2, HPDF_Page_GetHeight(mainPage) - 50, pageTitle);
	HPDF_Page_EndText(mainPage);
	/* cartouche info locataire */
	HPDF_Page_SetFontAndSize(mainPage, font, 12);
	HPDF_Page_BeginText(mainPage);
	HPDF_Page_TextRect (mainPage ,55,760,530,430, "le __/__/____", HPDF_TALIGN_RIGHT, NULL);
    HPDF_Page_SetFontAndSize(mainPage, HPDF_GetFont(pdf, "Helvetica-Bold", NULL), 12);
	HPDF_Page_TextRect (mainPage ,55,700,530,430, "Adresse de la location :", HPDF_TALIGN_LEFT, NULL);
	HPDF_Page_SetFontAndSize(mainPage, font, 12);
    printAddress(mainPage, report_p);
	HPDF_Page_TextRect (mainPage ,55,550,530,200, content, HPDF_TALIGN_LEFT, NULL);
    //
    printRecap(mainPage, report_p);
    //
    HPDF_Page_SetFontAndSize(mainPage, HPDF_GetFont(pdf, "Helvetica-Bold", NULL), 12);
	HPDF_Page_TextRect (mainPage ,55,200,530,430, "le locataire", HPDF_TALIGN_LEFT, NULL);
	HPDF_Page_TextRect (mainPage ,55,200,530,430, "le bailleur", HPDF_TALIGN_RIGHT, NULL);
	HPDF_Page_SetFontAndSize(mainPage, font, 12);
	HPDF_Page_EndText(mainPage);
	draw_line (mainPage, 50, 630, NULL);

	HPDF_SaveToFile(pdf, reportPath);

    free(content);
	HPDF_Free(pdf);
	pdf = NULL;
	
    return 0;
}
