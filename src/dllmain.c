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
#include "bindings.h"
#include "util.h"

typedef struct {
    int ordinal;
    HMODULE* module;
    void** proc;
} D2Import;

typedef int ( __stdcall* ShowCursor1Proc ) ( int show );
typedef void ( __stdcall* ShowCursor2Proc ) ( int show );
typedef HWND ( __stdcall* GetDiabloWindowProc ) ( void );

void mainThread( LPVOID param );
BOOL createMainThread( void );
void showCursor( void );
void hideCursor( void );
BOOL diaboInterfaceInit( void );

GetDiabloWindowProc GetDiabloWindow = NULL;
ShowCursor1Proc ShowCursor1 = NULL;
ShowCursor2Proc ShowCursor2 = NULL;

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
        10011,
        &dllD2Win,
        ( void** ) &ShowCursor1
    },
    {
        10029,
        &dllD2Win,
        ( void** ) &ShowCursor2
    },
    {
        10027,
        &dllD2Gfx,
        ( void** ) &GetDiabloWindow
    }
};

#if 0
void doTest( void ) {
    INPUT mouseEvents[ 2 ];

    sendKey( MapVirtualKeyEx( VK_F6, MAPVK_VK_TO_VSC, 0 ), TRUE );
    sendKey( MapVirtualKeyEx( VK_F6, MAPVK_VK_TO_VSC, 0 ), FALSE );

    setupMouseMoveRelative( 0, 0, MOUSEEVENTF_RIGHTDOWN, &mouseEvents[ 0 ] );
    setupMouseMoveRelative( 0, 0, MOUSEEVENTF_RIGHTUP, &mouseEvents[ 1 ] );
    SendInput( 2, mouseEvents, sizeof( INPUT ) );
}
#endif

void showCursor( void ) {
    //*shouldDrawCursor = 1;
}

void hideCursor( void ) {
    //*shouldDrawCursor = 0;
}

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
    StickMovement ls;
    StickMovement rs;
    POINT screenSize;
    POINT midPoint;
    float x = 0.0f;
    float y = 0.0f;

    padInit( );
    setDefaultKeyBinds( );

    while ( shouldRun == TRUE ) {
        diabloWindow = ( *GetDiabloWindow ) ( );

        if ( diabloWindow /*&& diabloWindow == GetFocus( )*/ ) {
            utilGetWindowMidpoint( diabloWindow, &midPoint );
            utilGetScreenSize( diabloWindow, &screenSize );

            inputBegin( );
                padUpdate( );
                padGetLeftStick( &ls );
                padGetRightStick( &rs );

                if ( padIsStickMoving( &rs ) ) {
                    inputAddMouseMoveRelative( 
                        ( int ) ( rs.dx * Config_Cursor_Movement_Scale ), 
                        ( int ) ( rs.dy * Config_Cursor_Movement_Scale )
                    );
                }
            inputEnd( );
        }

        Sleep( 10 );
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

BOOL WINAPI DllMain( HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved ) {
    switch ( dwReason ) {
        case DLL_PROCESS_ATTACH: {
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
