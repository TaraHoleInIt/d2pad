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
#include "versionproxy.h"
#include "diablo.h"

void mainThread( LPVOID param );
BOOL createMainThread( void );

HANDLE mainThreadHandle = NULL;
HWND diabloWindow = NULL;
BOOL shouldRun = FALSE;

void mainThread( LPVOID param ) {
    padInit( );
    setDefaultKeyBinds( );

    printFunction( );

    while ( shouldRun == TRUE ) {
        diabloWindow = ( *gfxGetGameWindow ) ( );

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
            utilCreateDebugConsole( );
            versionProxyInit( );

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
