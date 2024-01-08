#include <windows.h>
#include <process.h>
#include <wchar.h>
#include <wtypes.h>
#include <stdarg.h>
#include <xinput.h>
#include <math.h>
#include "common.h"
#include "gamepad.h"
#include "input.h"
#include "util.h"

typedef struct {
    int ordinal;
    HMODULE* module;
    void** proc;
} D2Import;

typedef HWND ( __stdcall* GetDiabloWindowProc ) ( void );

void mainThread( LPVOID param );
BOOL createMainThread( void );
BOOL diaboInterfaceInit( void );

GetDiabloWindowProc GetDiabloWindow = NULL;

HANDLE mainThreadHandle = NULL;
HWND diabloWindow = NULL;
BOOL shouldRun = FALSE;

// Offsets for LoD 1.09d
int* cursorX = ( int* ) 0x6F9018A0;
int* cursorY = ( int* ) 0x6F9018A4;
int* lButtonDown = ( int* ) 0x6F9018DC;
int* isInGame = ( int* ) 0x6FA6608C;

int* shouldDrawCursor = ( int* ) 0x6fbcb188;

HMODULE dllD2Gfx = NULL;
HMODULE dllD2Win = NULL;

D2Import importTable[ ] = {
    {
        10027,
        &dllD2Gfx,
        ( void** ) &GetDiabloWindow
    }
};

BOOL diaboInterfaceInit( void ) {
    int i = 0;

    dllD2Gfx = GetModuleHandleW( L"d2gfx.dll" );
    dllD2Win = GetModuleHandleW( L"d2win.dll" );

    for ( ; i < _countof( importTable ); i++ ) {
        if ( importTable[ i ].module && importTable[ i ].proc ) {
            *importTable[ i ].proc = ( void* ) GetProcAddress(
                *importTable[ i ].module,
                ( LPCSTR ) importTable[ i ].ordinal
            );
        }
    }

    return dllD2Gfx && dllD2Win;
}

void mainThread( LPVOID param ) {
    padInit( );
    setDefaultKeyBinds( );

    while ( shouldRun == TRUE ) {
        diabloWindow = FindWindow( NULL, L"Diablo II" );

        if ( diabloWindow /*&& diabloWindow == GetFocus( )*/ ) {
            inputBegin( );
                padUpdate( );
            inputEnd( );
        }

        Sleep( Config_Update_Rate );
    }

    _endthread( );
}

BOOL createMainThread( void ) {
    shouldRun = TRUE;
    mainThreadHandle = ( HANDLE ) _beginthread( mainThread, 0, NULL );

    return ( mainThreadHandle != NULL ) ? TRUE : FALSE;
}

void closeMainThread( void ) {
    shouldRun = FALSE;
    WaitForSingleObject( mainThreadHandle, INFINITE );
}

extern void versionProxyInit( void );

BOOL WINAPI DllMain( HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved ) {
    switch ( dwReason ) {
        case DLL_PROCESS_ATTACH: {
            versionProxyInit( );
            utilCreateDebugConsole( );

            if ( diaboInterfaceInit( ) ) {
                createMainThread( );
            }

            break;
        }
        case DLL_PROCESS_DETACH: {
            closeMainThread( );
            utilCloseDebugConsole( );

            break;
        }
        default: break;
    };

    return TRUE;
}
