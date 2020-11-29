#ifndef FANO_CODE_H
#define FANO_CODE_H
/*      GDI Chapter 3: Encoding exercise       
*       27 - 11 - 2020
*       ...where are you dum dum.....
*/

/*
*      3. FanoCode.h FanoCode.c -> takes symbol alphabet table to 
*       create a code table, ecodes and decodes messages according to the fano algorithm
*/
#include "SymbolAlphabet.h"
#include "FileIO.h"
#include <memory.h>
#include "CodeTree.h"

#define FC_CSIZE 64
//codewords are stored as one string for each symbol
//the size of string/array is limited to FC_CSIZE aka fano code, code size

typedef struct fanocode
{
    size_t size;    //size of alphabet
    size_t csize;   //max size of code string  ///field fc->csize is redundant
    char* x;        //array of alphabet characters in sorted order
    double* px;     //array of probabilities of p
    double* sp;     //sum of probabilities
    char** code;    //array of strings of the code Bits
    size_t* idx;     //index of code bit string end idx[i]-1 == length of final code word
    double L;       //average word length
    double H;       //alphabet entropy
    double Rc;      //code redundancy 
}fanocode;


static inline fanocode def_fanocode()
{
    fanocode fc;
    fc.size = 0;
    fc.csize = 0;
    fc.x = 0;
    fc.px = 0;
    fc.sp = 0;
    fc.code = 0;
    fc.idx = 0;
    fc.L = 0;
    fc.H = 0;
    fc.Rc = 0;
    return fc;
}

int create_fanocode(const symbolal* ca, fanocode* fc);
//Function to fill out fano code struct, and to create fano codes for each char
//precondition: fc is not defined/initialized

void print_fano(fanocode* fc);
//Function to print the information stored in
//the fano struct, list of symbols and their codewords.

static inline void clear_fano(fanocode* fc)
//Function to realease memory used by strings and arrays
//in the fano struct
{
    if(fc->code != 0)
    {
        for (size_t i = 0; i < fc->size; ++i)
        {
            if(fc->code[i] != 0)free(fc->code[i]);
        }
    }
    free(fc->code);
    if(fc->idx) free(fc->idx);
    if (fc->sp) free(fc->sp);
    if (fc->px) free(fc->px);
    if (fc->x) free(fc->x);
    fc->size = 0;
    fc->csize = 0;
    fc->code = 0;
    fc->L = 0;
    fc->H = 0;
    fc->Rc = 0;
}

int encode_msg_fano(sfile* filehandler, fanocode* fc);
//Function:             takes the message stored in filehandler,
//                      and translates it into a coded message using
//                      the codetable of fc, the encoded message
//                      is also stored in filehandler's outmessage field
//                      The encoded message has at the begginning:
//                      A marker for which algorithm was used F for fano
//                      the symbol table and the corresponding codewords
//                      like this: 
//                      F[A][B][I][y][110][111][011][01]
//                      11011011101101 


int decode_msg_fano(sfile* file, fanocode* fc, codetree* ct);
//Function:             Decodes a message encoded with the fano algorithm
//Precondition:         fc is zero initialized/default initailized
//                      the entire encoded message file's contents are stored
//                      in file->message in the following format, on the first line:
//                      F[A][B][C][0][10][11]
//                      000010111110
//                      a marker for which algorithm was used, the symbols and their codewords
//                      on the second line the stream of bits of the message

//                      codetree ct is zero initialized!

//Return value:         -1 if fc is not zero initialized or codetree is not zero initialized
//                      -2 if file->message is empty or 0 pointer
//                      -3 if the F marker is missing
//                      -4 on a parse error of the symbols and their codewords
//                      -5 on memory allocation errors
//                      -6 if codeword length FC_CSIZE is exceeded for fc->idx[i]
//Effect:               the decoded message is written to file->outmessage
//                      fc will have its symbol table, and codestrings filled out
//                      the other fields are left zero initialized

#endif