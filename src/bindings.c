#include <windows.h>
#include "common.h"
#include "gamepad.h"
#include "util.h"
#include "input.h"

#define MakeKeyFunc( xInputButton, vKeyNormal, vKeyMeta, isActionButton, funcName ) \
    void funcName( void ) { \
        int vkey = ( isMeta ) ? vKeyMeta : vKeyNormal; \
        debugMessage( L""__FUNCTION__ ); \
        inputAddKeyDownEvent( vkey ); \
        inputAddKeyUpEvent( vkey ); \
        if ( isActionButton ) { \
            inputAddMouseRightButton( true ); \
            inputAddMouseRightButton( false ); \
        } \
    }

static void onMetaKeyDown( void );
static void onMetaKeyUp( void );
static void onLeftClickDown( void );
static void onLeftClickUp( void );
static void onLeftStickStartedMoving( void );
static void onLeftStickStoppedMoving( void );
static void onLeftStickMoving( void );
static void onRightStickStartedMoving( void );
static void onRightStickStoppedMoving( void );
static void onRightStickMoving( void );
static void onR2ButtonDown( void );
static void onR2ButtonUp( void );

extern HWND diabloWindow;

static bool isMeta = false;

static void onMetaKeyDown( void ) {
    debugMessage( L""__FUNCTION__ );
    isMeta = true;
}

static void onMetaKeyUp( void ) {
    debugMessage( L""__FUNCTION__ );
    isMeta = false;
}

static void onLeftClickDown( void ) {
    debugMessage( L""__FUNCTION__ );

    if ( isMeta ) {
        inputAddMouseRightButton( true );
    } else {
        inputAddMouseLeftButton( true );
    }
}

static void onLeftClickUp( void ) {
    debugMessage( L""__FUNCTION__ );

    if ( isMeta ) {
        inputAddMouseRightButton( false );
    } else {
        inputAddMouseLeftButton( false );
    }
}

static void onLeftStickStartedMoving( void ) {
    inputAddMouseLeftButton( true );
}

static void onLeftStickStoppedMoving( void ) {
    POINT screenSize;
    POINT midPoint;
    float x = 0.0f;
    float y = 0.0f;

    debugMessage( L"onLeftStickStoppedMoving\n" );
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

    // HACKHACKHACK
    midPoint.y-= 64;

    sm.dx *= 100.0f;
    sm.dy *= 100.0f;

    x = ( ( float ) midPoint.x + sm.dx ) / ( ( float ) screenSize.x );
    y = ( ( float ) midPoint.y + sm.dy ) / ( ( float ) screenSize.y );

    inputAddMouseMoveAbsolute( ( int ) ( x * 65535.0f ), ( int ) ( y * 65535.0f ) );

    debugMessage( L"Left stick moving; deltas: %.2f, %.2f\n", sm.dx, sm.dy );
}

static void onRightStickStartedMoving( void ) {
}

static void onRightStickStoppedMoving( void ) {
}

static void onRightStickMoving( void ) {
    StickMovement rs;

    padGetRightStick( &rs );

    if ( padIsStickMoving( &rs ) ) {
        inputAddMouseMoveRelative(
            ( int ) ( rs.dx * Config_Cursor_Movement_Scale ),
            ( int ) ( rs.dy * Config_Cursor_Movement_Scale )
        );

        debugMessage( L"Right stick moving; deltas: %.2f, %.2f\n", rs.dx, rs.dy );
    }
}

static void onR2ButtonDown( void ) {
    debugMessage( L""__FUNCTION__ );
    inputAddKeyDownEvent( VK_SHIFT );
}

static void onR2ButtonUp( void ) {
    debugMessage( L""__FUNCTION__ );
    inputAddKeyUpEvent( VK_SHIFT );
}

MakeKeyFunc( XINPUT_GAMEPAD_B, VK_F1, VK_F5, true, onBButtonDown );
MakeKeyFunc( XINPUT_GAMEPAD_A, VK_F2, VK_F6, true, onAButtonDown );
MakeKeyFunc( XINPUT_GAMEPAD_X, VK_F3, VK_F7, true, onXButtonDown );
MakeKeyFunc( XINPUT_GAMEPAD_Y, VK_F4, VK_F8, true, onYButtonDown );

MakeKeyFunc( XINPUT_GAMEPAD_DPAD_RIGHT, '1', '1', false, onDpadRightDown );
MakeKeyFunc( XINPUT_GAMEPAD_DPAD_DOWN, '2', '2', false, onDpadDownDown );
MakeKeyFunc( XINPUT_GAMEPAD_DPAD_LEFT, '3', '3', false, onDpadLeftDown );
MakeKeyFunc( XINPUT_GAMEPAD_DPAD_UP, '4', '4', false, onDpadUpDown );

MakeKeyFunc( XINPUT_GAMEPAD_LEFT_THUMB, 'R', 'R', false, onLStickDown );
MakeKeyFunc( XINPUT_GAMEPAD_RIGHT_THUMB, VK_TAB, VK_TAB, false, onRStickDown );

MakeKeyFunc( XINPUT_GAMEPAD_START, 'I', 'A', false, onStartButtonDown );
MakeKeyFunc( XINPUT_GAMEPAD_BACK, VK_ESCAPE, VK_ESCAPE, false, onBackButtonDown );

MakeKeyFunc( XINPUT_GAMEPAD_LEFT_SHOULDER, 0, 'W', false, onLButtonDown );

void setDefaultKeyBinds( void ) {
    padAddBinding( Button_LStickMoving, onLeftStickStartedMoving, onLeftStickMoving, onLeftStickStoppedMoving );
    padAddBinding( Button_RStickMoving, NULL, onRightStickMoving, NULL );

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
    padAddBinding( Button_R2, onR2ButtonDown, NULL, onR2ButtonUp );

    padAddBinding( XINPUT_GAMEPAD_LEFT_SHOULDER, onLButtonDown, NULL, NULL );
}
