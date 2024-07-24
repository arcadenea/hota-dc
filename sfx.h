#include "kos.h"
#include <sys/cdefs.h>
#include <arch/types.h>
#include <dc/sound/sound.h>

sfxhnd_t snd_sfx_load_ex(uint16 *tmp, int len, int hz, int chn, int fmt);
int snd_sfx_play_chn_ex(int chn, sfxhnd_t idx, int vol, int pan);
void snd_sfx_stop_ex(int chn);
