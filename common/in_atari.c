/*
Copyright (C) 1996-1997 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#include <mint/osbind.h>
#include <mint/ostruct.h>

#include "client.h"
#include "quakedef.h"

#include "keys_atari_asm.h"

cvar_t _windowed_mouse = { "_windowed_mouse", "1", true };

#define SCANCODE_BUFFER_SIZE 256
unsigned char g_scancodeBuffer[SCANCODE_BUFFER_SIZE];
int g_scancodeBufferHead = 0;
int g_scancodeBufferTail = 0;
int g_scancodeShiftDepressed = 0;

typedef struct
{
	int		mx;
	int		my;
	qboolean	leftButtonDepressed;
	qboolean	rightButtonDepressed;
} SMouse;
SMouse g_mouseInfo;
qboolean reset_mouse_deltas;

static knum_t unshiftToAscii[128];
static knum_t shiftToAscii[128];
static knum_t capsToAscii[128];

static qboolean isInputInitialized = false;
static qboolean mouse_available;
static float mouse_x, mouse_y;

static cvar_t m_filter = { "m_filter", "0" };

void
IN_Init(void)
{
    _KEYTAB*	pSKeyboards;
    int i;
    
    mouse_x = mouse_y = 0.0;
    mouse_available = !COM_CheckParm("-nomouse");
    Cvar_RegisterVariable(&m_filter);
    Cvar_RegisterVariable(&_windowed_mouse);
    
    // clear scancode buffer
    memset( g_scancodeBuffer, 0, SCANCODE_BUFFER_SIZE );

    // setup new ikbd handler
    atari_ikbd_init();

    // get translation tables
    pSKeyboards = Keytbl( KT_NOCHANGE, KT_NOCHANGE, KT_NOCHANGE );

    for( i = 0; i < 128; ++i )
    {
	    unshiftToAscii[i] = (knum_t)pSKeyboards->unshift[i];
	    shiftToAscii[i] = (knum_t)pSKeyboards->shift[i];
	    capsToAscii[i] = (knum_t)pSKeyboards->caps[i];
    }

    // patch for atari scancodes
    unshiftToAscii[0x0f] = K_TAB;
    unshiftToAscii[0x1c] = K_ENTER;
    unshiftToAscii[0x01] = K_ESCAPE;
    unshiftToAscii[0x39] = K_SPACE;

    unshiftToAscii[0x0e] = K_BACKSPACE;
    unshiftToAscii[0x48] = K_UPARROW;
    unshiftToAscii[0x50] = K_DOWNARROW;
    unshiftToAscii[0x4b] = K_LEFTARROW;
    unshiftToAscii[0x4d] = K_RIGHTARROW;

    unshiftToAscii[0x38] = K_ALT;
    unshiftToAscii[0x1d] = K_CTRL;
    unshiftToAscii[0x2a] = K_LSHIFT;
    unshiftToAscii[0x36] = K_RSHIFT;
    unshiftToAscii[0x3b] = K_F1;
    unshiftToAscii[0x3c] = K_F2;
    unshiftToAscii[0x3d] = K_F3;
    unshiftToAscii[0x3e] = K_F4;
    unshiftToAscii[0x3f] = K_F5;
    unshiftToAscii[0x40] = K_F6;
    unshiftToAscii[0x41] = K_F7;
    unshiftToAscii[0x42] = K_F8;
    unshiftToAscii[0x43] = K_F9;
    unshiftToAscii[0x44] = K_F10;
    unshiftToAscii[0x62] = K_F11;	// help
    unshiftToAscii[0x61] = K_F12;	// undo
    unshiftToAscii[0x52] = K_INS;
    unshiftToAscii[0x53] = K_DEL;
    unshiftToAscii[0x46] = K_PGDN;	// eiffel only
    unshiftToAscii[0x45] = K_PGUP;	// eiffel only
    unshiftToAscii[0x47] = K_HOME;
    unshiftToAscii[0x55] = K_END;	// eiffel only

    unshiftToAscii[0x4f] = K_PAUSE;	// eiffel only
    unshiftToAscii[0x59] = K_MWHEELUP;	// eiffel only
    unshiftToAscii[0x5a] = K_MWHEELDOWN;	// eiffel only
    unshiftToAscii[0x5b] = '`';	// eiffel only

    shiftToAscii[0x0f] = K_TAB;
    shiftToAscii[0x1c] = K_ENTER;
    shiftToAscii[0x01] = K_ESCAPE;
    shiftToAscii[0x39] = K_SPACE;

    shiftToAscii[0x0e] = K_BACKSPACE;
    shiftToAscii[0x48] = K_UPARROW;
    shiftToAscii[0x50] = K_DOWNARROW;
    shiftToAscii[0x4b] = K_LEFTARROW;
    shiftToAscii[0x4d] = K_RIGHTARROW;

    shiftToAscii[0x38] = K_ALT;
    shiftToAscii[0x1d] = K_CTRL;
    shiftToAscii[0x2a] = K_LSHIFT;
    shiftToAscii[0x36] = K_RSHIFT;
    shiftToAscii[0x3b] = K_F1;
    shiftToAscii[0x3c] = K_F2;
    shiftToAscii[0x3d] = K_F3;
    shiftToAscii[0x3e] = K_F4;
    shiftToAscii[0x3f] = K_F5;
    shiftToAscii[0x40] = K_F6;
    shiftToAscii[0x41] = K_F7;
    shiftToAscii[0x42] = K_F8;
    shiftToAscii[0x43] = K_F9;
    shiftToAscii[0x44] = K_F10;
    shiftToAscii[0x62] = K_F11;	// help
    shiftToAscii[0x61] = K_F12;	// undo
    shiftToAscii[0x52] = K_INS;
    shiftToAscii[0x53] = K_DEL;
    shiftToAscii[0x46] = K_PGDN;	// eiffel only
    shiftToAscii[0x45] = K_PGUP;	// eiffel only
    shiftToAscii[0x47] = K_HOME;
    shiftToAscii[0x55] = K_END;	// eiffel only

    shiftToAscii[0x4f] = K_PAUSE;	// eiffel only
    shiftToAscii[0x59] = K_MWHEELUP;	// eiffel only
    shiftToAscii[0x5a] = K_MWHEELDOWN;	// eiffel only
    shiftToAscii[0x5b] = '~';	// eiffel only
    
    isInputInitialized = true;
}

void
IN_Shutdown(void)
{
    mouse_available = false;

    if( isInputInitialized )
    {
	atari_ikbd_shutdown();
    }
}

void
IN_Commands(void)
{
    if( mouse_available )
    {
	Key_Event( K_MOUSE1, g_mouseInfo.leftButtonDepressed );
	Key_Event( K_MOUSE2, g_mouseInfo.rightButtonDepressed );
    }
}

void
IN_Move(usercmd_t *cmd)
{
    static float old_mouse_x, old_mouse_y;

    if (!mouse_available)
	return;
    
    mouse_x = g_mouseInfo.mx;
    mouse_y = g_mouseInfo.my;
    
    reset_mouse_deltas = true;
    g_mouseInfo.mx = 0;
    g_mouseInfo.my = 0;

    if (m_filter.value) {
	mouse_x = (mouse_x + old_mouse_x) * 0.5;
	mouse_y = (mouse_y + old_mouse_y) * 0.5;
    }

    old_mouse_x = mouse_x;
    old_mouse_y = mouse_y;

    mouse_x *= sensitivity.value;
    mouse_y *= sensitivity.value;

    if ((in_strafe.state & 1) || (lookstrafe.value && ((in_mlook.state & 1) ^ (int)m_freelook.value)))
	cmd->sidemove += m_side.value * mouse_x;
    else
	cl.viewangles[YAW] -= m_yaw.value * mouse_x;
    if ((in_mlook.state & 1) ^ (int)m_freelook.value)
	if (mouse_x || mouse_y)
	    V_StopPitchDrift();

    if (((in_mlook.state & 1) ^ (int)m_freelook.value) && !(in_strafe.state & 1)) {
	cl.viewangles[PITCH] += m_pitch.value * mouse_y;
	if (cl.viewangles[PITCH] > 80)
	    cl.viewangles[PITCH] = 80;
	if (cl.viewangles[PITCH] < -70)
	    cl.viewangles[PITCH] = -70;
    } else {
	if ((in_strafe.state & 1) && noclip_anglehack)
	    cmd->upmove -= m_forward.value * mouse_y;
	else
	    cmd->forwardmove -= m_forward.value * mouse_y;
    }
    
    mouse_x = mouse_y = 0.0;
}

void Sys_SendKeyEvents (void)
{
    unsigned char scancode;
    knum_t ascii;

    while( g_scancodeBufferHead != g_scancodeBufferTail )
    {
	scancode = g_scancodeBuffer[g_scancodeBufferTail++];
	g_scancodeBufferTail &= SCANCODE_BUFFER_SIZE-1;

	// it's fucking important to pass this ascii value as
	// unsigned char !!!
	if( g_scancodeShiftDepressed == 0 )
	{
	    ascii = unshiftToAscii[scancode & 0x7f];
	}
	else
	{
	    g_scancodeShiftDepressed = 0;
	    ascii = shiftToAscii[scancode & 0x7f];
	}

	if( ( scancode & 0x80 ) == 0 )
	{
	    Key_Event( ascii, true );
	}
	else
	{
	    Key_Event( ascii, false );
	}
    }
    
    if( mouse_available )
    {
	Key_Event( K_MOUSE1, g_mouseInfo.leftButtonDepressed );
	Key_Event( K_MOUSE2, g_mouseInfo.rightButtonDepressed );
    }
}
