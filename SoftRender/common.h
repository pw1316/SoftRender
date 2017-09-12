#pragma once
#include <windows.h>
#include <cmath>

__forceinline BOOL isAndSetZero(FLOAT& value) {
    if (abs(value) < 1e-7f)
    {
        value = 0.0f;
        return true;
    }
    return false;
}

__forceinline FLOAT quickInvSqrt(FLOAT number)
{
    LONG i;
    FLOAT x2, y;
    const FLOAT threehalfs = 1.5F;
    x2 = number * 0.5F;
    y = number;
    i = *(LONG *)&y;// evil floating point bit level hacking（对浮点数的邪恶位元hack）
    i = 0x5f3759df - (i >> 1);// what the fuck?（这他妈的是怎么回事？）
    y = *(FLOAT *)&i;
    y = y * (threehalfs - (x2 * y * y));   // 1st iteration （第一次迭代）
    y = y * (threehalfs - (x2 * y * y));   // 2nd iteration, this can be removed（第二次迭代，可以删除）
    return 1.0f / y;
}