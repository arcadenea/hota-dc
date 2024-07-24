#include "sfx.h"

#define AICA_SM_8BIT	1
#define AICA_SM_16BIT	0
#define AICA_SM_ADPCM	2

struct snd_effect;
LIST_HEAD(selist, snd_effect);

typedef struct snd_effect {
    uint32  locl, locr;
    uint32  len;
    uint32  rate;
    uint32  used;
    int stereo;
    uint32  fmt;

    LIST_ENTRY(snd_effect)  list;
} snd_effect_t;

struct selist snd_effects;

sfxhnd_t snd_sfx_load_ex(uint16 *tmp, int len, int hz, int chn, int fmt)
{

	snd_effect_t *t;
	uint16 *buftemp;

    t = malloc(sizeof(snd_effect_t));

    if(chn == 1) {

        /* Mono PCM/ADPCM */
        t->len = len/2; /* 16-bit samples */
        t->rate = hz;
        t->used = 1;
        t->locl = snd_mem_malloc(len);

        if(t->locl)
            spu_memload(t->locl, tmp, len);

        t->locr = 0;
        t->stereo = 0;

        if(fmt == 20) {
            t->fmt = AICA_SM_ADPCM;

            t->len *= 4;    /* 4-bit packed samples */
        }
        else
            t->fmt = AICA_SM_8BIT;

    }
    else if(chn == 2 && fmt == 1) {
        /* Stereo PCM */
        int i;
        uint16 * sepbuf;

        sepbuf = malloc(len / 2);

        for(i = 0; i < len / 2; i += 2) {
            sepbuf[i / 2] = tmp[i + 1];
        }

        for(i = 0; i < len / 2; i += 2) {
            tmp[i / 2] = tmp[i];
        }

        t->len = len / 4; /* Two stereo, 16-bit samples */
        t->rate = hz;
        t->used = 1;
        t->locl = snd_mem_malloc(len / 2);
        t->locr = snd_mem_malloc(len / 2);

        if(t->locl)
            spu_memload(t->locl, tmp, len / 2);

        if(t->locr)
            spu_memload(t->locr, sepbuf, len / 2);

        t->stereo = 1;
        t->fmt = AICA_SM_16BIT;

        free(sepbuf);
    }
    else if(chn == 2 && fmt == 20) {
        /* Stereo ADPCM */

        /* We have to be careful here, because the second sample might not
           start on a nice even dword boundary. We take the easy way out
           and just malloc a second buffer. */
        uint8 * buf2 = malloc(len / 2);
        memcpy(buf2, ((uint8*)tmp) + len / 2, len / 2);

        t->len = len;   /* Two stereo, 4-bit samples */
        t->rate = hz;
        t->used = 1;
        t->locl = snd_mem_malloc(len / 2);
        t->locr = snd_mem_malloc(len / 2);

        if(t->locl)
            spu_memload(t->locl, tmp, len / 2);

        if(t->locr)
            spu_memload(t->locr, buf2, len / 2);

        t->stereo = 1;
        t->fmt = AICA_SM_ADPCM;

        free(buf2);
    }
    else {
        free(t);
        t = NULL;
    }

    if(t) {
        LIST_INSERT_HEAD(&snd_effects, t, list);
    }

    return (sfxhnd_t)t;
}

