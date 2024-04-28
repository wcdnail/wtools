#pragma once

namespace Generic
{
    template <class T, class S>
    inline void Increment(T& val, S step, T rangeMin, T rangeMax, bool loop = false)
    {
        val += step;

        if (step < 0)
        {
            if (val < rangeMin) 
                val = loop ? rangeMax : rangeMin;
        }

        if (step > 0)
        {
            if (val > rangeMax)
                val = loop ? rangeMin : rangeMax;
        }
    }
}
