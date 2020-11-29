#include "Encoding.h"
#ifdef USE_OBSOLETE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Math.h>
/*      GDI Chapter 3: Encoding exercise
*       This program reads a text file,
*       creates an alphabet of the message stored in
*       the file and computes the amount of Information
*       stored by each character of the file
*       this can be later used for encoding and decoding
*
*       24 - 11 - 2020
*           ...where are you dum dum.....
*/


///     Refactor code!
//      so that reading the message file, creating the alphabet, creating the fano codetable, the huffman code table are each their own modules
//      keep the read message file in memory so that you can use it to create an encoded message file!
///     TODO: 
//      write an encoder and decoder for the fano codetable


void printcodeAlphabet(codeAlphabet* ca)
//Function to print the information stored in a codeAlphabet struct
{
    printf("Code Alphabet size: %I64u\n", ca->size);
    printf("Messge Length: %I64u\n", ca->msglen);
    printf("Alphabet:\n");
    for (size_t i = 0; i < ca->size; ++i)
    {
        putchar(ca->alphabet[i]);
        printf("\t\tp(x): ");
        printf("%.3lf ", ca->px[i]);
        printf("\t\tI(x): ");
        printf("%.3lf", ca->ix[i]);
        printf("\t\tl(x): ");
        printf("%I64u\n", ca->lx[i]);
    }
    printf("\nAlphabet entropy\t\tH0:\t%.3lf\n", ca->H0);
    printf("Average entropy\t\t\tH:\t%5.3lf\n", ca->H);
    printf("Average word length:\t\tL:\t%.3lf\n", ca->L);
    printf("Message source redundancy\tRq:\t%.3lf\n", ca->Rq);
    printf("Code redundancy:\t\tRc:\t%.3lf\n",ca->Rc);
}



void freecodeAlphabet(codeAlphabet* ca)
//Release memory used by an alphabet
{
    ca->size = 0;
    free(ca->alphabet);
    free(ca->px);
    free(ca->ix);
    free(ca->lx);
    ca->ix = 0;
    ca->px = 0;
    ca->alphabet = 0;
}

