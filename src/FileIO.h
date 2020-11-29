#ifndef ENCODE_FILE_IO_H
#define ENCODE_FILE_IO_H
/*
*   1. FileIO.h FileIO.c->module resposible for reading in files
*   File Formats are: Simple text files, used as message source
*                     Encoded message files, contain the alphabet and codewords at the start
*   Also handles file output,
*
*
*/
#include <inttypes.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
typedef struct sstr //Simple String struct
{
    size_t smax; //maximum number of chars in string
    size_t len; //length of string including '\0', so [len-1] is '\0'
    char* str; //the string
}sstr;
static inline sstr def_sstr() //default initializer for sstr
{
    sstr a;
    a.smax = 0;     
    a.len = 0;
    a.str = 0;
    return a;
}

static inline int init_sstr(sstr* ss, size_t len)
//Function:         initialize simple string
//                  initializes simplestring struct to default values
//Return value:     0 on failure, 1 on success
{
    ss->smax = len;
    ss->len = 0;
    ss->str = (char*)malloc(sizeof(char)*len);
    if(!ss->str) return 0;
    for (size_t i = 0; i < len; ++i)
    {
        ss->str[i] = '\0';
    }
    return 1;
}
static inline void del_sstr(sstr* ss)
//Function:     release memory used by simple string
{
    ss->len = 0;
    if(ss->str != 0)
    {
        free(ss->str);
    }
}
static inline int app_sstrc(sstr* ss,const  char val)
//Function:     appends a character to simple string
//Return value: 0 on failure 1 on success
{
    if(ss->len >= ss->smax) return 0;
    if(ss->len == 0)
    {
        ss->len += 2;
        ss->str[ss->len - 2] = val;
        ss->str[ss->len - 1] = 0;

    }
    else
    {
        ss->len += 1;
        ss->str[ss->len - 2] = val;
        ss->str[ss->len - 1] = 0;

    }
    return 1;
}

int resize_sstr(sstr* ss, size_t newsize);
//Function:     resizes internal buffer of ss to previous size + newsize
//              only meant to be used for increasing the size of ss
//Return value:  0  on memory allocation failure
//               1 on success


int app_sstr(sstr* ss, const char* val);
//Function:         append to simple string
//                  appends string to simplestring     


void read_sstr(sstr* ss,const char* str,size_t lim);
//Function:     read into simple string from a c-style string  
//              copies string str into ss


static inline int rep_sstr(sstr* ss, size_t start, size_t end, char val)
//Function:         replace inside simple string
//                  replace an interval withing ss with a single character,
//Return value:     1 if successful, 0 on bounds error
{
    if(ss->len < end || start > ss->len) return 0;
    for(size_t i = start; i < end; ++i)
    {
        ss->str[i] = val;
    }
    //note: len points one index past of the \0 end
    ++ss->len;
    ss->len -= end-start;
    return 1;
}
/*
*       ------------------------------------------------
*           File Handler Type and util functions
*   
*       -------------------------------------------------
*/

typedef struct sfile
//Struct meant to hold the input and output file names, 
//handlers to those files and the contents of the input and output
{
   sstr infilename;
   sstr outfilename;
   sstr message;
   sstr outmessage;
   FILE* infile;
   FILE* outfile;

}sfile;
static inline sfile def_sfile()  //initializer function 
// use as follows: sfile a = defsfile();
{
    sfile a;
    a.infilename = def_sstr();
    a.outfilename = def_sstr();
    a.message = def_sstr();
    a.outmessage = def_sstr();
    a.infile = 0;
    a.outfile = 0;
    return a;
}
static inline void clear_sfile(sfile* file)
//Function:         releases all buffers held by file struct
{
    if (file->infilename.str != 0) del_sstr(&file->infilename);
    if (file->outfilename.str != 0) del_sstr(&file->outfilename);
    if (file->message.str != 0)  del_sstr(&file->message);
    if (file->outmessage.str != 0)  del_sstr(&file->outmessage);
    if (file->infile != 0) fclose(file->infile);
    if (file->outfile != 0) fclose(file->outfile);
    *file = def_sfile();
}



int load_sfile(sfile* file, const char* infilename, const char* inmode,
                            const char* outfilename);
//Function:         Loads contents of input file to memory,
//                  and prepares an output file for writing
//Mode:             if outfile name is an empty string it is left unopenned
//                  use this to open and load encoded message files to memory
//Precondition:     sfile is not initialized, does not hold resources!
//Return value:     -1 on failure to open input file, 
//                  -2 on failure to open output file
//                  -3 if input file is empty
//                   1 on on success,

int print_out_message(sfile* file);
//Function:             Prints value of file->outmessage to file->outfile, 
//                      both stored in file as fields



//USE load_sfile function instead, pass outfile name an empty string ""
//int load_encoded_sfile(sfile* file, const char* encodedfilename, const char* inmode);
//Function:         Loads the contents of a previously encoded message from the specified file
//                  to be processed by later by the correct encoder
//Precondition:     file is default initialized, has no malloced memory!
//                  The encoded message file format is as follows:
//                  F[A][B][C][0][10][11]
//                   00001011001011
//                  The first line has the symbols and their codewords inside square brackets, as ascii
//                  the second line holds the encoded message, as ascii 0 and 1 chars
//Return value:     0 if memory allocation occurs,
//                  -1 if file is no default initialized,
//                  -2 if file opening fails,                 
//                  1 on success


#endif