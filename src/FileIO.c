#include "FileIO.h"
/*
*   1. FileIO.h FileIO.c->module resposible for reading in files
*   File Formats are: Simple text files, used as message source
*                     Encoded message files, contain the alphabet and codewords at the start
*   Also handles file output,
*                     
* 
*/
int resize_sstr(sstr* ss, size_t newsize)
//Function:     resizes internal buffer of ss to previous size + newsize
//              only meant to be used for increasing the size of ss
//Return value:  0  on memory allocation failure
//               1 on success
{
    ss->smax += newsize; //if ss is default initialized, ss->smax is 0,
    char* tmp = (char*)malloc(sizeof(char) * (ss->smax));
    if (tmp == 0) return 0;
    for (size_t i = 0; i < ss->len; ++i) //if ss is empty, then ss->len is 0 so this loop is skipped
    {
        tmp[i] = ss->str[i];
    }
    for (size_t i = ss->len; i < ss->smax; ++i)
    {
        tmp[i] = 0;
    }
    if (ss->str != 0)free(ss->str); //only try to free if ss is not empty
    ss->str = tmp;
    return 1;
}

int app_sstr(sstr* ss, const char* val)
//Function:     appends string to simplestring     
{
    //check ss buffer size if it cannot also fit val, 
    //reallocate memory
    size_t len = ss->len;
    size_t newmax = 0;
    for (size_t i = 0; i < val[i] != 0; ++i)
    {
        ++newmax;
    }
    if (len + newmax > ss->smax)
    {
        resize_sstr(ss, newmax);
    }
    size_t i = ss->len - 1;   //length is 1 past the 0 terminator, so write in the previous location
    --ss->len;
    size_t j = 0;
    do
    {
        ss->str[i] = val[j];
        ++ss->len;
        ++i;
        ++j;
    } while (val[j] != 0 && i < ss->smax);
    ss->str[ss->len] = 0;
    return 1;
}

void read_sstr(sstr* ss, const char* str, size_t lim)
//Function:       copies string str into ss
{
    del_sstr(ss);
    ss->smax = 1;
    for (size_t i = 0; str[i] != 0; ++i)
    {
        ++ss->smax;
        if (i > lim)break;
    }

    ss->smax *= 2;
    init_sstr(ss, ss->smax);
    ss->len += 1;
    for (size_t i = 0; str[i] != 0; ++i)
    {
        ss->str[i] = str[i];
        ++ss->len;
        if (i >= ss->smax) break;
    }
    ss->str[ss->len] = '\0';
}

int load_sfile(sfile* file, const char* infilename, const char* inmode,
    const char* outfilename)
//Function:         Loads contents of input file to memory,
//                  and prepares an output file for writing
//Mode:             if outfile name is an empty string it is left unopenned
//                  use this to open and load encoded message files to memory
//Precondition:     sfile is not initialized, does not hold resources!
//Return value:     -1 on failure to open input file, 
//                  -2 on failure to open output file
//                  -3 if input file is empty
//                  1 on on success,
{
    if (file->infile != 0 || file->outfile != 0) return -1;  //check for default initialization
    read_sstr(&file->infilename, infilename, 128);
    read_sstr(&file->outfilename, outfilename, 128);
    fopen_s(&file->infile, infilename, inmode);         //try to open input file
    if (file->infile == 0)
    {                                                   //if it fails, return -1
        del_sstr(&file->infilename);
        del_sstr(&file->outfilename);
        return -1;
    }
    if(outfilename[0] != '\0')  //only open outfile if given a name for it
    {
        fopen_s(&file->outfile, outfilename, "w");
        if (file->outfile == 0)       //try to open output file for writing
        {                                                   //if it fails, return -2
            del_sstr(&file->infilename);
            del_sstr(&file->outfilename);
            return -1;
        }
    }
    //read the contents of the input file into message buffer
    //count the number of characters in msg file first for buffer size estimation
    //then to copy contents to memory, if input file is empty close and return -3
    char c;
    size_t msize = 0;
    do
    {
        c = getc(file->infile);
        ++msize;
    } while (c != EOF);
   

    if (msize < 1)
    {
        clear_sfile(file);
        return -3;
    }
    //copy contents of message to memory
   
    resize_sstr(&file->message, msize);   //resize the buffer to that of the input
    rewind(file->infile);
    for (size_t i = 0; i < msize; ++i)
    {

        app_sstrc(&file->message, getc(file->infile));
    }
    return 1;
}

int print_out_message(sfile* file)
//Function:         Prints value of file->outmessage to file->outfile, 
//                  both stored in file as fields
{
    if(file->outfile == 0) return -1;
    for(size_t i = 0; i < file->outmessage.len-1; ++i)
    {
        putc(file->outmessage.str[i], file->outfile);
    }
    return 1;
}


