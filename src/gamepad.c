#include <windows.h>
#include <xinput.h>
#include "common.h"
#include "gamepad.h"
#include "util.h"

static void getStickForInput( XINPUT_GAMEPAD* gp, StickType whichStick, float* sx, float* sy );
static float normalizeStick( SHORT stickValue );
static int padGetFirstController( void );
static JoyToKeyEntry* getFreeKeyBinding( void );

static XINPUT_STATE lastInputState;
static XINPUT_STATE curInputState;
static int playerId = -1;
static uint32_t keysHeld = 0;
static uint32_t keysDown = 0;
static uint32_t keysUp = 0;

static uint32_t lastButtonState = 0;
static uint32_t curButtonState = 0;

static JoyToKeyEntry keyBindingTable[ Config_Max_Key_Bindings ];
static int keyBindingCount = 0;

static bool lastLeftStickMoving = false;
static bool lastRightStickMoving = false;

static int padGetFirstController( void ) {
    XINPUT_STATE xst;

    for ( int i = 0; i < 4; i++ ) {
        if ( XInputGetState( i, &xst ) == ERROR_SUCCESS )
            return i;
    }

    return -1;
}

void padInit( void ) {
    memset( &lastInputState, 0, sizeof( XINPUT_STATE ) );
    memset( &curInputState, 0, sizeof( XINPUT_STATE ) );
    memset( keyBindingTable, 0, sizeof( keyBindingTable ) );

    keyBindingCount = 0;
}

static void getStickForInput( XINPUT_GAMEPAD* gp, StickType whichStick, float* sx, float* sy ) {
    SHORT x = 0;
    SHORT y = 0;

    if ( gp && sx && sy ) {
        x = ( whichStick == Stick_Left ) ? gp->sThumbLX : gp->sThumbRX;
        y = ( whichStick == Stick_Left ) ? gp->sThumbLY : gp->sThumbRY;

        *sx = normalizeStick( x );
        *sy = -normalizeStick( y );

        clamp( *sx, -1.0f, 1.0f );
        clamp( *sy, -1.0f, 1.0f );
    }
}

void padGetLeftStick( StickMovement* mov ) {
    if ( mov ) {
        getStickForInput( &curInputState.Gamepad, Stick_Left, &mov->dx, &mov->dy );
    }
}

void padGetRightStick( StickMovement* mov ) {
    if ( mov ) {
        getStickForInput( &curInputState.Gamepad, Stick_Right, &mov->dx, &mov->dy );
    }
}

bool padIsStickMoving( StickMovement* mov ) {
    bool moving = false;

    if ( mov ) {
        moving = ( mov->dx <= -Config_Movement_Thresh || mov->dx >= Config_Movement_Thresh ) || 
            ( mov->dy <= -Config_Movement_Thresh || mov->dy >= Config_Movement_Thresh )
        ;
    }

    return moving;
}

static float normalizeStick( SHORT stickValue ) {
    return ( ( float ) stickValue ) / 32768.0f;
}

static void printBinary32( uint32_t b ) {
    for ( int i = 0; i < 32; i++ ) {
        utilDebugMessage( L"%c", ( b & 0x80000000 ) ? L'1' : L'0' );
        b <<= 1;
    }

    utilDebugMessage( L"\n" );
}

void padUpdate( void ) {
    JoyToKeyEntry* bind = NULL;
    StickMovement ls;
    StickMovement rs;
    int err = 0;
    int i = 0;

    if ( ( playerId = padGetFirstController( ) ) >= 0 ) {
        err = XInputGetState( playerId, &curInputState );

        if ( err == ERROR_SUCCESS ) {
            padGetLeftStick( &ls );
            padGetRightStick( &rs );

            curButtonState = curInputState.Gamepad.wButtons;

            curButtonState |= padIsStickMoving( &ls ) ? Button_LStickMoving : 0;
            curButtonState |= padIsStickMoving( &rs ) ? Button_RStickMoving : 0;
            curButtonState |= ( curInputState.Gamepad.bLeftTrigger >= 64 ) ? Button_Meta : 0;

            keysHeld = curButtonState;
            keysDown = curButtonState & ~lastButtonState;
            keysUp = lastButtonState & ~curButtonState;

            for ( i = 0; i < keyBindingCount; i++ ) {
                bind = &keyBindingTable[ i ];

                if ( bind ) {
                    if ( ( keysHeld & bind->joyButtonMask ) && bind->onButtonHeld ) {
                        ( *bind->onButtonHeld ) ( );
                    }

                    if( ( keysDown & bind->joyButtonMask ) && bind->onButtonDown ) {
                        ( *bind->onButtonDown ) ( );
                    }

                    if ( ( keysUp & bind->joyButtonMask ) && bind->onButtonUp ) {
                        ( *bind->onButtonUp ) ( );
                    }
                }
            }

            memcpy( &lastInputState, &curInputState, sizeof( XINPUT_STATE ) );
            lastButtonState = curButtonState;
        }
    }
}

uint32_t padGetKeysDown( void ) {
    return keysDown;
}

uint32_t padGetKeysHeld( void ) {
    return keysHeld;
}

uint32_t padGetKeysUp( void ) {
    return keysUp;
}

static JoyToKeyEntry* getFreeKeyBinding( void ) {
    JoyToKeyEntry* result = NULL;

    if ( keyBindingCount < Config_Max_Key_Bindings ) {
        result = &keyBindingTable[ keyBindingCount++ ];

        if ( result ) {
            memset( result, 0, sizeof( JoyToKeyEntry ) );
        }
    }

    return result;
}

bool padAddBinding( uint32_t buttonMask, ButtonProc onButtonDown, ButtonProc onButtonHeld, ButtonProc onButtonUp ) {
    JoyToKeyEntry* bind = getFreeKeyBinding( );

    if ( bind ) {
        bind->joyButtonMask = buttonMask;
        bind->onButtonDown = onButtonDown;
        bind->onButtonHeld = onButtonHeld;
        bind->onButtonUp = onButtonUp;
    }

    return ( bind ) ? true : false;
}
