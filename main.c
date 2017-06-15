/*
 * Autor: Frantisek Matecny (xmatec00)
 * Datum: 7.5.2017
 * Soubor: main.c
 * Komentar:
 */ 


#include <string.h>
#include <getopt.h>
#include "ahed.h"

bool encoding = false;;
bool decoding = false;


/**
 * Napoveda
 **/
void help_msg() {
	
	printf(	"Napoveda:\n"
			"-i <ifile>\t- vstupny subor\n"
			"-o <ofile>\t- vystupny subor\n"
			"-l <logfile>\t- subor vystupnej spravy\n"
			"-c \t\t- aplikacia bude vstupny subor kodovat\n"
			"-x \t\t- aplikacia bude vstupny subor dekodovat\n"
			"-h \t\t- vypis napovedy\n");
}


/**
 * Nacitanie argumentov programu
 **/
void load_arguments(int argc, char** argv, FILE** iF, FILE** oF, FILE** lF) {
	
	// test poctu argumentov
	if(argc < 2)
	{
		 fprintf(stderr, "Zly pocet argumentov\n");
		 help_msg();
		 exit(-1);
	 }
	
	int c;	
	
	// spracovanie argumentov programu
	while ((c = getopt(argc, argv, "i:o:l:cxh")) != -1)
	{	
		switch (c) 
		{		
		// vstupny subor
		case 'i':	*iF = fopen(optarg, "r");
					if(*iF == NULL){
						fprintf(stderr, "Chyba otvorenia suboru \"%s\".\n", optarg);
						exit(-1);}
					//printf("input_file: %s\n", optarg);
					break;
					
		// vystupny subor
		case 'o':	*oF = fopen(optarg, "w+");
					if(*oF == NULL){
						fprintf(stderr, "Chyba otvorenia suboru \"%s\".\n", optarg);
						exit(-1);}
					//printf("ouput_file: %s\n", optarg);
					break;
		
		// log subor
		case 'l':	*lF = fopen(optarg, "w+");
					if(*lF == NULL){
						fprintf(stderr, "Chyba otvorenia suboru \"%s\".\n", optarg);
						exit(-1);}
					//printf("log_file: %s\n", optarg);
					break;
					
		// kodovanie			
		case 'c':	encoding = true;
					break;
		
		// dekodovanie			
		case 'x':	decoding = true;
					break;
		
		// vypis napovedu			
		case 'h':	help_msg();
					exit(0);
					break;		
					
		default:	break;
		}
	}
    
    // kodovanie alebo dekodovanie
	if(encoding == decoding){
		 fprintf(stderr, "Zvolte kodovanie alebo dekodovanie.\n");
		 help_msg();
		 exit(-1);
	}	
}



/**
 * Hlavna funkcia
 **/
int main(int argc, char **argv)
{
	FILE* inF = NULL;
	FILE* outF = NULL;
	FILE* logF = NULL;
	tAHED ahed;

	// nacitanie argumentov
	load_arguments(argc, argv, &inF, &outF, &logF);

	if (inF == NULL)
		inF = stdin;
	
	if (outF == NULL)
		outF = stdout;
 
	int result;

	if(encoding){
		result = AHEDEncoding(&ahed, inF, outF);
	}
	else{
		result = AHEDDecoding(&ahed, inF, outF);
		}

	// zapis logovacieho suboru
	if (logF != NULL){
		fprintf(logF, "login = xmatec00\n");	
		fprintf(logF, "uncodedSize = %ld\n",ahed.uncodedSize);	
		fprintf(logF, "codedSize = %ld\n",ahed.codedSize);	
		}
	
	// zatvorenie suborov
	if (inF != NULL)
		fclose(inF);
		
	if (outF != NULL)	
		fclose(outF);
		
	if (logF != NULL)
		fclose(logF);


	return(result);
}
