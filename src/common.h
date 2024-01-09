#pragma once

#include <windows.h>
#include <xinput.h>
#include <stdbool.h>
#include <stdint.h>

#define clamp( value, min, max ) { \
    ( value ) = ( ( value ) < min ) ? min : ( value ); \
    ( value ) = ( ( value ) > max ) ? max : ( value ); \
}

#define Config_Max_Input_Events 64
#define Config_Max_Key_Bindings 32
#define Config_Movement_Thresh 0.15f
#define Config_Cursor_Movement_Scale 6.0f
#define Config_Update_Rate 10

#define Config_Cursor_Stick Stick_Right
#define Config_Movement_Stick Stick_Left
#define Config_Button_Click XINPUT_GAMEPAD_RIGHT_SHOULDER

// Source: https://stackoverflow.com/a/14421702
#define StrWide1( s ) L##s
#define StrWide( s ) StrWide1( s )

void setDefaultKeyBinds( void );
void versionProxyInit( void );
