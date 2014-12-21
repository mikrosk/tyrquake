/*
 * snd_atari.c -- sound and dma handling for Atari Falcon 060
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

#include <mint/falcon.h>
#include <mint/osbind.h>
#include <mint/ostruct.h>

#include "snd_atari_asm.h"

#include "quakedef.h"
#include "sound.h"
#include "console.h"

dma_t atari_shm;

#define DMA_BUFFER_SIZE	64*1024*2	// 64K for each channel

typedef struct
{
	void* playptr;
	void* recordptr;
	void* reserved1;
	void* reserved2;
} SBUFPTR;

static qboolean isSndInited = false;

qboolean SNDDMA_Init(void)
{
	if( Locksnd() == SNDLOCKED )
	{
		Con_Printf( "Sound system is already in use!\n" );
		return false;
	}
	
#if defined(USE_M68K_ASM)
	// save sound regs
	sound_atari_init();
#endif
	
	memset( &atari_shm, 0, sizeof( dma_t ) );
	
	shm = &atari_shm;
	
	shm->samplebits = 16;
	//shm->speed = 12292;
	shm->speed = 24585;
	//shm->speed = 32880;
	shm->channels = 2;
	
	shm->samples = DMA_BUFFER_SIZE/2;	// number of mono samples
	shm->submission_chunk = 1;
	shm->buffer = (unsigned char*)Mxalloc( DMA_BUFFER_SIZE + 1, MX_STRAM );
	if( shm->buffer == NULL )
	{
		Con_Printf("Not enough memory for dma buffer\n");
		return false;
	}
	shm->buffer = (unsigned char*)( ( (unsigned long)shm->buffer + 1 ) & 0xfffffffe );
	
	memset( shm->buffer, 0, DMA_BUFFER_SIZE );
	
	Sndstatus( SND_RESET );
	Soundcmd( ADDERIN, MATIN );	/* input from connection matrix */
	
	//Devconnect( DMAPLAY, DAC, CLK25M, CLK12K, NO_SHAKE );	/* DMA playback -> DAC */
	Devconnect( DMAPLAY, DAC, CLK25M, CLK25K, NO_SHAKE );	/* DMA playback -> DAC */
	Setmode( MODE_STEREO16 );
	
	Dsptristate( DSP_TRISTATE, DSP_TRISTATE );
	
	Setbuffer( SR_PLAY, shm->buffer, shm->buffer + DMA_BUFFER_SIZE );
	Buffoper( SB_PLA_ENA | SB_PLA_RPT );	/* enable playback in loop mode */
	
	shm->samplepos = 0;
	isSndInited = true;
	return true;
}

int SNDDMA_LockBuffer(void)
{
	return 0;
}
    
void SNDDMA_UnlockBuffer(void)
{
}
    

int SNDDMA_GetDMAPos(void)
{
	SBUFPTR SDmaBuffers;
	
	if( !isSndInited )
	{
		return 0;
	}
	
	if( Buffptr( &SDmaBuffers ) != 0 )
	{
		return 0;
	}

	shm->samplepos = ( (int)SDmaBuffers.playptr - (int)shm->buffer ) / 2;

	return shm->samplepos;

}

void SNDDMA_Shutdown(void)
{
	if( isSndInited )
	{
		Buffoper( 0x0000 );	/* disable playback */
		Devconnect( 0x0000, DAC, CLK25M, CLK12K, NO_SHAKE );	/* nothing -> DAC */
		Unlocksnd();
		
#if defined(USE_M68K_ASM)
		// restore sound regs
		sound_atari_shutdown();
#endif
	}
}

/*
==============
SNDDMA_Submit

Send sound to device if buffer isn't really the dma buffer
===============
*/
void SNDDMA_Submit(void)
{
}

