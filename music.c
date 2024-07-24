/*
 * Heart of The Alien: Music player (cd and mp3)
 * Copyright (c) 2004 Gil Megidish
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
#ifdef DREAMCAST
	#include <SDL.h>
	#include <kos.h>
	#include <mp3/sndserver.h>
	#include "debug.h"
	#include "music.h"
#else
	#include <SDL.h>
	#include <SDL_mixer.h>
	#include "debug.h"
	#include "music.h"
#endif


///
extern int nosound_flag;
/////

static SDL_CD *sdl_cd;

#ifndef DREAMCAST
static Mix_Music *current_track;
#endif


/* stop cd player */
static void stop_music_cd()
{
	SDL_CDStop(sdl_cd);
}

/* stop mp3 player */
static void stop_music_mp3()
{

#ifdef DREAMCAST
        mp3_stop();
#else
	if (current_track != NULL)
	{
		Mix_FreeMusic(current_track);
		current_track = NULL;

	}
#endif

}

/* stop music */
void stop_music()
{
	if (nosound_flag)
	{
		return;
	}

	if (get_iso_toggle())
	{
		stop_music_mp3();	
	}
	else
	{
		stop_music_cd();
	}
}

/* play a single track from cd */
static void play_music_track_cd(int track, int loop)
{
	LOG(("play_music_track_cd(track=%d, loop=%d, sdlcd=0x%x)\n", track, loop, sdl_cd));

	if (CD_INDRIVE(SDL_CDStatus(sdl_cd)))
	{
		int err = SDL_CDPlayTracks(sdl_cd, track, 0, 1, 0);
		LOG(("SDL_CDPlay returned %d\n", err));
	}
	else
	{
		LOG(("can't play cd audio, CD_INDRIVE failed\n"));
	}
}

/* play an mp3 in background */
static void play_music_track_mp3(int track, int loop)
{
	char filename[256];

	stop_music_mp3();

	sprintf(filename, "Heart Of The Alien (U) %02d.mp3", track + 1);
	LOG(("playing mp3 %s\n", filename));

#ifdef DREAMCAST
    mp3_start(filename, loop);
#else
	current_track = Mix_LoadMUS(filename);
	Mix_PlayMusic(current_track, loop);
#endif

}

/* play audio track */
void play_music_track(int track, int loop)
{
	if (nosound_flag)
	{
		printf("no sound\n");
		return;
	}

	stop_music();
	if (get_iso_toggle())
	{
		play_music_track_mp3(track, loop);
	}
	else
	{
		play_music_track_cd(track, loop);
	}
}

/* callback after a frame has been rendered */
void music_update()
{
}

/* module initializer */
void music_init()
{
	if (nosound_flag)
	{
		return;
	}
#ifdef DREAMCAST
    snd_stream_init();
	mp3_init();
#endif
	if (get_iso_toggle() == 0)
	{
		/* initialize SDL CDROM, for playing audio tracks */
		sdl_cd = SDL_CDOpen(0);
	}
}
