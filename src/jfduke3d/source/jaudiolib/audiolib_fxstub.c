//-------------------------------------------------------------------------
/*
Duke Nukem Copyright (C) 1996, 2003 3D Realms Entertainment

This file is part of Duke Nukem 3D version 1.5 - Atomic Edition

Duke Nukem 3D is free software; you can redistribute it and/or
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

  Dummy AudioLib stub implementation by Jonathon Fowler (jonof@edgenetwk.com)
*/
//-------------------------------------------------------------------------

#include "fx_man.h"
#include "duke3d.h"
#include <SDL_mixer.h>


#define TRUE  ( 1 == 1 )
#define FALSE ( !TRUE )


int FX_ErrorCode = FX_Ok;

#define FX_SetErrorCode( status ) \
   FX_ErrorCode = ( status );





/*---------------------------------------------------------------------
   Function: FX_ErrorString

   Returns a pointer to the error message associated with an error
   number.  A -1 returns a pointer the current error.
---------------------------------------------------------------------*/

char *FX_ErrorString(int ErrorNumber)
{
   char *ErrorString;

   switch( ErrorNumber )
      {
      case FX_Warning :
      case FX_Error :
         ErrorString = FX_ErrorString( FX_ErrorCode );
         break;

      case FX_Ok :
         ErrorString = "Fx ok.";
         break;

      case FX_ASSVersion :
         ErrorString = "Apogee Sound System Version 0  "
            "Programmed by Jim Dose\n"
            "(c) Copyright 1995 James R. Dose.  All Rights Reserved.\n";
         break;

      default :
         ErrorString = "Unknown Fx error code.";
         break;
      }

   return( ErrorString );
}


/*---------------------------------------------------------------------
   Function: FX_Init

   Selects which sound device to use.
---------------------------------------------------------------------*/

int FX_Init(int SoundCard, int numvoices, int numchannels, int samplebits, unsigned mixrate)
{
	initprintf("about to open audio!\n");

	if (Mix_OpenAudio(mixrate, MIX_DEFAULT_FORMAT, numchannels, 256)) {
		initprintf("Unable to open audio!\n");
		return FX_Error;
	}
	initprintf("success open audio!\n");
	Mix_AllocateChannels(16);

	return( FX_Ok );
}


/*---------------------------------------------------------------------
   Function: FX_Shutdown

   Terminates use of sound device.
---------------------------------------------------------------------*/

int FX_Shutdown(void)
{
	Mix_CloseAudio();
	return( FX_Ok );
}


/*---------------------------------------------------------------------
   Function: FX_SetCallback

   Sets the function to call when a voice is done.
---------------------------------------------------------------------*/

int FX_SetCallBack(void ( *function )( unsigned long ))
{
	Mix_ChannelFinished(function);
	return( FX_Ok );
}


/*---------------------------------------------------------------------
   Function: FX_SetVolume

   Sets the volume of the current sound device.
---------------------------------------------------------------------*/

void FX_SetVolume(int volume)
{
	Mix_Volume(-1, volume);
}


/*---------------------------------------------------------------------
   Function: FX_SetReverseStereo

   Set the orientation of the left and right channels.
---------------------------------------------------------------------*/

void FX_SetReverseStereo(int setting)
{
}


/*---------------------------------------------------------------------
   Function: FX_GetReverseStereo

   Returns the orientation of the left and right channels.
---------------------------------------------------------------------*/

int FX_GetReverseStereo(void)
{
	return 0;
}


/*---------------------------------------------------------------------
   Function: FX_SetReverb

   Sets the reverb level.
---------------------------------------------------------------------*/

void FX_SetReverb(int reverb)
{
}


/*---------------------------------------------------------------------
   Function: FX_SetReverbDelay

   Sets the delay level of reverb to add to mix.
---------------------------------------------------------------------*/

void FX_SetReverbDelay(int delay)
{
}


/*---------------------------------------------------------------------
   Function: FX_VoiceAvailable

   Checks if a voice can be play at the specified priority.
---------------------------------------------------------------------*/

int FX_VoiceAvailable(int priority)
{
	return 1;
}


/*---------------------------------------------------------------------
   Function: FX_PlayLoopedVOC

   Begin playback of sound data with the given volume and priority.
---------------------------------------------------------------------*/

int FX_PlayLoopedVOC(void *ptr, long loopstart, long loopend, int pitchoffset, int vol, int left, int right, int priority, unsigned long callbackval)
{
	int phaserChannel = Mix_PlayChannel(-1, ptr, -1);
	initprintf("FX_PlayLoopedVOC %d\n", phaserChannel+1);
	return( phaserChannel+1 );
}


/*---------------------------------------------------------------------
   Function: FX_PlayVOC3D

   Begin playback of sound data at specified angle and distance
   from listener.
---------------------------------------------------------------------*/

int FX_PlayVOC3D(void *ptr, int pitchoffset, int angle, int distance, int priority, unsigned long callbackval)
{
	int phaserChannel = Mix_PlayChannel(-1, ptr, 0);
	initprintf("FX_PlayVOC3D %d 0x%x\n", phaserChannel+1, ptr);
	Mix_SetPosition(phaserChannel, angle, distance);
	return( phaserChannel+1 );
}

int FX_PlayVOC(void *ptr, long size)
{
	int phaserChannel = Mix_PlayChannel(-1, ptr, 0);
	initprintf("FX_PlayVOC %d\n", phaserChannel+1);
	return( phaserChannel+1 );
}


/*---------------------------------------------------------------------
   Function: FX_PlayWAV

   Begin playback of sound data with the given volume and priority.
---------------------------------------------------------------------*/

int FX_PlayLoopedWAV(void *ptr, long loopstart, long loopend, int pitchoffset, int vol, int left, int right, int priority, unsigned long callbackval )
{
	int phaserChannel = Mix_PlayChannel(-1, ptr, -1);
	return( phaserChannel+1 );
}


/*---------------------------------------------------------------------
   Function: FX_PlayWAV3D

   Begin playback of sound data at specified angle and distance
   from listener.
---------------------------------------------------------------------*/

int FX_PlayWAV3D(void *ptr, int pitchoffset, int angle, int distance, int priority, unsigned long callbackval)
{
	int phaserChannel = Mix_PlayChannel(-1, ptr, 0);
	Mix_SetPosition(phaserChannel, angle, distance);
	return( phaserChannel+1 );
}

int FX_PlayWAV(void *ptr, long size)
{
	int phaserChannel = Mix_PlayChannel(-1, ptr, 0);
	return( phaserChannel+1 );
}

/*---------------------------------------------------------------------
   Function: FX_Pan3D

   Set the angle and distance from the listener of the voice associated
   with the specified handle.
---------------------------------------------------------------------*/

int FX_Pan3D(int handle, int angle, int distance)
{
	initprintf("FX_Pan3D %d\n", handle-1);
	Mix_SetPosition(handle-1, angle, distance);
	return( 0 );
}


/*---------------------------------------------------------------------
   Function: FX_StopSound

   Halts playback of a specific voice
---------------------------------------------------------------------*/

int FX_StopSound(int handle)
{
	initprintf("FX_StopSound %d\n", handle-1);
	Mix_HaltChannel(handle-1);
	return( FX_Ok );
}


/*---------------------------------------------------------------------
   Function: FX_StopAllSounds

   Halts playback of all sounds.
---------------------------------------------------------------------*/

int FX_StopAllSounds(void)
{
	initprintf("FX_StopAllSounds\n");
	Mix_HaltChannel(-1);
	return( FX_Ok );
}


void AudioUpdate(void)
{
}
