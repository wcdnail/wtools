#ifndef _nd_digimagix_simple_h__
#define _nd_digimagix_simple_h__

#ifdef _MSC_VER
#pragma warning(disable: 4146)  // C4146: unary minus operator applied 
                                // to unsigned type, result still unsigned
#endif

template <typename T> 
inline T DmMax(T x, T y) 
{
    return x > y ? x : y; 
}

template <typename T> 
inline T DmMin(T x, T y) 
{
    return x < y ? x : y; 
}

template <typename T> 
inline T DmNegate(T x) 
{
    return (T)-x; 
}

template <typename T> 
inline T DmAbs(T x) 
{
    return x < 0 ? (T)-x : x; 
}

template <typename T> 
inline T const& DmDivider(T const& d) 
{ 
    static T sd = 1;
    return !d ? sd : d; 
}

template <typename T> 
inline T DmRemainder(T const& x, T const& d) 
{
    return x % DmDivider(d); 
}

template <typename T> 
inline T DmDiv(T const& x, T const& d)
{
    return x / DmDivider(d); 
}

template <typename T> 
inline int DmDigitCount(T x, int radix, bool isNegative) 
{
    int count = (isNegative ? 2 : 1);

    while (x /= DmDivider(radix)) 
        ++count;

    return count;
}

/**
 * Получает степень и кол-во десятичных знаков вещественного числа.
 */
template <typename T> 
inline int DmPowerOf(T value, T& counter) 
{
    int power = 0;
    counter = 1.;

    if (value < 1.)
    {
        do 
        {
            counter /= 10.; 
            --power;
        } 
        while(value < counter);
    }
    else
    {
        while(value > counter * 10)
        { 
            counter *= 10.; 
            ++power; 
        }
    }

    return power;
}

#endif /* _nd_digimagix_simple_h__ */
