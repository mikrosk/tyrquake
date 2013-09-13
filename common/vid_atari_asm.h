/*
 * vid_atari_asm.h -- declarations for vid_atari_asm.s
 *
 * Copyright (c) 2006 Miro Kropacek; miro.kropacek@gmail.com
 * 
 * This file is part of the Atari Quake project, 3D shooter game by ID Software,
 * for Atari Falcon 060 computers.
 *
 * Atari Quake is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Atari Quake is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Atari Quake; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _VID_ATARI_ASM_H_
#define _VID_ATARI_ASM_H_

extern void video_atari_init( const byte* screen );
extern void video_atari_shutdown( void );
extern void video_atari_set_palette( const byte* palette );
extern void video_atari_c2p( char* buffer, const byte* screen, int size );
extern void video_atari_set_320x200( void );

#endif
