#ifndef SYMBOL_ALPHABET_H
#define SYMBOL_ALPHABET_H
#include <malloc.h>
/*      GDI Chapter 3: Encoding exercise
*       This program reads a text file,
*       creates an alphabet of the message stored in
*       the file and computes the amount of Information
*       stored by each character of the file
*       this can be later used for encoding purposes
*
*       27 - 11 - 2020
*       ...where are you dum dum.....
*/

/*
*        2. CodeAlphabet.h CodeAlphabet.c->module to process messages 
*       creates table of symbols, compute information theory values
*/

//Symbol Alphabet struct holds all relevant information of the message read from file
typedef struct symbolal
{
    size_t size;
    size_t msglen;
    char* alphabet;
    double* px; // probability of each character
    double* ix; // information content of each element
    int* lx; //code length of each alphabet character, by rounding up ix, each encoding style has its own length
    double  H0; // entropy of alphabet
    double  H;  // average entropy of alphabet
    double  Rq; // Source Redundancy
    double  Rc; // Code Redundancy
    double  L;  //average word length / sum of the weighted average number of bits of the alphabet
    ///Think about how you are going to store codes, as strings of '0','1' chars??
}symbolal;

static inline symbolal def_symbol_alphabet()
//Function:     Default symbolalphabet, used to zero initialize
//              your variables like this:
//              symbolalphabet a = def_symbol_alphabet()
{
    symbolal a;
    a.size = 0;
    a.msglen = 0;
    a.alphabet = 0;
    a.px = 0;
    a.ix = 0;
    a.lx = 0;
    a.H0 = 0.0;
    a.H = 0.0;
    a.Rq = 0.0;
    a.Rc = 0.0;
    a.L = 0.0;
    return a;
}


void print_s_a(symbolal* ca);
//Function to print the information stored in a codeAlphabet struct

static inline void clear_s_a(symbolal* ca)
//Release memory used by an alphabet
{
    ca->size = 0;
    if(ca->alphabet != 0) free(ca->alphabet);
    if (ca->px != 0)free(ca->px);
    if (ca->ix != 0)free(ca->ix);
    if (ca->lx != 0)free(ca->lx);
    ca->ix = 0;
    ca->px = 0;
    ca->alphabet = 0;
}

int create_s_a(const char* msg, size_t msgsize, symbolal* ca);
//Function:         used to fill out one part of the code alphabet
//Return value:     -1 on memory allocation error
//                  1 on success

void compute_s_info_content(const char* msg, size_t msgsize, symbolal* ca);
//Function to compute the information content of each symbol



#endif