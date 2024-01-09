#pragma once

#include "common.h"

#ifdef _DEBUG
#define debugMessage( ... ) utilDebugMessage( __VA_ARGS__ )
#else
#define debugMessage( ... )
#endif

#define printFunction( ) utilDebugMessage( StrWide( "%s\n" ), __FUNCTION__ )

void utilGetScreenSize( HWND forWindow, POINT* screenSize );
void utilGetWindowMidpoint( HWND hWnd, POINT* midPoint );
void utilDebugMessage( const wchar_t* fmt, ... );
void utilCreateDebugConsole( void );
void utilCloseDebugConsole( void );
