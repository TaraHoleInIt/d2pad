#include <windows.h>
#include <wchar.h>
#include <math.h>
#include "common.h"
#include "util.h"

static HANDLE conStdout = INVALID_HANDLE_VALUE;

// Untested on multiple screens
void utilGetScreenSize( HWND forWindow, POINT* screenSize ) {
    HMONITOR mon = NULL;
    MONITORINFO mi;

    if ( forWindow && screenSize ) {
        if ( ( mon = MonitorFromWindow( forWindow, MONITOR_DEFAULTTOPRIMARY ) ) != NULL ) {
            memset( &mi, 0, sizeof( MONITORINFO ) );
            mi.cbSize = sizeof( MONITORINFO );

            if ( GetMonitorInfoW( mon, &mi ) ) {
                screenSize->x = abs( mi.rcWork.right - mi.rcWork.left );
                screenSize->y = abs( mi.rcWork.bottom - mi.rcWork.top );

                return;
            }
        } 

        screenSize->x = GetSystemMetrics( SM_CXSCREEN );
        screenSize->y = GetSystemMetrics( SM_CYSCREEN );
    }
}

void utilGetWindowMidpoint( HWND hWnd, POINT* midPoint ) {
    WINDOWINFO wi;

    if ( hWnd && midPoint ) {
        memset( &wi, 0, sizeof( WINDOWINFO ) );
        wi.cbSize = sizeof( WINDOWINFO );

        if ( GetWindowInfo( hWnd, &wi ) ) {
            midPoint->x = ( ( wi.rcClient.right - wi.rcClient.left ) / 2 ) + wi.rcClient.left;
            midPoint->y = ( ( wi.rcClient.bottom - wi.rcClient.top ) / 2 ) + wi.rcClient.top;
        }
    }
}

void utilDebugMessage( const wchar_t* fmt, ... ) {
    static wchar_t buffer[ 1024 ];
    int count = 0;
    va_list argp;

    if ( conStdout != INVALID_HANDLE_VALUE ) {
        va_start( argp, fmt );
            memset( buffer, 0, sizeof( buffer ) );
            count = _vsnwprintf_s( buffer, _countof( buffer ), _TRUNCATE, fmt, argp );
        va_end( argp );

        WriteConsoleW( conStdout, buffer, count, NULL, NULL );
    }
}

void utilCreateDebugConsole( void ) {
    if ( AllocConsole( ) ) {
        conStdout = GetStdHandle( STD_OUTPUT_HANDLE );

        if ( conStdout != INVALID_HANDLE_VALUE ) {
            utilDebugMessage( L"Debug console created.\n" );
        }
    }
}

void utilCloseDebugConsole( void ) {
    conStdout = INVALID_HANDLE_VALUE;
    FreeConsole( );
}
