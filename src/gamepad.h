#pragma once

#include "common.h"

typedef void ( *ButtonProc ) ( void );

typedef enum {
    Stick_Left = 0,
    Stick_Right
} StickType;

typedef struct {
    float dx;
    float dy;
} StickMovement;

typedef struct {
    uint32_t joyButtonMask;

    ButtonProc onButtonDown;
    ButtonProc onButtonHeld;
    ButtonProc onButtonUp;
} JoyToKeyEntry;


#define Button_Meta 0x80000000
#define Button_LStickMoving 0x40000000
#define Button_RStickMoving 0x20000000
#define Button_Z 0x10000000

void padInit( void );
void padGetLeftStick( StickMovement* mov );
void padGetRightStick( StickMovement* mov );
bool padIsStickMoving( StickMovement* mov );
void padUpdate( void );
uint32_t padGetKeysDown( void );
uint32_t padGetKeysHeld( void );
uint32_t padGetKeysUp( void );
bool padAddBinding( uint32_t buttonMask, ButtonProc onButtonDown, ButtonProc onButtonHeld, ButtonProc onButtonUp );
