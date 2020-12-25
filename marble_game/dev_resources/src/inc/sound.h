#ifndef SOUND_H
#define SOUND_H

#include <SDL2/SDL_mixer.h>

typedef struct Sound {
	int musicstate;
	Mix_Music *music_menu;
	Mix_Music *music_level;
} Sound;

Sound *init_sound(char *filename);
void delete_sound(Sound *sound);
void change_music(Sound *sound);

#endif
