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

inline BOOL isAndSetZero(FLOAT & value) {
    if (abs(value) < 1e-7f)
    {
        value = 0.0f;
        return true;
    }
    return false;
}