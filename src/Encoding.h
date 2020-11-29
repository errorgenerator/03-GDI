#ifndef ENCODING_H
#define ENCODING_H
#ifdef USE_OBSOLETE
#include "..\TypeUtil.h"
//
//  Obsolete file, moved to symbolalphabet.h and fanocode.h
//

/*      GDI Chapter 3: Encoding exercise 
*       This program reads a text file,
*       creates an alphabet of the message stored in
*       the file and computes the amount of Information
*       stored by each character of the file
*       this can be later used for encoding purposes
* 
*       24 - 11 - 2020 
*           ...where are you dum dum.....
*/

//This struct holds all relevant information of the message read from file
typedef struct codeAlphabet
{
    size_t size;
    size_t msglen;
    char* alphabet;
    double* px; // probability of each character
    double* ix; // information content of each element
    size_t*   lx; //code length of each alphabet character
    double  H0; // entropy of alphabet
    double  H;  // average entropy of alphabet
    double  Rq; // Source Redundancy
    double  Rc; // Code Redundancy
    double  L;  //average word length / sum of the weighted average number of bits of the alphabet
    ///Think about how you are going to store codes, as strings of '0','1' chars??

}codeAlphabet;

#define FC_CSIZE 64
typedef struct fanoCode
{
    size_t size;    //size of alphabet
    size_t csize;   //max size of code string
    char* x;        //array of alphabet characters in sorted order
    double* px;     //array of probabilities of p
    double* sp;     //sum of probabilities
    char** code;    //array of strings of the code Bits
    size_t*   idx;     //index of code bit string end idx[i]-1 == length of final code word
    double L;       //average word length
    double H;       //alphabet entropy
    double Rc;      //code redundancy 
}fanoCode;

void printcodeAlphabet(codeAlphabet* ca);
//Function to print the information stored in a codeAlphabet struct

void freecodeAlphabet(codeAlphabet* ca);
//Release memory used by an alphabet

int createAlphabet(const char* msg, size_t msgsize, codeAlphabet* ca);
//Function used to fill out one part of the code alphabet

void computeInfoContent(const char* msg, size_t msgsize, codeAlphabet* ca);
//Function to compute the information content of each letter of the message

int readMessage(const char* filename);
//Program Starts at this function

int createFanoCode(const codeAlphabet* ca, fanoCode* fc);
//Function to fill out fano code struct, and to create fano codes for each char
//precondition: fc is not defined/initialized

void printFano(fanoCode* fc);
//Function to print the information stored in
//the fano struct, list of symbols and their codewords.

void freeFano(fanoCode* fc);
//Function to realease memory used by strings and arrays
//in the fano struct

#endif
#endif