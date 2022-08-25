#ifndef __UTILS_H_
#define __UTILS_H_

// Find the smallest power of two that will contain the input
static int powerOfTwo(int input)
{
    int value = 1;

    while (value < input)
    {
        value <<= 1;
    }
    return value;
}

#endif