int createAlphabet(const char* msg, size_t msgsize, codeAlphabet* ca)
//Function used to fill out one part of the code alphabet
{
    //create a duplicate of msg
    //replace duplicate instances of characters with '\0'
    //count the number of unique characters
    //write unique characters to alphabet, not sorted
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
    //writing unique characters into the alphabet
    ca->size = 0;
    for (size_t i = 0; i < msgsize; ++i)
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
    while (i < msgsize && j < ca->size)
    {
        if (wmsg[i] != '\0')
        {
            ca->alphabet[j] = wmsg[i];
            ++j;
        }
        ++i;

    }
    ca->H  = 0.0;
    ca->H0 = 0.0;
    ca->Rq = 0.0;
    ca->L  = 0.0;
    ca->Rc = 0.0;
    ca->px = (double*)malloc(sizeof(double) * ca->size);
    ca->ix = (double*)malloc(sizeof(double) * ca->size);
    ca->lx = (size_t*)malloc(sizeof(size_t) * ca->size);
    if (ca->px == 0 || ca->ix == 0 || ca->lx == 0)
    {
        printf("createAlphabet(): error allocating memory!\n");
        if(ca->px != 0) free(ca->px);
        if(ca->ix != 0) free(ca->ix);
        if(ca->lx != 0) free(ca->lx);
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
    return 1;
}

void computeInfoContent(const char* msg, size_t msgsize, codeAlphabet* ca)
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
        ca->ix[i] = log10(1.0 / ca->px[i]) / ld;
        ca->H += ca->px[i] * ca->ix[i];
        ca->lx[i] = (ceil(ca->ix[i]));
        ca->L += ca->px[i] * ca->lx[i];       
    }
    ca->H0 = log10((double)ca->size) / ld;
    ca->Rq = ca->H0 - ca->H;
    ca->Rc = ca->L - ca->H;
    /* // note to avoid having separate loops, the code 
    *  // below has been stuffed in the one loop
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

int readMessage(const char* filename)
//Program Starts at this function;

{
    printf("Filename is: %s\n", filename);
    FILE* msgfile = 0;
    fopen_s(&msgfile, filename, "r");
    if (msgfile == 0)
    {
        printf("readMessage(): Error Opening file!");
        return -1;
    }

    //count the number of characters in msg file
    //to copy contents to memory
    char c;
    size_t msize = 0;
    do
    {
        c = getc(msgfile);
        ++msize;
    } while (c != EOF);
    --msize; // to remove EOF

    if (msize < 1)
    {
        fclose(msgfile);
        printf("Message file is empty.\n");
        return -1;
    }

    //copy contents of message to memory
    char* msgmem = (char*)malloc(msize);
    if (msgmem == 0)
    {
        printf("\n\treadMessage(): Error allocating memory!\n");
        return -1;
    }
    rewind(msgfile);
    for (int i = 0; i < msize; ++i)
    {

        msgmem[i] = getc(msgfile);
    }

    printf("\nMessage is:\n");
    for (int i = 0; i < msize; ++i)
    {
        putchar(msgmem[i]);
    }
    putchar('\n');
    putchar('\n');


    //create alphabet
    codeAlphabet ca;
    //create fano code off of alphabet
    fanoCode fc;
    if (createAlphabet(msgmem, msize, &ca) != -1)
    {
        //compute information content of the message
        computeInfoContent(msgmem, msize, &ca);
        createFanoCode(&ca,&fc);

        printcodeAlphabet(&ca);
        printFano(&fc);

        freecodeAlphabet(&ca);        
        freeFano(&fc);
    }
    free(msgmem);
    fclose(msgfile);
    return 1;
}

static void recurseFano(fanoCode* fc, size_t start, size_t end)
//Function to create the codewords for each symbol in the alphabet,
//Each call represents one depth of a binary tree, each codeword ends up as a leaf node
//on the code tree, if this codition is violated then decoding the message will not be possible.
//Precondition:     symbol table are sorted from highest to lowest occurence probability
//                  table of sums of probabilities is filled out correctly
{ 
    //Exit condition 1: If the start index is equal to the end index, nothing needs to be done
    //Exit condition 2: If the start and end index are 1 apart, append a 1 to the codeword of start
    //                  and append a 0 to the codeword of end
    if(start == end)
    {
        return;
    }
    else if((start - end) == 1)
    {
        fc->idx[start] += 1;    //increment size of the string holding the code
        fc->code[start][fc->idx[start] - 1] = '1';


        fc->idx[end] += 1;    //increment size of the string holding the code
        fc->code[end][fc->idx[end] - 1] = '0';
    }

    
    //compute the value middle point value, which is used to select where to split
    //the symbol table, everything on the left hand side, gets a 1 appended to the codewords
    //the right hand side gets a 0 appended to the codewords
    double splitval = fc->sp[start] / 2.0;
    //search for the smallest delta between sp and splitval
    double delta = 999999999.0;
    size_t midindex = 0;
    for(size_t i = start; i <= end; ++i)
    {
        if(fabs((fc->sp[i] - splitval)) <= fabs(delta))
        {
            delta = fc->sp[i] - splitval;
            midindex = i;
        }
        else if (fabs((fc->sp[i] - splitval)) > fabs(delta))
        {
            break;
        }
    }
    
    //Append 0 to everything to the left of midindex
    for(size_t i = start; i < midindex; ++i)
    {
        fc->idx[i] += 1;    //increment size of the string holding the code
        fc->code[i][fc->idx[i]-1] = '1';
    }
    //Apped 1 to everything to the right of midindex
    for(size_t i = midindex; i <= end; ++i)
    {
        fc->idx[i] += 1;    //increment size of the string holding the code
        fc->code[i][fc->idx[i]-1] = '0';
    }

    //Recuresively process left side of tree, and right side of tree
    recurseFano(fc,start,midindex-1);
    recurseFano(fc,midindex,end);

}

static void createFano(fanoCode* fc)
{
//1st: Sort fc symbols in decreasing order of probability
    char c = 0;
    double p = 0.0;
    for(size_t i = 0; i < fc->size; ++i)
    {
        for(size_t j = i; j <fc->size; ++j )
        {
            if(fc->px[j] >= fc->px[i])
            {
                c = fc->x[j];
                fc->x[j] = fc->x[i];
                fc->x[i] = c;

                p = fc->px[j];
                fc->px[j] = fc->px[i];
                fc->px[i] = p;
            }   
        }
    }
//2nd: Compute the sum of probabilities
    p = 0.0;
    size_t i = fc->size;
    do
    {
        --i;
        p += fc->px[i];
        fc->sp[i] = p;
    }while(i != 0);
    
//3rd: Recursive algorithm to create the fano codes for each symbol
    recurseFano(fc,0,fc->size-1);
    
}


int createFanoCode(const codeAlphabet* ca, fanoCode* fc)
//Function to fill out fano code struct, and to create fano codes for each char
//precondition: fc is not defined/initialized
{
    fc->size = ca->size;
    fc->csize = FC_CSIZE;
    fc->L = 0;
    fc->H = ca->H;
    fc->Rc = 0;
    fc->x = (char*)malloc(sizeof(char)*fc->size);
    fc->px = (double*)malloc(sizeof(double)*fc->size);
    fc->sp = (double*)malloc(sizeof(double)*fc->size);
    fc->idx =(size_t*) malloc(sizeof(size_t)*fc->size);    
    if(fc->x == 0 || fc->px == 0 || fc->sp == 0 || fc->idx == 0)
    {
       if(fc->x != 0 )
       {
            free(fc->x);
       } 
       if (fc->px != 0)
       {
           free(fc->px);
       }
       if (fc->sp != 0)
       {
           free(fc->sp);
       }
       if (fc->idx != 0)
       {
           free(fc->idx);
       }
       printf("\ncreateFanoCode(): memory allocation error byebye!\n");
       return -1;
    }


    fc->code = (char**)malloc(sizeof(char*)*fc->size);
    for(size_t i = 0; i < fc->size; ++i)
    {
        fc->code[i] = (char*)malloc(fc->csize);
        if(fc->code[i] == 0)
        {
            for(size_t j = 0; j < i; ++j)
            {
                free(fc->code[i]);
            }
            free(fc->code);
            free(fc->x);
            free(fc->px);
            free(fc->sp);
            free(fc->idx);
            printf("\ncreateFanoCode(): memory allocation error byebye!\n");
            return -1;
        }
    }
    for(size_t i = 0; i < fc->size; ++i)
    {
        for(size_t j = 0; j < fc->csize; ++j)
        {
            fc->code[i][j] = '\0';
        }
        fc->x[i] = ca->alphabet[i];
        fc->px[i] = ca->px[i];
        fc->idx[i] = 0;
    }
    createFano(fc);

    //Compute average word length
    for(size_t i = 0; i < fc->size; ++i)
    {   
        fc->L = fc->L + (fc->px[i] * fc->idx[i]);
    }
    //Compute code redundancy
    fc->Rc = fc->L - fc->H;    
    return 1;
}


void printFano(fanoCode* fc)
//Function to print the information stored in
//the fano struct, list of symbols and their codewords.
{
    printf("Number of symblos:\t%I64u\n",fc->size);
    printf("Max codeword size:\t%I64u\n", fc->csize);
    printf("Symbol\t\t\tp(x)\t\t\tsum of p(x)\t\t\tcodeword\n");
    for(int i = 0; i < fc->size; ++i)
    {
        printf("%c\t\t\t%.4lf\t\t\t%.4lf\t\t\t\t%s\n",fc->x[i],fc->px[i],fc->sp[i],fc->code[i]);
    }
    printf("Average word length\t\t\tL:%.4lf\n",fc->L);
    printf("Average Entropy\t\t\t\tH:%.4lf\n",fc->H);
    printf("Code Redundancy\t\t\t\tRc:%.4lf\n",fc->Rc);
}


void freeFano(fanoCode* fc)
//Function to realease memory used by strings and arrays
//in the fano struct
{
    for(size_t i = 0; i < fc->size; ++i)
    {
       free(fc->code[i]);
    }
    free(fc->code);
    free(fc->idx);
    free(fc->sp);
    free(fc->px);
    free(fc->x);
    fc->size = 0;
    fc->csize = 0;
    fc->code = 0;
    fc->L = 0;
    fc->H = 0;
    fc->Rc = 0;
}
#endif
