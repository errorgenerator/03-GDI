#include "SymbolAlphabet.h"
#include <stdint.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <math.h>
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

//This struct holds all relevant information of the message read from file
void print_s_a(symbolal* ca)
//Function to print the information stored in a codeAlphabet struct
{
    printf("-----------Symbol Table-----------\n");
    printf("Symbol Alphabet size: %I64u\n", ca->size);
    printf("Message Length: %I64u\n", ca->msglen);
    printf("Alphabet:\n");
    for (size_t i = 0; i < ca->size; ++i)
    {
        putchar(ca->alphabet[i]);
        printf("\t\tp(x): ");
        printf("%.3lf ", ca->px[i]);
        printf("\t\tI(x): ");
        printf("%.3lf", ca->ix[i]);
        printf("\t\tl(x): ");
        printf("%d\n", ca->lx[i]);
    }
    printf("\nAlphabet entropy\t\tH0:\t%.3lf\n", ca->H0);
    printf("Average entropy\t\t\tH:\t%5.3lf\n", ca->H);
    printf("Average word length:\t\tL:\t%.3lf\n", ca->L);
    printf("Message source redundancy\tRq:\t%.3lf\n", ca->Rq);
    printf("Code redundancy:\t\tRc:\t%.3lf\n", ca->Rc);
}

int create_s_a(const char* msg, size_t msgsize, symbolal* ca)
//Function:         used to fill out one part of the code alphabet
//Return value:     -1 on memory allocation error
//                  1 on success
{
    //create a duplicate of msg
    //filter out duplicate characters by replacing them with '\0'
    //count the number of unique characters
    //write unique characters to alphabet, in order of occurence
    char* wmsg = (char*)malloc(msgsize);
    if (wmsg == 0)
    {
        printf("createAlphabet(): error allocating memory!\n");
        return -1;
    }
    memcpy_s(wmsg, msgsize, msg, msgsize);
    for (size_t i = 0; i < msgsize; ++i)
    {
        if (wmsg[i] == '\0') continue;
        for (size_t j = i + 1; j < msgsize; ++j)
        {
            if (wmsg[i] == wmsg[j])
            {
                wmsg[j] = '\0';
            }
        }
    }
    //store symbols filtered out above
    ca->size = 0;
    for (size_t i = 0; i < msgsize; ++i)    //count how many unique symbols there are
    {
        if (wmsg[i] != '\0') ++ca->size;
    }
    ca->alphabet = (char*)malloc(ca->size);
    if (ca->size == 0 || ca->alphabet == 0)
    {
        printf("createAlphabet(): error allocating memory!\n");
        free(wmsg);
        return -1;
    }
    ca->msglen = msgsize;
    size_t i = 0;
    size_t j = 0;
    while (i < msgsize && j < ca->size) //copy symbols to alphabet struct
    {
        if (wmsg[i] != '\0')
        {
            ca->alphabet[j] = wmsg[i];
            ++j;
        }
        ++i;

    }
    //default initialize members 
    ca->H = 0.0;
    ca->H0 = 0.0;
    ca->Rq = 0.0;
    ca->L = 0.0;
    ca->Rc = 0.0;
    //properties of each symbol are implicitly linked together by
    //their index in the arrays, so symbol[0] has probability px[0] informationcontent ix[0]
    //and a potential codelength lx[0]
    ca->px = (double*)malloc(sizeof(double) * ca->size);
    ca->ix = (double*)malloc(sizeof(double) * ca->size);
    ca->lx = (int*)malloc(sizeof(int) * ca->size);
    if (ca->px == 0 || ca->ix == 0 || ca->lx == 0)
    {
        printf("createAlphabet(): error allocating memory!\n");
        if (ca->px != 0) free(ca->px);
        if (ca->ix != 0) free(ca->ix);
        if (ca->lx != 0) free(ca->lx);
        free(wmsg);
        return -1;
    }
    for (size_t i = 0; i < ca->size; ++i)
    {
        ca->px[i] = 0.0;
        ca->ix[i] = 0.0;
        ca->lx[i] = 0;
    }
    free(wmsg);
    wmsg = 0;
    compute_s_info_content(msg,msgsize,ca);
    return 1;
}

void compute_s_info_content(const char* msg, size_t msgsize, symbolal* ca)
//Function to compute the information content of each letter of the message
//Precondition:         ca has been initialized and all its array members,
//                      point to valid memory and their contents have also been set to 0
{
    //compute the probability of each letter of the alphabet p(x)
    //by the number of times each letter repeats in the alphabet
    //and dividing that by the size of the message
    for (size_t i = 0; i < ca->size; ++i)
    {
        for (size_t j = 0; j < msgsize; ++j)
        {
            if (msg[j] == ca->alphabet[i]) ca->px[i] += 1.0;
        }
        ca->px[i] /= (double)msgsize;
    }

    //Now compute the statistical information content of each letter I(x)
    //using the formula log(1.0/p(x)) / log(2) so the base 2 logarithm of the reciprocal
    //of the probability of each letter ld(1/p(x))
    double ld = log10(2);
    for (size_t i = 0; i < ca->size; ++i)
    {
        //ca->ix[i] = log10(1.0 / ca->px[i]) / ld; //since p(x) is 0.0 - 1.0 log of it would be negative
                                                    //that is why we take the log of the reciprocal
        ca->ix[i] = (log10(ca->px[i]) / ld)*-1.0;  //is this faster? one division operation less?
        ca->H += ca->px[i] * ca->ix[i];
        ca->lx[i] = (int)(ceil(ca->ix[i]));
        ca->L += ca->px[i] * ca->lx[i];
    }
    ca->H0 = log10((double)ca->size) / ld;
    ca->Rq = ca->H0 - ca->H;
    ca->Rc = ca->L - ca->H;
    /* // note to avoid having separate loops, the code
    *  // below has been stuffed in the one loop above
    * //
    //Compute the Source Entropy H0
    //ld(n) so log base 2 of number of letters in the alphabet
    ca->H0 = log10((double)ca->size) / ld;

    //compute the average entropy of the alphabet
    // sum from i=0 to n p(x)*i(x)
    ca->H = 0;
    for (size_t i = 0; i < ca->size; ++i)
    {
        ca->H += ca->px[i] * ca->ix[i];
    }
    //compute message source redundancy
    //H0 - H
    ca->Rq = ca->H0 - ca->H;

    //convert Information content to a code length in Bit
    for (size_t i = 0; i < ca->size; ++i)
    {
        ca->lx[i] = (byte)(ceil(ca->ix[i]));
    }
    //compute average word length
    for (size_t i = 0; i < ca->size; ++i)
    {
        ca->L += ca->px[i] * ca->lx[i];
    }
    */
}
