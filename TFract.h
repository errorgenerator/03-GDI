#ifndef T_FRACT_H
#define T_FRACT_H

#include "TypeUtil.h"

typedef enum tfractsign
{
    n = -1,
    p = 1

}tfractsign;

typedef struct TFract
{
    u64 num;
    u64 denom;
    //byte sign;
    tfractsign sign;
}TFract;

static inline u64 tf_gcd(u64 a, u64 b)
{
    if( a == 0) return 1;
    if (b > a)
    {
        u64 c = b;
        b = a;
        a = c;

    }
    u64 r = 0;
    while ((a % b) != 0)
    {
        r = a % b;
        a = b;
        b = r;
    }
    if( r == 0) return b;
    return r;
}

static inline u64 tf_lcm(u64 a, u64 b)
{
    return (a * b) / tf_gcd(a, b);
}

static inline TFract tf_reduce(TFract f)
{
    u64 gcd = tf_gcd(f.num,f.denom);
    f.num = f.num / gcd;
    f.denom = f.denom / gcd;
    return f;
}

static inline TFract tf_add(TFract a, TFract b)
{
    TFract result = {.num=0,.denom=1,.sign=p};
    if(a.sign == b.sign)
    {
        result.sign = p;
        result.num = (a.num * b.denom) + (b.num*a.denom);
        result.denom = a.denom * b.denom;
        return tf_reduce(result);
    }else
    {
        result = a;
        a.num *= b.denom;
        b.num *= a.denom;
        if(a.num > b.num)
        {
            result.sign = a.sign;
            result.num = a.num - b.num;
            result.denom = a.denom * b.denom;
            return tf_reduce(result);
        }
        else
        {
            result.sign = b.sign;
            result.num = b.num - a.num;
            result.denom = a.denom * b.denom;
            if(result.num == 0) result.sign = p;
            return tf_reduce(result);
        
        }
    }
    return result;
}

static inline TFract tf_sub(TFract a, TFract b)
{
    if(b.sign == n) b.sign = p;
    else b.sign = n;
    return tf_add(a,b);
}


static inline TFract tf_mul(TFract a, TFract b)
{
    TFract result;
    result.num = a.num * b.num;
    result.denom = a.denom * b.denom;
    if(a.sign == p && b.sign == p) result.sign = p;
    else if(a.sign == n && b.sign == n) result.sign = p;
    else if(result.num == 0) result.sign = p;
    else result.sign = n;
    return tf_reduce(result);
}

static inline TFract tf_div(TFract a, TFract b)
{
    TFract result;
    result.num = a.num * b.denom;
    result.denom = a.denom * b.num;
    if (a.sign == p && b.sign == p) result.sign = p;
    else if (a.sign == n && b.sign == n) result.sign = n;
    else result.sign = n;
    return tf_reduce(result);
}

static inline int tf_inBounds(TFract a, TFract low, TFract up)
//Function to check wether fraction a is within bounds of low and up,
//inclusive: a in [low;up]
{
    a.num   *= low.denom * up.denom;
    low.num *= a.denom * up.denom;
    up.num  *= a.denom * low.denom;
    int ret = 0;
    //Handle range testing where parts all have the same sign
    //Note: User is resposible to pass the bounds in the correct order
    //      from low to high
    if(a.sign == low.sign && a.sign == up.sign)
    {
        if(a.num >= low.num && a.num <= up.num) ret = 1;
        else return 0;
    }
    else if(a.sign == n && low.sign == p)
    {
        return 0;
    }
    else if(a.sign == p && up.sign == n)
    {
        return 0;
    }
    else if(low.sign == n && a.sign == n && up.sign == p)
    {
        //Up sign must be positive, otherwise first testcase would have triggered
        if(a.denom <= low.denom) return 1;
        else if(a.denom > low.denom) return 0;
    }
    else if(low.sign == n && a.sign == p && up.sign == p)
    {
        if(a.denom <= up.denom) return 1;
        else return 0;
    }

    /// ToDo: handle sign cases correctly 
    return ret;

}


#endif
