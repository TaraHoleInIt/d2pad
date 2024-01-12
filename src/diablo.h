#pragma once

#include "common.h"

#define Diablo_II_1_13c 0x10000000D003C

typedef struct {
    int ordinal;
    HMODULE* module;
    void** proc;
} D2Import;

typedef HWND( __stdcall* GetDiabloWindowProc ) ( void );

extern GetDiabloWindowProc gfxGetGameWindow;

bool diaboInterfaceInit( void );
