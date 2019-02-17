/* SNDDRV.C
  SDL sound driver for CloneKeen.

  Basically, sound_init() will load all sounds from SOUNDS.CK?.
  sound_play(snd,PLAY_NOW), where snd is a sound defined in src\sounds.h,
  will start that sound playing asyncrounsly. (sound_play is non-blocking).
*/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "../keen.h"
#include <SDL_audio.h>

#include "snddrv.h"
#include "snddrv.fdh"

char sound_on = 0;
extern uchar gl_retval;		// returned by gameloop

char SoundDrv_Start(void)
{
char name[32];

	lprintf("Starting sound driver...\n");
	
	// start up the SDL sound system
	AudioSpec.freq = 44100;
	AudioSpec.format = AUDIO_U8;
	AudioSpec.channels = 1;
	AudioSpec.silence = 0;        //512;
	AudioSpec.samples = 512;
	AudioSpec.callback = SoundDrv_Callback;
	AudioSpec.userdata = NULL;
	
	/* Initialize fillerup() variables */
	if ( SDL_OpenAudio(&AudioSpec, NULL) < 0 )
	{
		lprintf("SoundDrv_Start(): Couldn't open audio: %s\n", SDL_GetError());
		lprintf("Sound will be disabled.\n");
		sound_on = 0;
		return 0;
	}
	
	lprintf("SDL_AudioSpec:\n");
	lprintf("  freq: %d\n", AudioSpec.freq);
	lprintf("  channels: %d\n", AudioSpec.channels);
	lprintf("  audio buffer size: %d\n", AudioSpec.size);
	//lprintf("Using audio driver: %s\n", SDL_AudioDriverName(name, 32));
	
	sound_stop_all();
	SDL_PauseAudio(0);
	
	lprintf("SoundDrv_Start(): SDL sound system initilized.\n");
	sound_on = 1;
	return 0;
}

void SoundDrv_Stop(void)
{
   sound_stop_all();

   if (sound_on)
   {
     SDL_PauseAudio(1);
     SDL_CloseAudio();
   }
}

char load_sounds(uchar episode)
{
int ok;
char soundfile[80];

  sprintf(soundfile, "data/SOUNDS.CK%c", episode + '0');
  lprintf("load_sounds(): loading '%s'...\n", soundfile);

  ok  = sound_load(soundfile, "KEENWALKSND", SOUND_KEEN_WALK);
  ok |= sound_load(soundfile, "KEENWLK2SND", SOUND_KEEN_WALK2);
  ok |= sound_load(soundfile, "KEENJUMPSND", SOUND_KEEN_JUMP);
  ok |= sound_load(soundfile, "KEENPOGOSND", SOUND_KEEN_POGO);
  ok |= sound_load(soundfile, "KEENLANDSND", SOUND_KEEN_LAND);
  ok |= sound_load(soundfile, "KEENBLOKSND", SOUND_KEEN_BLOK);
  ok |= sound_load(soundfile, "KEENDIESND", SOUND_KEEN_DIE);
  ok |= sound_load(soundfile, "PLUMMETSND", SOUND_KEEN_FALL);
  ok |= sound_load(soundfile, "BUMPHEADSND", SOUND_KEEN_BUMPHEAD);
  ok |= sound_load(soundfile, "WLDENTERSND", SOUND_ENTER_LEVEL);
  ok |= sound_load(soundfile, "keensleft", SOUND_KEENSLEFT);

  ok |= sound_load(soundfile, "KEENFIRESND", SOUND_KEEN_FIRE);
  ok |= sound_load(soundfile, "GUNCLICK", SOUND_GUN_CLICK);
  ok |= sound_load(soundfile, "SHOTHIT", SOUND_SHOT_HIT);

  ok |= sound_load(soundfile, "GOTITEMSND", SOUND_GET_ITEM);
  ok |= sound_load(soundfile, "GOTBONUSSND", SOUND_GET_BONUS);
  ok |= sound_load(soundfile, "GOTPARTSND", SOUND_GET_PART);
  ok |= sound_load(soundfile, "LVLDONESND", SOUND_LEVEL_DONE);
  ok |= sound_load(soundfile, "GAMEOVERSND", SOUND_GAME_OVER);
  ok |= sound_load(soundfile, "TELEPORTSND", SOUND_TELEPORT);
  ok |= sound_load(soundfile, "EXTRAMANSND", SOUND_EXTRA_LIFE);
  ok |= sound_load(soundfile, "CHUNKSMASH", SOUND_CHUNKSMASH);
  ok |= sound_load(soundfile, "GOINDOORSND", SOUND_GOINDOOR);
  ok |= sound_load(soundfile, "GETCARDSND", SOUND_GET_CARD);
  ok |= sound_load(soundfile, "USEKEYSND", SOUND_USE_KEY);
  ok |= sound_load(soundfile, "CLICKSND", SOUND_SWITCH_TOGGLE);
  ok |= sound_load(soundfile, "DOOROPENSND", SOUND_DOOR_OPEN);

  ok |= sound_load(soundfile, "YORPBUMPSND", SOUND_YORP_BUMP);
  ok |= sound_load(soundfile, "YORPBOPSND", SOUND_YORP_STUN);
  ok |= sound_load(soundfile, "YORPSCREAM", SOUND_YORP_DIE);
  ok |= sound_load(soundfile, "GARGSCREAM", SOUND_GARG_DIE);
  ok |= sound_load(soundfile, "vortscream", SOUND_VORT_DIE);
  ok |= sound_load(soundfile, "TANKFIRE", SOUND_TANK_FIRE);

  if (episode==2)
  {
    ok |= sound_load(soundfile, "EARTHPOW", SOUND_EARTHPOW);
  }
  else if (episode==3)
  {
    ok |= sound_load(soundfile, "MEEP", SOUND_MEEP);
    ok |= sound_load(soundfile, "ANKH", SOUND_ANKH);
    ok |= sound_load(soundfile, "MORTIMER", SOUND_MORTIMER);
    ok |= sound_load(soundfile, "FOOTSLAM", SOUND_FOOTSLAM);
  }

  return ok;
}

