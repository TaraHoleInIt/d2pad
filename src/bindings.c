#include <windows.h>
#include "common.h"
#include "gamepad.h"
#include "util.h"
#include "bindings.h"
#include "input.h"

static void onMetaKeyDown( void );
static void onMetaKeyUp( void );
static void onLeftClickDown( void );
static void onLeftClickUp( void );
static void onLeftStickStartedMoving( void );
static void onLeftStickStoppedMoving( void );
static void onLeftStickMoving( void );
static void onBackButtonDown( void );
static void onStartButtonDown( void );

extern HWND diabloWindow;

static bool isMeta = false;

static void onMetaKeyDown( void ) {
    isMeta = true;
}

static void onMetaKeyUp( void ) {
    isMeta = false;
}

static void onLeftClickDown( void ) {
    if ( isMeta ) {
        inputAddMouseRightButton( true );
    } else {
        inputAddMouseLeftButton( true );
    }
}

static void onLeftClickUp( void ) {
    if ( isMeta ) {
        inputAddMouseRightButton( false );
    } else {
        inputAddMouseLeftButton( false );
    }
}

static void onLeftStickStartedMoving( void ) {
    utilDebugMessage( L"onLeftStickStartedMoving\n" );
    inputAddMouseLeftButton( true );
}

static void onLeftStickStoppedMoving( void ) {
    POINT screenSize;
    POINT midPoint;
    float x = 0.0f;
    float y = 0.0f;

    utilDebugMessage( L"onLeftStickStoppedMoving\n" );
    inputAddMouseLeftButton( false );

    utilGetScreenSize( diabloWindow, &screenSize );
    utilGetWindowMidpoint( diabloWindow, &midPoint );

    x = ( ( float ) midPoint.x ) / ( ( float ) screenSize.x );
    y = ( ( float ) midPoint.y ) / ( ( float ) screenSize.y );

    inputAddMouseMoveAbsolute( ( int ) ( x * 65535.0f ), ( int ) ( y * 65535.0f ) );
}

static void onLeftStickMoving( void ) {
    StickMovement sm;
    POINT screenSize;
    POINT midPoint;
    float x = 0.0f;
    float y = 0.0f;

    utilGetScreenSize( diabloWindow, &screenSize );
    utilGetWindowMidpoint( diabloWindow, &midPoint );
    padGetLeftStick( &sm );

    sm.dx *= 50.0f;
    sm.dy *= 50.0f;

    x = ( ( float ) midPoint.x + sm.dx ) / ( ( float ) screenSize.x );
    y = ( ( float ) midPoint.y + sm.dy - 50 ) / ( ( float ) screenSize.y );

    inputAddMouseMoveAbsolute( ( int ) ( x * 65535.0f ), ( int ) ( y * 65535.0f ) );
}

static void onBackButtonDown( void ) {
    inputAddKeyDownEvent( VK_ESCAPE );
    inputAddKeyUpEvent( VK_ESCAPE );
}

static void onStartButtonDown( void ) {
    int vkey = ( isMeta ) ? 
        'A' :
        'I'
    ;

    inputAddKeyDownEvent( vkey );
    inputAddKeyUpEvent( vkey );
}

#define MakeKeyFunc( xInputButton, vKeyNormal, vKeyMeta, isActionButton, funcName ) \
    void funcName( void ) { \
        int vkey = ( isMeta ) ? vKeyMeta : vKeyNormal; \
        inputAddKeyDownEvent( vkey ); \
        inputAddKeyUpEvent( vkey ); \
        inputAddMouseRightButton( true ); \
        inputAddMouseRightButton( false ); \
    }

MakeKeyFunc( XINPUT_GAMEPAD_B, VK_F1, VK_F5, true, onBButtonDown );
MakeKeyFunc( XINPUT_GAMEPAD_A, VK_F2, VK_F6, true, onAButtonDown );
MakeKeyFunc( XINPUT_GAMEPAD_X, VK_F3, VK_F7, true, onXButtonDown );
MakeKeyFunc( XINPUT_GAMEPAD_Y, VK_F4, VK_F8, true, onYButtonDown );

MakeKeyFunc( XINPUT_GAMEPAD_DPAD_RIGHT, '1', '1', false, onDpadRightDown );
MakeKeyFunc( XINPUT_GAMEPAD_DPAD_DOWN, '2', '2', false, onDpadDownDown );
MakeKeyFunc( XINPUT_GAMEPAD_DPAD_LEFT, '3', '3', false, onDpadLeftDown );
MakeKeyFunc( XINPUT_GAMEPAD_DPAD_UP, '3', '3', false, onDpadUpDown );

MakeKeyFunc( XINPUT_GAMEPAD_LEFT_THUMB, 'R', 'R', false, onLStickDown );
MakeKeyFunc( XINPUT_GAMEPAD_RIGHT_THUMB, VK_TAB, VK_TAB, false, onRStickDown );

void setDefaultKeyBinds( void ) {
    padAddBinding( Button_LStickMoving, onLeftStickStartedMoving, onLeftStickMoving, onLeftStickStoppedMoving );
    padAddBinding( Config_Button_Click, onLeftClickDown, NULL, onLeftClickUp );
    padAddBinding( XINPUT_GAMEPAD_BACK, onBackButtonDown, NULL, NULL );
    padAddBinding( XINPUT_GAMEPAD_START, onStartButtonDown, NULL, NULL );

    padAddBinding( XINPUT_GAMEPAD_A, onAButtonDown, NULL, NULL );
    padAddBinding( XINPUT_GAMEPAD_B, onBButtonDown, NULL, NULL );
    padAddBinding( XINPUT_GAMEPAD_X, onXButtonDown, NULL, NULL );
    padAddBinding( XINPUT_GAMEPAD_Y, onYButtonDown, NULL, NULL );

    padAddBinding( XINPUT_GAMEPAD_DPAD_RIGHT, onDpadRightDown, NULL, NULL );
    padAddBinding( XINPUT_GAMEPAD_DPAD_DOWN, onDpadDownDown, NULL, NULL );
    padAddBinding( XINPUT_GAMEPAD_DPAD_LEFT, onDpadLeftDown, NULL, NULL );
    padAddBinding( XINPUT_GAMEPAD_DPAD_UP, onDpadUpDown, NULL, NULL );

    padAddBinding( XINPUT_GAMEPAD_LEFT_THUMB, onLStickDown, NULL, NULL );
    padAddBinding( XINPUT_GAMEPAD_RIGHT_THUMB, onRStickDown, NULL, NULL );
    
    padAddBinding( Button_Meta, onMetaKeyDown, NULL, onMetaKeyUp );
}
