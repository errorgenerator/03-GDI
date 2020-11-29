#include "FanoCode.h"
#include <stdio.h>
#include <memory.h>
#include <math.h>
/*      GDI Chapter 3: Encoding exercise
*       27 - 11 - 2020
*       ...where are you dum dum.....
*/

/*
*      3. FanoCode.h FanoCode.c -> takes symbol alphabet table to
*       create a code table, ecodes and decodes messages according to the fano algorithm
*/

void print_fano(fanocode* fc)
//Function to print the information stored in
//the fano struct, list of symbols and their codewords.
{
    printf("\n------------FANO INFO----------\n");
    printf("Number of symblos:\t%I64u\n", fc->size);
    printf("Max codeword size:\t%I64u\n", fc->csize);
    printf("Symbol\t\t\tp(x)\t\t\tsum of p(x)\t\t\tcodeword\n");
    for (int i = 0; i < fc->size; ++i)
    {
        printf("%c\t\t\t%.4lf\t\t\t%.4lf\t\t\t\t%s\n", fc->x[i], fc->px[i], fc->sp[i], fc->code[i]);
    }
    printf("Average word length\t\t\tL:%.4lf\n", fc->L);
    printf("Average Entropy\t\t\t\tH:%.4lf\n", fc->H);
    printf("Code Redundancy\t\t\t\tRc:%.4lf\n", fc->Rc);
}
static void recurse_fano(fanocode* fc, size_t start, size_t end)
//Function to create the codewords for each symbol in the alphabet,
//Each call represents one depth of a binary tree, each codeword ends up as a leaf node
//on the code tree, if this codition is violated then decoding the message will not be possible.
//Precondition:     symbol table are sorted from highest to lowest occurence probability
//                  table of sums of probabilities is filled out correctly
{
    //Exit condition 1: If the start index is equal to the end index, nothing needs to be done
    //Exit condition 2: If the start and end index are 1 apart, append a 1 to the codeword of start
    //                  and append a 0 to the codeword of end
    if (start == end)
    {
        return;
    }
    else if ((start - end) == 1)
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
    for (size_t i = start; i <= end; ++i)
    {
        if (fabs((fc->sp[i] - splitval)) <= fabs(delta))
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
    for (size_t i = start; i < midindex; ++i)
    {
        fc->idx[i] += 1;    //increment size of the string holding the code
        fc->code[i][fc->idx[i] - 1] = '1';
    }
    //Apped 1 to everything to the right of midindex
    for (size_t i = midindex; i <= end; ++i)
    {
        fc->idx[i] += 1;    //increment size of the string holding the code
        fc->code[i][fc->idx[i] - 1] = '0';
    }

    //Recuresively process left side of tree, and right side of tree
    recurse_fano(fc, start, midindex - 1);
    recurse_fano(fc, midindex, end);

}

static void create_fano(fanocode* fc)
{
    //1st: Sort fc symbols in decreasing order of probability
    char c = 0;
    double p = 0.0;
    for (size_t i = 0; i < fc->size; ++i)
    {
        for (size_t j = i; j < fc->size; ++j)
        {
            if (fc->px[j] >= fc->px[i])
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
    } while (i != 0);

    //3rd: Recursive algorithm to create the fano codes for each symbol
    recurse_fano(fc, 0, fc->size - 1);

}

int create_fanocode(const symbolal* ca, fanocode* fc)
//Function to fill out fano code struct, and to create fano codes for each char
//precondition: fc is not defined/initialized
{
    fc->size = ca->size;
    fc->csize = FC_CSIZE;
    fc->L = 0;
    fc->H = ca->H;
    fc->Rc = 0;
    fc->x = (char*)malloc(sizeof(char) * fc->size);
    fc->px = (double*)malloc(sizeof(double) * fc->size);
    fc->sp = (double*)malloc(sizeof(double) * fc->size);
    fc->idx = (size_t*)malloc(sizeof(size_t) * fc->size);
    if (fc->x == 0 || fc->px == 0 || fc->sp == 0 || fc->idx == 0)
    {
        clear_fano(fc);
        printf("\ncreateFanoCode(): memory allocation error byebye!\n");
        return -1;
    }


    fc->code = (char**)malloc(sizeof(char*) * fc->size);
    for (size_t i = 0; i < fc->size; ++i)
    {
        fc->code[i] = (char*)malloc(fc->csize);
        if (fc->code[i] == 0)
        {
            clear_fano(fc);
            printf("\ncreateFanoCode(): memory allocation error byebye!\n");
            return -1;
        }
    }
    for (size_t i = 0; i < fc->size; ++i)
    {
        for (size_t j = 0; j < fc->csize; ++j)
        {
            fc->code[i][j] = '\0';
        }
        fc->x[i] = ca->alphabet[i];
        fc->px[i] = ca->px[i];
        fc->idx[i] = 0;
        fc->sp[i] = 0.0;
    }
    create_fano(fc);

    //Compute average word length
    for (size_t i = 0; i < fc->size; ++i)
    {
        fc->L = fc->L + (fc->px[i] * fc->idx[i]);
    }
    //Compute code redundancy
    fc->Rc = fc->L - fc->H;
    return 1;
}

int encode_msg_fano(sfile* filehandler, fanocode* fc)
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
{
    size_t codelentotal = 0;
    //cound the total word lengths
    for(size_t i = 0; i < fc->size; ++i)
    {    
        codelentotal += fc->idx[i]-1;
    }
    size_t outmsgsize = 3*fc->size + 3*codelentotal + codelentotal*filehandler->message.len-1 + 8;
    printf("outmsgsize: %I64u\n",outmsgsize);
    resize_sstr(&filehandler->outmessage,outmsgsize);
    //Place a marker to know which encoder was used
    app_sstrc(&filehandler->outmessage, 'F');   //F for fano
    //Write out symbol table
    for(size_t i = 0; i < fc->size; ++i)
    {
        app_sstrc(&filehandler->outmessage,'[');
        app_sstrc(&filehandler->outmessage, fc->x[i]);
        app_sstrc(&filehandler->outmessage, ']');
    }
    for (size_t i = 0; i < fc->size; ++i)
    {
        app_sstrc(&filehandler->outmessage, '[');
        for(size_t j = 0; j < fc->idx[i]; ++j)
        {
            app_sstrc(&filehandler->outmessage, fc->code[i][j]);
        }
        app_sstrc(&filehandler->outmessage, ']');
    }
    app_sstrc(&filehandler->outmessage, '\n');
    //Now replace each symbol with its code and append it to outmsg
    size_t sid = 0; //symbol index
    for(size_t i = 0; i <  filehandler->message.len-1; ++i)
    {
        if(filehandler->message.str[i] == 0)return 0;
        //search the curent symbol in the symbol table to store its index
        for(size_t j = 0; j < fc->size; ++j)
        {
            
            if(filehandler->message.str[i] == fc->x[j])
            {
                sid = j;            
            }

        }
        //now that the symbol id is found, append the codeword at that id to outmessage
        for(size_t j = 0; j < fc->idx[sid]; ++j)
        {
            app_sstrc(&filehandler->outmessage, fc->code[sid][j]);
        }
        sid = 0;
    
    }
    return 1;
}

static int create_code_tree(fanocode* fc, codetree* ct)
//Function:             Helper function for decoding a message,
//                      creates a code tree that is used to lookup symbols read from 
//                      an encoded message from fc symbol table and codewords
//                      the codewords are implicit, aka, based on the the tree traversal
//                      chooses either the left or right branch, the symbol is then placed
//                      in the leaf node!
{
    ctreenode* pos = ct->root;
    pos  = codetree_insert_next(ct,pos,0,0); // initialize sequential insertion
    if (pos == 0) return -1;
    for(size_t i = 0; i < fc->size; ++i)
    {
        for(size_t j = 0; j < fc->idx[i]; ++j)
        {
            if(fc->code[i][j] == '0')   //branch left
            {
                if((j+1) == fc->idx[i]) //if we reached the last code character, store the symbol value in the tree
                {
                    pos = codetree_insert_next(ct, pos, 0, fc->x[i]); 
                    if (pos == 0) return -1;
                }
                else
                { 
                    pos = codetree_insert_next(ct, pos, 0, 0);
                    if (pos == 0) return -1;
                }
            }
            else if(fc->code[i][j] == '1')   //branch right
            {
                if ((j + 1) == fc->idx[i]) //if we reached the last code character, store the symbol value in the tree
                {
                    pos = codetree_insert_next(ct, pos, 1, fc->x[i]);
                    if(pos == 0) return -1;
                }
                else
                { 
                    pos = codetree_insert_next(ct, pos, 1, 0);
                    if (pos == 0) return -1;
                }
            }
        }
        //now start back at the root, process the next symbol
        pos = ct->root;
    }
    return 1;
}


int decode_msg_fano(sfile* file, fanocode* fc, codetree* ct)
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
{
    printf("\n\ndecode_msg_fano():\n");
    if(fc->size != 0 || fc->code != 0 || ct->root != 0 ) return -1;
    if(file->message.len == 0 || file->message.str == 0) return -2;
    if(file->message.str[0] != 'F') return -3;

    //count the number of symbols in the alphabet
    int symbolcount = 0;
    for(size_t i = 1; file->message.str[i] != '\n'; ++i)
    {
        if(file->message.str[i] != '[' && file->message.str[i] != ']' 
                && file->message.str[i] != '0' && file->message.str[i] != '1')
        {
            ++symbolcount;        
        }
    }
    //allocate storage for fc, read the symbols in from the file
    //and then the codewords
    fc->size = symbolcount;
    fc->csize = FC_CSIZE;   ///field fc->csize is redundant
    fc->x = (char*)malloc(sizeof(char)* symbolcount);
    fc->code = (char**)malloc(sizeof(char*)*symbolcount);
    fc->idx = (size_t*)malloc(sizeof(size_t)*symbolcount);
    if(fc->x == 0 || fc->code == 0 || fc->idx == 0)
    {
        clear_fano(fc);
        return -5;
    }
    //note, the max length of a codeword in number of bits is set in fanocode.h FC_CSIZE define
    for(size_t i = 0; i < symbolcount; ++i)
    {
        fc->code[i] = (char*)malloc(sizeof(char) * FC_CSIZE);
        //if any of the subarrays failed allocation, release previously allocated arrays
        //then all other memory and return -5
        if(fc->code[i] == 0)
        {
            clear_fano(fc);
            return -5;
        }
        for(size_t j = 0; j < FC_CSIZE; ++j)
        {
            fc->code[i][j] = 0;
        }
    }
    for(size_t i = 0; i < fc->size; ++i)
    {
        fc->x[i] = 0;
        fc->idx[i] = 0;
    }
    //store the symbols in fc
    size_t j = 0;
    size_t cstart = 0; //index at which the codewords start
    for (size_t i = 1; file->message.str[i] != '\n'; ++i)
    {
        if(file->message.str[i] == '0' || file->message.str[i] == '1')
        { 
            cstart = i;
            break;
        }
        else if (file->message.str[i] != '[' && file->message.str[i] != ']')
        {
            fc->x[j++] = file->message.str[i];
        }
    }
    //read the codewords into fc->code at fc->idx
    j = 0;
    for(size_t i = cstart; file->message.str[i] != '\n'; ++i)
    {
        if(file->message.str[i] != '[' && file->message.str[i] != ']')
        {
            for(size_t l = i; file->message.str[l] != ']'; ++l)
            {
                if(fc->idx[j] >= FC_CSIZE) //if a codeword is longer than this, we cannot store it, bail!
                {
                    clear_fano(fc);
                    return -6;
                }
                fc->code[j][fc->idx[j]] = file->message.str[l];
                ++fc->idx[j];
                i = l;  //skip over the code which we read in above, i indexes the ] char in message
            }
            ++j;  //advance to the next symbol of fc->x
            ++i; //skip over the ']' of message.str[i]
        }
    }

    //Create the code tree from the symbol table that we read from the file!
    create_code_tree(fc, ct);


    printf("\nSymbol Table read from file: \n");
    for(int i = 0; i < fc->size; ++i)
    {
        printf("x: %c\t\t",fc->x[i]);
        printf("%s",fc->code[i]);
        printf("\n");
    }
    printf("\n");
    
    print_codetree(ct);   

    return 1;
}

