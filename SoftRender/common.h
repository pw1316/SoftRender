#pragma once
#include <windows.h>
#include <cmath>

/* Define assertion */
#ifndef Assert
#if defined( DEBUG ) || defined( _DEBUG )
#define Assert(b) do {if (!(b)) {OutputDebugString(TEXT("Assert: " #b "\n"));}} while(0)
#else
#define Assert(b)
#endif //DEBUG || _DEBUG
#endif

__forceinline BOOL isAndSetZero(FLOAT & value) {
    if (abs(value) < 1e-7f)
    {
        value = 0.0f;
        return true;
    }
    return false;
}

__forceinline FLOAT quickSqrt(FLOAT number)
{
    LONG i;
    FLOAT x2, y;
    const FLOAT threehalfs = 1.5F;
    x2 = number * 0.5F;
    y = number;
    i = *(LONG *)&y;// evil floating point bit level hacking���Ը�������а��λԪhack��
    i = 0x5f3759df - (i >> 1);// what the fuck?�������������ô���£���
    y = *(FLOAT *)&i;
    y = y * (threehalfs - (x2 * y * y));   // 1st iteration ����һ�ε�����
    y = y * (threehalfs - (x2 * y * y));   // 2nd iteration, this can be removed���ڶ��ε���������ɾ����
    return 1.0f / y;
}