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

typedef struct {
    int ordinal;
    HMODULE* module;
    void** proc;
} D2Import;

typedef HWND ( __stdcall* GetDiabloWindowProc ) ( void );

void mainThread( LPVOID param );
BOOL createMainThread( void );
BOOL diaboInterfaceInit( void );

GetDiabloWindowProc gfxGetGameWindow = NULL;

HANDLE mainThreadHandle = NULL;
HWND diabloWindow = NULL;
BOOL shouldRun = FALSE;

HMODULE dllD2Gfx = NULL;
HMODULE dllD2Win = NULL;

// Ordinals for 1.13
D2Import importTable[ ] = {
    {
        10048,
        &dllD2Gfx,
        ( void** ) &gfxGetGameWindow
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

uint64_t getDiabloVersion( void ) {
    VS_FIXEDFILEINFO* fileInfoPtr = NULL;
    static wchar_t modulePath[ 256 ];
    void* versionInfoBuffer = NULL;
    DWORD versionInfoSize = 0;
    DWORD versionLength = 0;
    uint64_t version = 0;

    // Get path of the EXE that loaded us (game.exe)
    GetModuleFileNameW( NULL, modulePath, _countof( modulePath ) );

    if ( ( versionInfoSize = ( *addrGetFileVersionInfoSizeW ) ( modulePath, NULL ) ) > 0 ) {
        if ( ( versionInfoBuffer = malloc( versionInfoSize ) ) != NULL ) {
            if ( ( *addrGetFileVersionInfoW ) ( modulePath, 0, versionInfoSize, versionInfoBuffer ) ) {
                // its this
                ( *addrVerQueryValueW ) ( versionInfoBuffer, StrWide( "\\" ), ( LPVOID* ) &fileInfoPtr, ( PUINT ) & versionLength );
                version = ( ( uint64_t ) fileInfoPtr->dwFileVersionMS << 32 ) | fileInfoPtr->dwFileVersionLS;
            }

            free( versionInfoBuffer );
        }
    }

    return version;
}

void mainThread( LPVOID param ) {
    uint64_t version = 0;

    padInit( );
    setDefaultKeyBinds( );

    printFunction( );

    version = getDiabloVersion( );

    debugMessage( StrWide( "Hi 0x%08llX\n" ), version );

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
