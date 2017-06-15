/*
 * Autor: Frantisek Matecny (xmatec00)
 * Datum: 7.5.2017
 * Soubor: ahed.c
 * Komentar:
 */ 

#include "ahed.h"

#define ESCcode 256
#define NOLetter 257

uint8_t count_bits = 0;
uint8_t bits = 0;


/**
 * Pomocna funkcia na vypis stromu
 **/
void print_tree(TNodePtr tree){
	
	printf("\t\t\tl: %d, cet: %lu, code: %d\n", tree->letter, tree->count, tree->code);
	printf("l: %d, cet: %lu, code: %d", tree->LPtr->letter, tree->LPtr->count, tree->LPtr->code);	
	printf("\t\t\tl: %d, cet: %lu, code: %d\n", tree->RPtr->letter, tree->RPtr->count, tree->RPtr->code);
	
	if (tree->LPtr->LPtr != NULL)
		print_tree(tree->LPtr);

	if (tree->RPtr->LPtr != NULL)
		print_tree(tree->RPtr);	
}


/**
 * Zrusenie celeho stromu
 **/
void dispose_tree (TNodePtr *RootPtr) {	
	
	if(*RootPtr != NULL)
	{
		dispose_tree (&(*RootPtr)->LPtr);
		dispose_tree (&(*RootPtr)->RPtr);
		(*RootPtr)->parentPtr = NULL;
		free(*RootPtr);
		*RootPtr = NULL;
	}
	else return;

}


/**
 * Vytvorenie podstromu s uzlom ESC a nacitanim znakom
 **/
void alloc_subtree(TNodePtr arr[], uint16_t letter){
	
	TNodePtr parent = arr[ESCcode];
	
	parent->letter = NOLetter;
	
	// alokacia pamati
	parent->LPtr = malloc(sizeof(struct TNode));
	if (parent->LPtr == NULL) exit(-1);
	
	parent->RPtr = malloc(sizeof(struct TNode));
	if (parent->RPtr == NULL) exit(-1);
	
	// nastavenie premennych v lavom synovi
	parent->LPtr->letter = ESCcode;
	parent->LPtr->code = 0;
	parent->LPtr->count = 0;
	parent->LPtr->number = parent->number + 2;
	parent->LPtr->parentPtr = parent;
	parent->LPtr->LPtr = NULL;
	parent->LPtr->RPtr = NULL;
	
	// nastavenie premennych v pravom synovi
	parent->RPtr->letter = letter;
	parent->RPtr->code = 1;
	parent->RPtr->count = 0;
	parent->RPtr->number = parent->number + 1;
	parent->RPtr->parentPtr = parent;
	parent->RPtr->LPtr = NULL;
	parent->RPtr->RPtr = NULL;

	// ulozenie uzlov do pola
	arr[NOLetter+parent->number] = parent;
	arr[ESCcode] = parent->LPtr;
	arr[letter] = parent->RPtr;

}

/**
 * Funckia vyhlada znak v strome. Pri uspechu vrati true, inak false
 **/
int find_letter (TNodePtr arr[], uint16_t c) {
	
	// prejde cele pole
	for (int i = 0; i < 2*NOLetter; i++)
	{
		if (arr[i] != NULL)
		{
			if (arr[i]->letter == c)
				return true;
		}
	}	
	return false;
			   
}


/**
 * Aktualizacia stromu
 **/
void actualize_tree(uint16_t letter, TNodePtr arr[]){

	TNodePtr change_node;
	TNodePtr current_node = arr[letter];

	// od listu az ku korenu
	while (current_node->parentPtr != NULL)
	{
		change_node = current_node;
		for (int i = 0; i < 2*NOLetter; i++)
		{				
			if ((arr[i]) != NULL)
			{
				if (arr[i]->count == current_node->count && 
					arr[i]->number < current_node->number &&
					arr[i]->number < change_node->number &&
					current_node->parentPtr != arr[i])
				{
					change_node = arr[i];
				}

			}	
			
		}

		// vymen change_node a current uzli
		if (change_node != current_node)
		{
			
			TNodePtr tmp_parent = current_node->parentPtr;
			uint16_t tmp_number = current_node->number;
			uint16_t tmp_code = current_node->code;

			if(current_node->parentPtr->RPtr == current_node)
				current_node->parentPtr->RPtr = change_node;
			else
				current_node->parentPtr->LPtr = change_node;

			if(change_node->parentPtr->RPtr == change_node)
				change_node->parentPtr->RPtr = current_node;
			else
				change_node->parentPtr->LPtr = current_node;

			current_node->parentPtr = change_node->parentPtr;
			current_node->number = change_node->number;
			current_node->code = change_node->code;
			
			change_node->parentPtr = tmp_parent;
			change_node->number = tmp_number;
			change_node->code = tmp_code;
		}
		
		current_node->count++;
		current_node = current_node->parentPtr;
		
	}
	
	// zvysenie pocetnosi 
	current_node->count++;
	
}


/**
 * Ulozenie bitov do pola
 * ak je bitov 8 napisu sa do suboru
 **/
