#include <windows.h>
#include "common.h"

//static void setupKeyEvent( INPUT* eventPtr, int keyCode, bool isDown );
static void setupMouseMoveRelative( INPUT* eventPtr, uint32_t dx, uint32_t dy, uint32_t flags );
static void setupMouseMoveAbsolute( INPUT* eventPtr, uint32_t x, uint32_t y, uint32_t flags );

static INPUT eventList[ Config_Max_Input_Events ];
static int eventCount = 0;

//static void setupKeyEvent( INPUT* eventPtr, int keyCode, bool isDown ) {
//    if ( eventPtr ) {
//        memset( eventPtr, 0, sizeof( INPUT ) );
//
//        eventPtr->type = INPUT_KEYBOARD;
//        eventPtr->ki.wScan = keyCode;
//        eventPtr->ki.dwFlags = ( ( isDown == FALSE ) ? KEYEVENTF_KEYUP : 0 ) | KEYEVENTF_SCANCODE;
//    }
//}

static void setupMouseMoveRelative( INPUT* eventPtr, uint32_t dx, uint32_t dy, uint32_t flags ) {
    if ( eventPtr ) {
        memset( eventPtr, 0, sizeof( INPUT ) );

        eventPtr->type = INPUT_MOUSE;
        eventPtr->mi.dx = dx;
        eventPtr->mi.dy = dy;
        eventPtr->mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_MOVE_NOCOALESCE | flags;
    }
}

static void setupMouseMoveAbsolute( INPUT* eventPtr, uint32_t x, uint32_t y, uint32_t flags ) {
    if ( eventPtr ) {
        memset( eventPtr, 0, sizeof( INPUT ) );

        eventPtr->type = INPUT_MOUSE;
        eventPtr->mi.dx = x;
        eventPtr->mi.dy = y;
        eventPtr->mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE_NOCOALESCE | flags;
    }
}

static INPUT* getFreeEvent( void ) {
    INPUT* result = NULL;

    if ( eventCount < Config_Max_Input_Events ) {
        result = &eventList[ eventCount++ ];
        memset( result, 0, sizeof( INPUT ) );
    }

    return result;
}

void inputBegin( void ) {
    memset( eventList, 0, sizeof( eventList ) );
    eventCount = 0;
}

void inputEnd( void ) {
    if ( eventCount ) {
        SendInput( eventCount, eventList, sizeof( INPUT ) );
    }
}

bool inputAddKeyDownEvent( int vkey ) {
    INPUT* evt = getFreeEvent( );

    if ( evt ) {
        evt->type = INPUT_KEYBOARD;
        evt->ki.wScan = MapVirtualKeyExW( vkey, MAPVK_VK_TO_VSC, 0 );
        evt->ki.dwFlags = KEYEVENTF_SCANCODE;
    }

    return ( evt ) ? true : false;
}

bool inputAddKeyUpEvent( int vkey ) {
    INPUT* evt = getFreeEvent( );

    if ( evt ) {
        evt->type = INPUT_KEYBOARD;
        evt->ki.wScan = MapVirtualKeyExW( vkey, MAPVK_VK_TO_VSC, 0 );
        evt->ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
    }

    return ( evt ) ? true : false;
}

bool inputAddMouseMoveRelative( int dx, int dy ) {
    INPUT* evt = getFreeEvent( );

    if ( evt ) {
        setupMouseMoveRelative( evt, dx, dy, 0 );
    }

    return ( evt ) ? true : false;
}

bool inputAddMouseMoveAbsolute( int x, int y ) {
    INPUT* evt = getFreeEvent( );

    if ( evt ) {
        setupMouseMoveAbsolute( evt,x, y, 0 );
    }

    return ( evt ) ? true : false;
}

bool inputAddMouseLeftButton( bool isDown ) {
    INPUT* evt = getFreeEvent( );

    if ( evt ) {
        setupMouseMoveRelative( evt, 0, 0, ( isDown ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_LEFTUP ) );
    }

    return ( evt ) ? true : false;
}

bool inputAddMouseRightButton( bool isDown ) {
    INPUT* evt = getFreeEvent( );

    if ( evt ) {
        setupMouseMoveRelative( evt, 0, 0, ( isDown ? MOUSEEVENTF_RIGHTDOWN : MOUSEEVENTF_RIGHTUP ) );
    }

    return ( evt ) ? true : false;
}
