/*
 * Autor: Frantisek Matecny (xmatec00)
 * Datum: 7.5.2017
 * Soubor: ahed.h
 * Komentar:
 */  

#ifndef __KKO_AHED_H__
#define __KKO_AHED_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <inttypes.h>

#define AHEDOK 0
#define AHEDFail -1

/* Datovy typ zaznamu o (de)kodovani */
typedef struct{
	/* velikost nekodovaneho retezce */
	int64_t uncodedSize;
	/* velikost kodovaneho retezce */
	int64_t codedSize;
} tAHED;


typedef struct TNode {
	uint16_t letter;
	uint64_t count;
	uint16_t code;
	uint16_t number;
	struct TNode * parentPtr;
	struct TNode * LPtr;
	struct TNode * RPtr;
} *TNodePtr;	

void print_tree(TNodePtr tree);
void dispose_tree (TNodePtr *RootPtr);
void alloc_subtree(TNodePtr arr[], uint16_t letter);
int find_letter (TNodePtr arr[], uint16_t c);
void actualize_tree(uint16_t letter, TNodePtr arr[]);
void save_bits(TNodePtr arr[], uint16_t c, FILE **outputFile, tAHED *ahed);

/* Nazev:
 *   AHEDEncoding
 * Cinnost:
 *   Funkce koduje vstupni soubor do vystupniho souboru a porizuje zaznam o kodovani.
 * Parametry:
 *   ahed - zaznam o kodovani
 *   inputFile - vstupni soubor (nekodovany)
 *   outputFile - vystupní soubor (kodovany)
 * Navratova hodnota: 
 *    0 - kodovani probehlo v poradku
 *    -1 - pøi kodovani nastala chyba
 */
int AHEDEncoding(tAHED *ahed, FILE *inputFile, FILE *outputFile);


/* Nazev:
 *   AHEDDecoding
 * Cinnost:
 *   Funkce dekoduje vstupni soubor do vystupniho souboru a porizuje zaznam o dekodovani.
 * Parametry:
 *   ahed - zaznam o dekodovani
 *   inputFile - vstupni soubor (kodovany)
 *   outputFile - vystupní soubor (nekodovany)
 * Navratova hodnota: 
 *    0 - dekodovani probehlo v poradku
 *    -1 - pøi dekodovani nastala chyba
 */
int AHEDDecoding(tAHED *ahed, FILE *inputFile, FILE *outputFile);

#endif

