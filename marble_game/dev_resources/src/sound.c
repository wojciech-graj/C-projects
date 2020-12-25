#include "inc/sound.h"
#include "sound_p.h"

Sound *init_sound(char *filename)
{
	Sound *sound = malloc(sizeof(Sound));
	*sound = (Sound) {
		.musicstate = -1,
		.music_menu = Mix_LoadMUS(filename),
		.music_level = NULL
	};
	assert(sound->music_menu);
	return sound;
}

void delete_sound(Sound *sound)
{
	Mix_FreeMusic(sound->music_menu);
	Mix_FreeMusic(sound->music_level);
	free(sound);
}

void change_music(Sound *sound)
{
	Mix_HaltMusic();
	switch(sound->musicstate)
	{
		case STATE_MENU:
		Mix_PlayMusic(context->sound->music_menu, -1);
		break;
		case STATE_GAME:
		Mix_PlayMusic(context->sound->music_level, -1);
		break;
	}
}