void save_bits(TNodePtr arr[], uint16_t c, FILE **outputFile, tAHED *ahed){
	
	// uloz nacitani znak
	if (arr == NULL)
	{
		uint16_t tmp = 0;
		
		bits = bits << (8-count_bits);
		tmp = c >> count_bits;
		bits = bits | tmp;

		// zapis
		fprintf(*outputFile, "%c", bits);
		ahed->codedSize++;
		
		
		bits = 0;
		bits = (c << (8-count_bits)) >> (8-count_bits);
		
	}
	else 	// uloz kod znaku v strome
	{
		TNodePtr tmp_node = arr[c];
		uint8_t arr_bits[32] = {0};				// pomocne pole pre kod znaku

		int shift = 0;
		int count_arr_bits = 0;
		int i = 0;
		
		// od listu ku korenu
		while(tmp_node->parentPtr != NULL)
		{
			arr_bits[i] = (arr_bits[i] << 1) + tmp_node->code;
			tmp_node = tmp_node->parentPtr;
			count_arr_bits++;
			
			if (count_arr_bits == 8){
				count_arr_bits = 0;
				i++;
				}
		}

		
		if(count_arr_bits == 0 && i > 0){
			i--;
			count_arr_bits = 8;
			}
		
		
		while(i >= 0)
		{
			bits = (bits << 1) + ((arr_bits[i] >> shift) & 1);
			
			shift++;
			if (count_arr_bits == shift){
				i--;
				shift = 0;
				count_arr_bits = 8;
				}
				
			count_bits++;
			
			// ak mas 8 bitov zapis
			if (count_bits == 8){
				fprintf(*outputFile, "%c", bits);
				ahed->codedSize++;
				bits = 0;
				count_bits = 0;
				}
		}
	}

}


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
int AHEDEncoding(tAHED *ahed, FILE *inputFile, FILE *outputFile)
{
	ahed->uncodedSize = 0;
	ahed->codedSize = 0;
	
	int c;
	TNodePtr tree;
	TNodePtr array[2*NOLetter] = {NULL};
	
	// inicializacia stromu
	tree = malloc(sizeof(struct TNode));
	if (tree == NULL){
		fprintf(stderr, "Chyba alokacie\n");
		return (AHEDFail);}
	
	tree->letter = ESCcode;
	tree->code = 0;
	tree->number = 0;
	tree->count = 0;
	tree->parentPtr = NULL;
	tree->LPtr = NULL;
	tree->RPtr = NULL;	
	
	array[ESCcode] = tree;

	// nacitanie vstupu po znakoch
	while((c = fgetc(inputFile)) != EOF) 
	{
		ahed->uncodedSize++;
		
		// znak je nacitani prvy krat
		if (find_letter(array, c) == false)
		{
			
			//zapis kod esc
			if (array[ESCcode]->parentPtr != NULL)
				save_bits(array, ESCcode, &outputFile, ahed);
				
			//zapis c
			save_bits(NULL, c, &outputFile, ahed);

			// vytvor novy uzol s nasledovnikmi ESC a c			
			alloc_subtree(array, c);
			
			// aktualizuj strom
			actualize_tree(c, array);
		}
		else{
			//zapis c
			save_bits(array, c, &outputFile, ahed);
			
			// aktualizuj strom
			actualize_tree(c, array);
			}
	}
	
	// zarovnanie konca suboru - vlozenie ESC kodu
	if (count_bits != 0)
	{
		save_bits(array, ESCcode, &outputFile, ahed);
		if(count_bits != 0){
			bits = bits << (8-count_bits);
			fprintf(outputFile, "%c", bits);
			ahed->codedSize++;}
	}

	//print_tree(tree);
	
	// zrusenie stromu
	dispose_tree(&tree);


	return AHEDOK;
}

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
int AHEDDecoding(tAHED *ahed, FILE *inputFile, FILE *outputFile)
{

	ahed->uncodedSize = 0;
	ahed->codedSize = 0;

	int c;
	TNodePtr tree;
	TNodePtr current_node;
	TNodePtr array[2*NOLetter] = {NULL};
	bool first = true;
	
	// inicializacia stromu
	tree = malloc(sizeof(struct TNode));
	if (tree == NULL){
		fprintf(stderr, "Chyba alokacie\n");
		return (AHEDFail);}

	tree->letter = ESCcode;
	tree->code = 0;
	tree->number = 0;
	tree->count = 0;
	tree->parentPtr = NULL;
	tree->LPtr = NULL;
	tree->RPtr = NULL;
		
	array[ESCcode] = tree;
	current_node = tree;
	
	// nacitanie vstupu po znakoch
	while((c = fgetc(inputFile)) != EOF) 
	{
		ahed->codedSize++;
		
		bits = c;
		count_bits = 0;
		while (count_bits != 8)
		{	
		count_bits++;

			// nacitavam prvy krat
			if (first){
				
				// vytvor novy uzol s nasledovnikmi ESC a c			
				alloc_subtree(array, c);
				
				// aktualizuj strom
				actualize_tree(bits, array);
					
				fprintf(outputFile, "%c", c);
				ahed->uncodedSize++;
				
				first = false;
				count_bits = 8;
				}
			else{
				
				// posun v strome
				if (((bits >> (8-count_bits)) & 1) == current_node->LPtr->code)
					current_node = current_node->LPtr;
				else
					current_node = current_node->RPtr;
						
				// v liste je ESC kod
				if(current_node->letter == ESCcode)
				{
					c = fgetc(inputFile);
					ahed->codedSize++;
					if (c == EOF){
						ahed->codedSize--;
						count_bits = 8;
						break;
						}
					bits = (bits << count_bits) + (c >> (8-count_bits));
					
					fprintf(outputFile, "%c", bits);
					ahed->uncodedSize++;
					// vytvor novy uzol s nasledovnikmi ESC a c			
					alloc_subtree(array, bits);
					
					// aktualizuj strom
					actualize_tree(bits, array);
					
					bits = c;
					current_node = tree;	
				}
				// v lste je iny znak
				else if(current_node->letter != NOLetter){
					
					// zapis
					fprintf(outputFile, "%c", current_node->letter);
					ahed->uncodedSize++;
					// aktualizuj strom
					actualize_tree(current_node->letter, array);
					
					current_node = tree;
					}

				}//else
		}//while
		
	}
		
	//print_tree(tree);

	// ztusenie stromu
	dispose_tree(&tree);
	
	return AHEDOK;
}
