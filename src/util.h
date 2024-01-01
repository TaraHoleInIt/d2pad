#pragma once

#include "common.h"

void utilGetScreenSize( HWND forWindow, POINT* screenSize );
void utilGetWindowMidpoint( HWND hWnd, POINT* midPoint );
void utilDebugMessage( const wchar_t* fmt, ... );
void utilCreateDebugConsole( void );
void utilCloseDebugConsole( void );