// loads sound searchname from file fname, into sounds[] entry loadnum
// return value is nonzero on failure
char sound_load(char *fname, char *searchname, char loadnum)
{
FILE *fp;
int curpos;
int i,j;
int offset, priority, nr_of_sounds;
int sndbyte;
char name[12];

	for(i=0;i<12;i++) name[i] = 0;
	fp = fcaseopen(fname, "rb");
	if (!fp)
	{
		lprintf("sound_load(): sounds file '%s' unopenable attempting load of '%s'\n", fname, searchname);
		return 1;
	}
	
	__fseek(fp, 0x6, SEEK_SET);
	nr_of_sounds = fgeti(fp);
	
	// search for requested sound in header
	curpos = 0x10;
	for(j=0;(j<nr_of_sounds)||__feof(fp);j++)
	{
		__fseek(fp, curpos, SEEK_SET);
		offset = fgeti(fp);
		priority = __fgetc(fp);
		__fgetc(fp);
		for(i=0;i<12;i++) name[i] = __fgetc(fp);
		if (!strcmp(name, searchname)) goto sound_found;
		
		curpos += 0x10;
	}
	// sound could not be found
	crash("sound_load(): sound %s could not be found in %s.\n", searchname, fname);
	__fclose(fp);
	return 1;
sound_found: ;
     __fseek(fp, offset, SEEK_SET);
     for(i=0;i<MAX_SOUND_LENGTH;i++)
     {
       sndbyte = fgeti(fp);
       if (sndbyte != 0x0000 && sndbyte != 0xFFFF)
       {
         sounds[loadnum].sounddata[i] = SOUND_FREQ_DIVISOR / sndbyte;
       }
       else
       {
			sounds[loadnum].sounddata[i] = sndbyte;
			if (sndbyte==0xffff) break;
       }
     }
     sounds[loadnum].priority = priority;
     lprintf("sound_load(): loaded sound %s, %d bytes.\n", searchname, i);

     __fclose(fp);
     return 0;
}

// returns 1 if sound snd is currently playing
char sound_is_playing(int snd)
{
int i;
   for(i=0;i<NUM_CHANNELS;i++)
   {
     if (SndChannels[i].sound_playing)
       if (SndChannels[i].current_sound==snd)
         return 1;
   }
   return 0;
}

// returns 1 if a sound is currently playing in PLAY_FORCE mode
char forced_sound_is_playing(void)
{
int i;
   for(i=0;i<NUM_CHANNELS;i++)
   {
     if (SndChannels[i].sound_playing)
       if (SndChannels[i].sound_forced)
         return 1;
   }
   return 0;
}

