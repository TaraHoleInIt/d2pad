#pragma once

#include "common.h"

typedef struct {
    int ordinal;
    HMODULE* module;
    void** proc;
} D2Import;

typedef HWND( __stdcall* GetDiabloWindowProc ) ( void );

extern GetDiabloWindowProc gfxGetGameWindow;

bool diaboInterfaceInit( void );
