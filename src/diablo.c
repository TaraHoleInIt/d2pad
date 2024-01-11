#include <Windows.h>
#include "common.h"
#include "util.h"
#include "diablo.h"
#include "versionproxy.h"

static uint64_t getDiabloVersion( void );
bool diaboInterfaceInit( void );

GetDiabloWindowProc gfxGetGameWindow = NULL;

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

bool diaboInterfaceInit( void ) {
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

static uint64_t getDiabloVersion( void ) {
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
                ( *addrVerQueryValueW ) ( versionInfoBuffer, StrWide( "\\" ), ( LPVOID* ) &fileInfoPtr, ( PUINT ) &versionLength );
                version = ( ( uint64_t ) fileInfoPtr->dwFileVersionMS << 32 ) | fileInfoPtr->dwFileVersionLS;
            }

            free( versionInfoBuffer );
        }
    }

    return version;
}
