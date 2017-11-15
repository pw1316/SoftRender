#pragma once
#include "stdafx.h"

inline PWfloat quickInvSqrt(PWfloat number)
{
    PWlong i;
    PWfloat x2, y;
    const PWfloat threehalfs = 1.5f;
    x2 = number * 0.5f;
    y = number;
    i = *(PWlong *)&y;// evil floating point bit level hacking
    i = 0x5f3759df - (i >> 1);// what the fuck?
    y = *(PWlong *)&i;
    y = y * (threehalfs - (x2 * y * y));   // 1st iteration
    y = y * (threehalfs - (x2 * y * y));   // 2nd iteration, this can be removed
    return 1.0f / y;
}