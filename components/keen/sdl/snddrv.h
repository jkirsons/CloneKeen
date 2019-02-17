#include "esp_attr.h"
#define MAX_SOUND_LENGTH	1024
#define MAX_SOUNDS			50
#define SOUND_RATE			2
#define SOUND_FREQ_DIVISOR	1193180

#define WAVEFORM_VOLUME		16
#define WAVE_SILENCE		128
#define WAVE_IN				(WAVE_SILENCE-WAVEFORM_VOLUME)
#define WAVE_OUT			(WAVE_SILENCE+WAVEFORM_VOLUME)

// I am REALLY not sure why this is required or even if it applies to
// all systems. This is just a trial-an-error value which affects how
// quickly the sounds play. For some reason on my Linux box they play
// too slow, whereas 1010 sounds correct for all other OS's.
//
// If you have Linux and sounds play too quickly, try reverting the
// value back to 1010.
#ifdef __linux__
#define SLOW_RATE			600
#else
#define SLOW_RATE			1010
#endif

#define NUM_CHANNELS		4
typedef struct stSoundChannel
{
    char sound_playing;           // 1 = a sound is currently playing
    char current_sound;           // # of the sound that is currently playing
    int sound_ptr;                // position within sound that we're at
    unsigned int sound_timer;     // used to slow down the rate of playback
    int sound_paused;             // 1 = pause playback
    int sound_forced;

    unsigned int desiredfreq;     // current desired frequency in hz
    unsigned int changerate;      // frequency in samples (calculated)
    int freqtimer;                // time when to change waveform state
    int waveState;                // current value of the output waveform
    unsigned char Waveform[5000];//[65536]; // output waveform buffer
} stSoundChannel;
EXT_RAM_ATTR stSoundChannel SndChannels[NUM_CHANNELS];

SDL_AudioSpec AudioSpec;

typedef struct stSound
{
  unsigned char priority;
  int sounddata[MAX_SOUND_LENGTH];
} stSound;
EXT_RAM_ATTR stSound sounds[MAX_SOUNDS];
