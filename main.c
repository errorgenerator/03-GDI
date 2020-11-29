#include <stdio.h>
#include <string.h>
#include <math.h>
#include "TypeUtil.h"
#include "TFract.h"
#define WINDOWS_P
#include "PerfCounter.h"
//#include "src/Encoding.h" //obsolete
#include "src/FileIO.h"
#include "src/SymbolAlphabet.h"
#include "src/FanoCode.h"
#include "src/CodeTree.h"
//Read a file, for a message done
//Create an Alphabet done
//compute probabilty of each character done
//compute Information content of each character I(x) = 1/p(x) done
//compute complete entropy of alphabet  H0 = ld(n) done
//compute average information content/entropy of alphabet H = Sum i=1 to n (p(x) * ld(1/p(x)) done
//compute alphabet/source redundancy Rq  = H0 - H done
///create a code
//compute average code length of alphabet L = Sum i=1 to n (p(xi) * l(xi))
//compute Code redundancy Rc = L - H


/*      Refactoring 26 - 11 - 2020
*       1. FileIO.h FileIO.c    ->   module resposible for reading in message files, 
*                                   encoded message files, decoded message files to memory
* 
*       2. SymbolAlphabet.h SymbolAlphabet.c -> module to process
*                                            new message files, create table of symbols, 
*                                            compute all relevant information
* 
*       3. FanoCode.h FanoCode.c    -> takes takes symbol alphabet table to 
*                                   create a code table, encodes messages according to the fano algorithm
*                                   or takes an ecoded message file and decodes it
* 
*       4. CodeTree.h CodeTree.c -> a binary tree that holds the symbols of a code table in its leaf nodes
*                                   it has functions to sequentially traverse from one level to the next
*                                   based on the juction value 0 or 1, 0 means selecting a left branch,
*                                   1 means selecting a right branch
* 
* 
*/

/// ToDo:   Continue implementing the fano decoder,
///         the codetree looks like it works, test more
///         implement codetree symbol lookup to convert the code bit stream into its symbol

sfile message;


int main(int argc, char **argv)
{
#define TEST
#ifdef TEST
    message = def_sfile();   
    sstr ifn = def_sstr();
    char filename[256];    
    
    //get the name of the input message file
    if(argc > 1)
    {
        read_sstr(&ifn,argv[1],128);
    }
    else
    {
        printf("Give me the message file name:\n");
        if(!scanf_s("%s",filename,256))
        {
            printf("What the heck have you typed!\n");    
        }
        read_sstr(&ifn,filename,256);    
    }
    sstr ofn = def_sstr();
    read_sstr(&ofn,ifn.str,ifn.smax);
    rep_sstr(&ofn,ofn.len-5,ofn.len,'\0');   //delete .txt filename ending
    app_sstr(&ofn,"_Encoded.txt");           //append _Encoded.txt to outputfilename
    int errc = load_sfile(&message,ifn.str,"r",ofn.str);
    if(errc == -1)
    {
        printf("Error openning input file!\n");
    }
    else if(errc == -2)
    {
        printf("Error openning output file!\n");
    }
    else if(errc == -3)
    {
        printf("Input file is empty!\n");
    }
    printf("Input file name: %s\n",ifn.str);
    printf("Output file name: %s\n",ofn.str);
    printf("Message is:\n");
    for(size_t i = 0; i < message.message.len; ++i)
    {
        putchar(message.message.str[i]);
    }
    printf("\n\n");
    errc = 1;
    symbolal symbolalphabet = def_symbol_alphabet();
    //note: since.message.message.len is the string length including the '\0', 
    //it has to be reduced by 1 to pass the number of printable characters
    errc = create_s_a(message.message.str,message.message.len-1,&symbolalphabet);
    print_s_a(&symbolalphabet);


    fanocode fc = def_fanocode();
    create_fanocode(&symbolalphabet,&fc);
    print_fano(&fc);

    encode_msg_fano(&message,&fc);
    printf("\nCoded message:\n%s\n\n", message.outmessage.str);
    print_out_message(&message);
    clear_sfile(&message);


    sfile encodedmsg = def_sfile();
    load_sfile(&encodedmsg,ofn.str,"r","");
    printf("\nEncoded Message read from file:\n%s\n",encodedmsg.message.str);
    fanocode f1 = def_fanocode();
    codetree ct = def_codetree();
    decode_msg_fano(&encodedmsg,&f1,&ct);



    del_sstr(&ifn);
    del_sstr(&ofn);
    clear_sfile(&message);
    clear_s_a(&symbolalphabet);
    clear_fano(&fc);
#endif
#ifdef TEST_CODE_TREE
    //Test the code tree datatype!
    codetree ctree = def_codetree();
    ctreenode* cnode = 0;
    cnode = codetree_insert_next(&ctree,ctree.root,0,0);
    cnode = codetree_insert_next(&ctree, cnode, 0, 0);
    cnode = codetree_insert_next(&ctree, cnode, 0, 0);
    cnode = codetree_insert_next(&ctree, cnode, 0, 'A');
    
    cnode = ctree.root;
    cnode = codetree_insert_next(&ctree, cnode, 0, 0);
    cnode = codetree_insert_next(&ctree, cnode, 0, 0);
    cnode = codetree_insert_next(&ctree, cnode, 1, 'B');

    cnode = ctree.root;
    cnode = codetree_insert_next(&ctree, cnode, 0, 0);    
    cnode = codetree_insert_next(&ctree, cnode, 1, 'C');

    cnode = ctree.root;
    cnode = codetree_insert_next(&ctree, cnode, 1, 0);
    cnode = codetree_insert_next(&ctree, cnode, 1, 0);
    cnode = codetree_insert_next(&ctree, cnode, 0, 'D');

    cnode = ctree.root;
    cnode = codetree_insert_next(&ctree, cnode, 1, 0);
    cnode = codetree_insert_next(&ctree, cnode, 1, 0);
    cnode = codetree_insert_next(&ctree, cnode, 1, 'E');

    printf("Now printing codetree:\n");
    print_codetree(&ctree);
#endif
    return 0;
}