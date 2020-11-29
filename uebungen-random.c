#include "TypeUtil.h"

#define WINDOWS_P
#include "PerfCounter.h"

#define arSize 1024LL*1024LL*1024LL*3LL
char* val;
int* v2;
static inline void init()
{

    val = (char*)malloc(arSize);
    v2 = (int*)malloc(sizeof(int) * arSize);
    for (u64 i = 0; i < arSize; ++i)
    {
        val[i] = '9';
        v2[i] = 0;
    }

}

static inline void convSub()
{
    for (u64 i = 0; i < arSize; ++i)
    {
        v2[i] = val[i] - '0';
    }

}

static inline void convAnd()
{
    unsigned int mask = 0x0f;
    for (u64 i = 0; i < arSize; ++i)
    {
        v2[i] = val[i] & mask;
    }
}

static inline void convXOR()
{
    unsigned int mask = 0x30;
    for (u64 i = 0; i < arSize; ++i)
    {
        v2[i] = val[i] ^ mask;
    }
}

void convUebung()
{
    init();
    PerfCounter pf;
    startTimer(&pf);
    convSub();
    stopTimerPrint(&pf);
    startTimer(&pf);
    convAnd();
    stopTimerPrint(&pf);
    startTimer(&pf);
    convXOR();
    stopTimerPrint(&pf);
    getchar();
    free(v2);
    free(val);


}