// if sound snd is currently playing, stops it immediately
void sound_stop(int snd)
{
int chnl;
  for(chnl=0;chnl<NUM_CHANNELS;chnl++)
  {
    if (SndChannels[chnl].sound_playing)
    {
      if (SndChannels[chnl].current_sound==snd)
      {
		SndChannels[chnl].sound_playing = 0;
      }
    }
  }
}

// stops all currently playing sounds
void sound_stop_all(void)
{
int chnl;
   for(chnl=0;chnl<NUM_CHANNELS;chnl++)
   {
     SndChannels[chnl].current_sound = 0;
     SndChannels[chnl].sound_ptr = 0;
     SndChannels[chnl].sound_timer = 0;
     SndChannels[chnl].sound_playing = 0;
     SndChannels[chnl].waveState = WAVE_IN;
     SndChannels[chnl].freqtimer = 0;
   }
}

// pauses any currently playing sounds
void sound_pause(void)
{
//   if (sound_on) SDL_PauseAudio(1);
	sound_stop_all();
}

// resumes playing a previously paused sound
void sound_resume(void)
{
   //if (sound_on) SDL_PauseAudio(0);
}


// generates len bytes of waveform for channel "Chan".
void SoundDrv_GenWaveform(stSoundChannel *Chan, unsigned int len)
{
unsigned long halffreq;
unsigned int index;
char firsttime;
	
	//lprintf("Entry to GenWaveform len = %d\n", len);
	
	/*
		assuming sample rate = 11025 hz...
		if we want a frequency of 5512 hz we would have
		to change the wave once every sample so that it
		makes a complete wave every 2 samples
	*/
	halffreq = (AudioSpec.freq / 2);
	
	// setup so we process a new byte of the sound first time through
	firsttime = 1;
	
	for(index=0;index<len;index++)
	{
		if (!Chan->sound_timer || firsttime)
		{
			// get new frequency and compute how fast we have to
			// change the wave data
			Chan->desiredfreq = sounds[Chan->current_sound].sounddata[Chan->sound_ptr];
			
			if (Chan->desiredfreq==0xffff)
			{  // end of sound...fill rest of buffer with silence
				//lprintf("End of sound\n");
				for(;index<len;index++)
					Chan->Waveform[index] = WAVE_SILENCE;
				
				Chan->sound_playing = 0;
				return;
			}
			else if (Chan->desiredfreq == 0x0000)
			{
				Chan->waveState = WAVE_IN;
			}
			else
			{  // compute change rate
				Chan->changerate = (halffreq / Chan->desiredfreq);
			}
			//lprintf("cursound = %d    sound_ptr = %d   desiredfreq = %d   changerate = %d\n", Chan->current_sound, Chan->sound_ptr, Chan->desiredfreq, Chan->changerate);
			
			Chan->sound_ptr++;
			if (firsttime)
				firsttime = 0;
			else
				Chan->sound_timer = SLOW_RATE;
		}
		
		if (Chan->sound_timer)
			Chan->sound_timer--;
		
		if (Chan->desiredfreq==0x0000)
		{    // silence
			Chan->Waveform[index] = WAVE_SILENCE;
		}
		else
		{
			// time to change waveform state?
			if (Chan->freqtimer > Chan->changerate)
			{  // toggle waveform, generating a square wave
				Chan->waveState =
					(Chan->waveState == WAVE_IN) ?
					WAVE_OUT : WAVE_IN;
				
				Chan->freqtimer = 0;
			}
			else
			{
				Chan->freqtimer++;
			}
			
			// put wave data into buffer
			if (index >= sizeof(Chan->Waveform))
			{
				lprintf("SoundDrv_GenWaveform: waveform buffer overflow\n");
				return;
			}
			else
			{
				Chan->Waveform[index] = Chan->waveState;
			}
		}
	}
//lprintf("reached len, exiting GenWaveform\n");
}

EXT_RAM_ATTR unsigned int MixedForm[5000];
EXT_RAM_ATTR unsigned char MixedFormFinal[5000];

