#pragma once

#include "common.h"

void inputBegin( void );
void inputEnd( void );

bool inputAddKeyDownEvent( int vkey );
bool inputAddKeyUpEvent( int vkey );
bool inputAddMouseMoveRelative( int dx, int dy );
bool inputAddMouseMoveAbsolute( int x, int y );
bool inputAddMouseLeftButton( bool isDown );
bool inputAddMouseRightButton( bool isDown );