void SoundDrv_Callback(void *unused, Uint8 *stream, int len)
{
	int    i, j, chan, numchannels;
	uchar temp;

        chan = 0;

        // mix the channels
        numchannels = 0;
        for(chan=0;chan<NUM_CHANNELS;chan++)
        {
           if (SndChannels[chan].sound_playing)
           {
              SoundDrv_GenWaveform(&SndChannels[chan], len);
              if (numchannels==0)
              {  // first channel. copy into mixed waveform
                 for(j=0;j<len;j++)
                 {
                    MixedForm[j] = SndChannels[chan].Waveform[j];
                 }
              }
              else
              {  // subsequent channel...merge with pre-existing channels
                 for(j=0;j<len;j++)
                 {
                    MixedForm[j] += SndChannels[chan].Waveform[j];
                 }
              }
              numchannels++;
           }
        }

        if (numchannels > 0)
        {
           // now average all the channels
		/*   if (!Menu_IsVisible())
		   {
	           for(i=0;i<len;i++)
	           {
	              MixedFormFinal[i] = (MixedForm[i] / numchannels);
	           }
		   }
		   else
		   {*/
		int reduce_amt = Menu_IsVisible() ? 2:1;
			// when menu is up, play sounds at 50% volume
	           for(i=0;i<len;i++)
	           {
					temp = (MixedForm[i] / numchannels);
					if (temp > 128)
					{
						temp -= 128;
						temp >>= reduce_amt;
						temp += 128;
					}
					else
					{
						temp = (128 - temp);
						temp >>= reduce_amt;
						temp = (128 - temp);
					}
					MixedFormFinal[i] = temp;
	           }
		   //}

           SDL_MixAudio(stream, MixedFormFinal, len, SDL_MIX_MAXVOLUME);
        }
}

// if priorities allow, plays the sound "snd".
// nonzero return value indicates a higher priority sound is playing.
void sound_play(int snd, char mode)
{
char chnl = 0;
int i;

	if (levelcontrol.gamemode==GM_DEMOPLAYBACK && Menu_IsVisible())
	{
		if (options[OPT_SOUNDOFFINDEMO])
		{
			sound_stop_all();
			return;
		}
	}
	if (gl_retval==QUITTING_PROGRAM)
	{
		sound_stop_all();
		return;
	}

  if (mode==PLAY_NORESTART)
  {
    if (sound_is_playing(snd))
      return;
  }

  // if a forced sound is playing then let it play
  if (forced_sound_is_playing()) return;

  // stop all other sounds if this sound has maximum priority
  if (sounds[snd].priority==255 || mode==PLAY_FORCE)
  {
    sound_stop_all();
  }

  if (snd==SOUND_KEEN_FALL)
  {  // only play KEEN_FALL if no other sounds are playing
    for(chnl=0;chnl<NUM_CHANNELS;chnl++)
    {
      if (SndChannels[chnl].sound_playing)
      {
        if (sounds[SndChannels[chnl].current_sound].priority > sounds[snd].priority)
        {
          return;
        }
      }
    }
    chnl = 0;
    goto playsound;
  }

  // first try to find an empty channel
  for(chnl=0;chnl<NUM_CHANNELS;chnl++)
  {
    if (!SndChannels[chnl].sound_playing)
    {
      goto startsound;
    }
  }
  // if all channels are full see if we have higher
  // priority than one of the sounds already playing.
  for(chnl=0;chnl<NUM_CHANNELS;chnl++)
  {
    if (sounds[SndChannels[chnl].current_sound].priority <= sounds[snd].priority)
    {
      goto startsound;
    }
  }
  // can't play sound right now.
  return;

startsound: ;
  // don't play more than once instance
  // of the same sound in a seperate channel--
  // instead restart the currently playing sound
  for(i=0;i<NUM_CHANNELS;i++)
  {
    if (SndChannels[i].current_sound==snd)
    {
      chnl = i;
      break;
    }
  }

playsound: ;
  // stop SOUND_KEEN_FALL if playing
  if (sound_is_playing(SOUND_KEEN_FALL))
    sound_stop(SOUND_KEEN_FALL);

  SndChannels[chnl].current_sound = snd;
  SndChannels[chnl].sound_ptr = 0;
  SndChannels[chnl].sound_timer = 0;
  SndChannels[chnl].sound_playing = 1;
  SndChannels[chnl].waveState = WAVE_IN;
  SndChannels[chnl].freqtimer = 0;
  SndChannels[chnl].sound_forced = 0;
  if (mode==PLAY_FORCE)
     SndChannels[chnl].sound_forced = 1;
}

