#include <ultra64.h>

#include "audio.h"

/* ----- externs ----- */
extern ALBankFile *g_al_bank_files[BANK_COUNT];
extern u8 g_mono;

extern u8 g_channel_index;
extern ALVoice g_voices[CHANNEL_COUNT];
extern Channel g_channels[CHANNEL_COUNT];
extern Channel *g_channel;
extern u8 *g_sequence_data_needle;

/* ----- .data ----- */
#include "includes/envelope_table.mnsg.h"

u8 g_vibrato_table[] =
{
    0x00, 0x20, 0x38, 0x50,
    0x68, 0x80, 0x90, 0xA0,
    0xB0, 0xC0, 0xD0, 0xE0,
    0xE8, 0xF0, 0xF8, 0xFC,
    0xFF, 0xFC, 0xF8, 0xF0,
    0xE8, 0xE0, 0xD0, 0xC0,
    0xB0, 0xA0, 0x90, 0x80,
    0x68, 0x50, 0x38, 0x20,
};

u8 g_random_table[] =
{
    0x9F, 0x3C, 0xB2, 0x52, 0xAF, 0x45, 0xC7, 0x89,
    0x10, 0x7F, 0xE0, 0x9D, 0xDC, 0x1F, 0x61, 0x16,
    0x39, 0xC9, 0x9C, 0xEB, 0x57, 0x08, 0x66, 0xF8,
    0x5A, 0x24, 0xBF, 0x0E, 0x3E, 0x15, 0x4B, 0xDB,
    0xAB, 0xF5, 0x31, 0x0C, 0x43, 0x02, 0x55, 0xDE,
    0x41, 0xDA, 0xBD, 0xAE, 0x19, 0xB0, 0x48, 0x57,
    0xBA, 0xA3, 0x36, 0x0B, 0xF9, 0xDF, 0x17, 0xA8,
    0x04, 0x0C, 0xE0, 0x91, 0x18, 0x5D, 0xDD, 0xD3,
    0x28, 0x8A, 0xF2, 0x11, 0x59, 0x6F, 0x06, 0x0A,
    0x34, 0x2A, 0x79, 0xAC, 0x5E, 0xA7, 0x83, 0xC6,
    0x39, 0xC1, 0xB4, 0x3A, 0x3F, 0xFE, 0x4F, 0xEF,
    0x1F, 0x00, 0x30, 0x99, 0x4C, 0x28, 0x83, 0xED,
    0x8A, 0x2F, 0x2C, 0x66, 0x3F, 0xD6, 0x6C, 0xB7,
    0x49, 0x22, 0xBC, 0x65, 0xFA, 0xCF, 0x02, 0xB1,
    0x46, 0xF0, 0x9A, 0xD7, 0xE2, 0x0F, 0x11, 0xC5,
    0x74, 0xF6, 0x7A, 0x2C, 0x8F, 0xFB, 0x19, 0x6A,
};

// TEMPORARY
u8 g_instrument_sounds_table[];
u8 g_sound_effect_sounds_table[];

Sound *g_sound_banks_table[BANK_COUNT] =
{
    g_instrument_sounds_table,
    g_sound_effect_sounds_table,
};

/*
Sound g_instrument_sounds_table[] =
{
    {0, 0, 0, 0, 0, 0, 0},
};
*/

#include "includes/instrument_sounds_table.mnsg.h"

#include "includes/sound_effect_sounds_table.mnsg.h"

Sound g_default_sound =
{
    0,
    0,
    127,
    0,
    0,
    31,
    64,
};

/*
DrumSound g_drum_sounds_table[] =
{
    {0, {0, 0, 0, 0, 0, 0, 0}},
};
*/

#include "includes/drum_sounds_table.mnsg.h"

void effects_set_tempo()
{
    g_channel->tempo = *g_sequence_data_needle << 8;
    g_sequence_data_needle++;

    g_channel->tempo_change_duration = 0;
}

void effects_set_tempo_change()
{
    s32 tempo_change_delta;

    g_channel->tempo_change_duration = *g_sequence_data_needle;
    g_sequence_data_needle++;

    g_channel->tempo_change_target = *g_sequence_data_needle;
    g_sequence_data_needle++;

    tempo_change_delta = (g_channel->tempo_change_target << 8) - g_channel->tempo;

    g_channel->tempo_change_amount = tempo_change_delta / g_channel->tempo_change_duration;
}

void effects_update_tempo_change()
{
    s32 new_tempo;

    g_channel->tempo_change_duration--;

    if (g_channel->tempo_change_duration != 0)
    {
        new_tempo = g_channel->tempo + g_channel->tempo_change_amount;

        if (new_tempo < 0x0100)
        {
            g_channel->tempo = new_tempo;
        }
        else if (new_tempo >= 0xFF00)
        {
            new_tempo = 0xFF00;
        }

        g_channel->tempo = new_tempo;
    }
    else
    {
        g_channel->tempo = g_channel->tempo_change_target << 8;
    }
}

/*
//TEMPORARY
typedef struct MNSGToGGAMapping 
{
    u8 mnsg_instrument_id;
    u8 gga_instrument_id;
} MNSGToGGAMapping;

MNSGToGGAMapping mnsg_to_gga_mappings[] =
{
    {0x27, 0x24}, // Woodblock
    {0x28, 0x27}, // fix this it sounds weird
    {0x33, 0x2F}, // Orchestra Hit
    {0x34, 0x30}, // Taiko
    {0x38, 0x32}, // Fingered Bass Low
    {0x39, 0x33}, // Fingered Bass High
    //{0x41, 0x38}, // Synth Organ
    {0x42, 0x6B}, // Muited Guitar Low
    {0x43, 0x6B}, // Muited Guitar High
    {0x45, 0x41}, // Koto 1
    {0x46, 0x42}, // Koto 2
    {0x47, 0x43}, // Koto 3
    {0x48, 0x44}, // Koto 4
    {0x49, 0x46}, // Shamisen 1
    {0x4A, 0x47}, // Shamisen 2
    {0x4B, 0x48}, // Shamisen 3
    {0x4C, 0x49}, // Shamisen 4
    {0x4D, 0x00}, // Key Synth 
    {0x50, 0x93}, // Some Mallet Instrument
    {0x51, 0x3C}, // Marimba 1 (doesnt exist) (needs special transpose maybe)
    {0x52, 0x3C}, // Marimba 2
    {0x53, 0x3D}, // Marimba 3
    {0x56, 0x4F}, // Strings 1
    {0x57, 0x51}, // Strings 2
    {0x58, 0x52}, // Strings 2
    {0x59, 0x54}, // Strings 3
    {0x5A, 0x55}, // Strings 3
    {0x62, 0x4A}, // Brass 1
    {0x63, 0x4B}, // Brass 2
    {0x64, 0x4C}, // Brass 3
    {0x65, 0x56}, // Byowww Synth 1 (needs special transpose maybe)
    {0x66, 0x56}, // Byowww Synth 2 (needs special transpose maybe)
    {0x6A, 0x5F}, // Flute 1 (wrong sample..)
    {0x6B, 0x5F}, // Flute 2
    {0x6C, 0x60}, // Flute 3
    {0x77, 0x40}, // Dyoooooow Synth
    // {0x78, 0x84}, // Crystallike (missing?)
    {0x7E, 0x5A}, // Shakuhachi
    {0x7F, 0x68}, // E-Guitar 1 (missing) (needs special transpose maybe)
    {0x80, 0x68}, // E-Guitar 2
    {0x81, 0x6A}, // E-Guitar 2
    {0x82, 0x67}, // E-Guitar Strum 1
    {0x83, 0x69}, // E-Guitar Strum 2
    {0x84, 0x6C}, // E-Guitar BEE!
    //{0x85, 0x6C}, // E-Guitar BRUUUUV! (wrong sample doesn't exist)
};
*/

void effects_set_sound()
{
    Sound *sound;

    // Something to do with panning here?

    g_channel->sound = *g_sequence_data_needle;
    g_sequence_data_needle++;

/*
    osSyncPrintf("Before Sound: %X\n", g_channel->sound);

    // Special Case Use Bank 4
    if (g_channel->sound == 0x85 || g_channel->sound == 0x78 || g_channel->sound == 0x41 || g_channel->sound == 0x2A)
    {
        osSyncPrintf("Special Case: Using Bank 4\n");
        g_channel->bank = 3;
    }


    //TEMPORARY: Find the correct instrument ID
    for (u8 i = 0; i < sizeof(mnsg_to_gga_mappings) / sizeof(MNSGToGGAMapping); i++)
    {
        if (mnsg_to_gga_mappings[i].mnsg_instrument_id == g_channel->sound)
        {
            if (g_channel->sound == 0x51)
            {
                if (g_channel->effect_transpose != 0)
                {
                    //osSyncPrintf("Warning: Effect Transpose is already set.\n");
                }

                //g_channel->effect_transpose = (-11 << 8); Wrong?
                //osSyncPrintf("Transposing Marimba 1\n");
            }
            else if (g_channel->sound == 0x42)
            {
                if (g_channel->effect_transpose != 0)
                {
                    //osSyncPrintf("Warning: Effect Transpose is already set.\n");
                }

                g_channel->effect_transpose = (12 << 8);
                //osSyncPrintf("Transposing Muited Guitar\n");
            }
            else if (g_channel->sound == 0x43)
            {
                if (g_channel->effect_transpose != 0)
                {
                    //osSyncPrintf("Warning: Effect Transpose is already set.\n");
                }

                g_channel->effect_transpose = (12 << 8);
                //osSyncPrintf("Transposing Muited Guitar\n");
            }
            else
            {
                g_channel->effect_transpose = 0;
            }

            g_channel->sound = mnsg_to_gga_mappings[i].gga_instrument_id;
            break;
        }
    }

    osSyncPrintf("After Sound: %X\n", g_channel->sound);
*/

    sound = &g_default_sound;

    if (g_channel->bank < BANK_COUNT)
    {
        if (g_sound_banks_table[g_channel->bank] != NULL)
        {
            sound = g_sound_banks_table[g_channel->bank] + g_channel->sound;
        }
    }

    effects_load_sound(sound);

    // Special Case Use Bank 4
    if (g_channel->sound == 0x85 || g_channel->sound == 0x78 || g_channel->sound == 0x41 || g_channel->sound == 0x2A)
    {
        g_channel->bank = 0;
    }

}

void effects_load_sound(Sound *sound)
{
    g_channel->wave_table = g_al_bank_files[g_channel->bank]->bankArray[0]->instArray[g_channel->sound]->soundArray[0]->wavetable;

    g_channel->sound_transpose_detune = sound->transpose << 8;
    g_channel->sound_transpose_detune += sound->detune << 2;

    g_channel->envelope_attack_duration = g_envelope_table[0x7F - sound->attack];

    g_channel->envelope_decay_duration = g_envelope_table[0x3D - ((sound->decay & 0xF0) >> 2)];
    g_channel->envelope_decay_volume = (sound->decay & 0x0F) << 2;

    if (sound->sustain == 0)
    {
        g_channel->envelope_sustain_duration = 0;
    }
    else
    {
        g_channel->envelope_sustain_duration = g_envelope_table[0x7F - sound->sustain] + 1;
    }

    g_channel->envelope_release_duration = g_envelope_table[0x7D - (sound->release << 2)];

    if (g_channel->pan < 0x8000)
    {
        g_channel->pan = sound->pan << 8;
        g_channel->pan_change_duration = 0;
    }

    effects_set_pan_();
}

void effects_load_drum_sound()
{
    g_channel->sound = ((DrumSound *)g_drum_sounds_table)[g_channel->note_pitch - 0x48].id;

    effects_load_sound(&((DrumSound *)g_drum_sounds_table)[g_channel->note_pitch - 0x48].sound);
}

void effects_set_volume_sound()
{
    effects_set_volume();
    effects_set_sound();
}

void effects_set_volume_sound_pan()
{
    effects_set_volume();
    effects_set_sound();
    effects_set_pan();
}

void effects_set_volume()
{
    u8 volume;

    volume = *g_sequence_data_needle;
    g_sequence_data_needle++;

    if ((g_channel->sequence_id >> 24) == 0)
    {
        g_channel->volume = volume << 8;
    }
    else
    {
        g_channel->volume = volume * (g_channel->sequence_id >> 24);
    }

    g_channel->volume_change_duration = 0;
}

void effects_set_volume_change()
{
    s32 volume_change_delta;

    g_channel->volume_change_duration = *g_sequence_data_needle;
    g_sequence_data_needle++;

    g_channel->volume_change_target = *g_sequence_data_needle;
    g_sequence_data_needle++;

    volume_change_delta = (g_channel->volume_change_target << 8) - g_channel->volume;

    g_channel->volume_change_amount = volume_change_delta / g_channel->volume_change_duration;
}

void effects_update_volume_change()
{
    s32 new_volume;

    g_channel->flags |= VOLUME_IS_CHANGING;

    g_channel->volume_change_duration--;
    if (g_channel->volume_change_duration != 0)
    {
        new_volume = g_channel->volume + g_channel->volume_change_amount;

        if (new_volume < 0x0000)
        {
            g_channel->volume = new_volume;
        }
        else if (new_volume > 0xFF00)
        {
            new_volume = 0xFF00;
        }

        g_channel->volume = new_volume;
    }
    else
    {
        g_channel->volume = g_channel->volume_change_target << 8;
    }
}

void effects_set_attack_decay()
{
    g_channel->envelope_attack_duration = g_envelope_table[0x7F - *g_sequence_data_needle];
    g_sequence_data_needle++;

    g_channel->envelope_decay_duration = g_envelope_table[0x3D - ((*g_sequence_data_needle & 0xF0) >> 2)];
    g_channel->envelope_decay_volume = (*g_sequence_data_needle & 0x0F) << 2;
    g_sequence_data_needle++;
}

void effects_set_sustain()
{
    u8 sustain;

    sustain = *g_sequence_data_needle;
    g_sequence_data_needle++;

    if (sustain == 0)
    {
        g_channel->envelope_sustain_duration = 0;

        if ((g_channel->note_hold_ratio == 0) && (g_channel->envelope_phase == SUSTAIN))
        {
            g_channel->envelope_phase_duration = 0x7FFF;
        }
    }
    else
    {
        g_channel->envelope_sustain_duration = g_envelope_table[0x7F - sustain] + 1;

        if ((g_channel->note_hold_ratio == 0) && (g_channel->envelope_phase == SUSTAIN))
        {
            g_channel->envelope_phase_duration = g_channel->envelope_sustain_duration;
        }
    }
}

void effects_set_release()
{
    g_channel->envelope_release_duration = g_envelope_table[0x7D - (*g_sequence_data_needle << 2)];
    g_sequence_data_needle++;

    if ((g_channel->note_hold_ratio == 0) && (g_channel->envelope_phase == RELEASE))
    {
        g_channel->envelope_phase_duration = g_channel->envelope_release_duration;
    }
}

//TEMPROARY
extern s32 g_custom_parameters[];
extern ALFxRef g_al_fx_ref;

void effects_set_reverb()
{
    u8 parameter_1;
    u8 parameter_2;
    s32 parameters[2];

    if (g_channel->dry_wet_mix != 0)
    {
        alSynSetFXMix(&alGlobals->drvr, &g_voices[g_channel_index], 0);
    }

    parameter_1 = *g_sequence_data_needle;
    g_sequence_data_needle++;

    parameters[0] = parameter_1 * 40;
    if (parameters[0] == 0)
    {
        parameters[0] = g_custom_parameters[3];
    }

    alSynSetFXParam(&alGlobals->drvr, g_al_fx_ref, 3, parameters);

    parameter_2 = *g_sequence_data_needle;
    g_sequence_data_needle++;

    parameters[0] = parameter_2 << 7;
    if (parameters[0] == 0)
    {
        parameters[0] = g_custom_parameters[4];
    }

    alSynSetFXParam(&alGlobals->drvr, g_al_fx_ref, 4, parameters);

    if (g_channel->dry_wet_mix != 0)
    {
        alSynSetFXMix(&alGlobals->drvr, &g_voices[g_channel_index], g_channel->dry_wet_mix);
    }
}

void effects_set_pan()
{
    g_channel->pan_change_duration = 0;

    g_channel->pan = *g_sequence_data_needle << 8;
    g_sequence_data_needle++;

    effects_set_pan_();
}

void effects_set_pan_()
{
    ALPan pan;

    if (g_mono == 0)
    {
        pan = (g_channel->pan >> 8) & 0x7F;
    }
    else
    {
        pan = 0x40;
    }

    alSynSetPan(&alGlobals->drvr, &g_voices[g_channel_index], pan);
}

void effects_set_pan_change()
{
    s32 pan_change_delta;

    g_channel->pan_change_duration = *g_sequence_data_needle;
    g_sequence_data_needle++;

    g_channel->pan_change_target = *g_sequence_data_needle;
    g_sequence_data_needle++;

    if ((g_channel->pan > 0x7FFF) || ((g_channel->flags & IS_DRUM) == 0))
    {
        pan_change_delta = (g_channel->pan_change_target << 8) - (g_channel->pan & 0x7FFF);

        g_channel->pan_change_amount = pan_change_delta / g_channel->pan_change_duration;
    }
}

void effects_update_pan_change()
{
    s32 new_pan;
    u16 old_pan;

    g_channel->flags |= VOLUME_IS_CHANGING;

    old_pan = g_channel->pan;

    g_channel->pan_change_duration--;
    if (g_channel->pan_change_duration != 0)
    {
        new_pan = (g_channel->pan & 0x7FFF) + g_channel->pan_change_amount;

        if (new_pan < 0x0000)
        {
            new_pan = 0;
        }
        else if (new_pan > 0x7FFF)
        {
            new_pan = 0x7F00;
        }

        g_channel->pan = new_pan;
    }
    else
    {
        g_channel->pan = g_channel->pan_change_target << 8;
    }

    g_channel->pan |= old_pan & 0x8000;

    effects_set_pan_();
}

void effects_set_transpose()
{
    g_channel->effect_transpose = *g_sequence_data_needle << 8;
    g_sequence_data_needle++;
}

void effects_set_detune()
{
    g_channel->effect_detune = *g_sequence_data_needle << 2;
    g_sequence_data_needle++;
}

void effects_set_vibrato()
{
    u8 delay;
    u8 rate;

    delay = *g_sequence_data_needle;
    g_sequence_data_needle++;

    if (delay < 100)
    {
        g_channel->vibrato_intended_delay_duration = delay;
    }
    else
    {
        g_channel->vibrato_intended_delay_duration = 0;
        g_channel->vibrato_intended_start_duration = delay - 100;
    }

    rate = *g_sequence_data_needle;
    g_sequence_data_needle++;

    if (rate < 0x20)
    {
        rate = ((rate & 0x1F) << 3);
        g_channel->vibrato_oscillation_step_size = 1;
    }
    else if (rate < 0x40)
    {
        rate = ((rate & 0x3F) << 2);
        g_channel->vibrato_oscillation_step_size = 2;
    }
    else if (rate < 0x80)
    {
        rate = ((rate & 0x7F) << 1);
        g_channel->vibrato_oscillation_step_size = 4;
    }
    else if (rate < 0xFF)
    {
        g_channel->vibrato_oscillation_step_size = 8;
    }
    else if (rate == 0xFF)
    {
        g_channel->vibrato_oscillation_step_size = 16;
    }

    g_channel->vibrato_rate = rate;

    g_channel->vibrato_depth_target = *g_sequence_data_needle << 8;
    g_sequence_data_needle++;

    if (g_channel->vibrato_intended_start_duration != 0)
    {
        g_channel->vibrato_depth_change_amount = g_channel->vibrato_depth_target / g_channel->vibrato_intended_start_duration;
    }
}

void effects_initialize_vibrato()
{
    g_channel->vibrato_delay_duration = g_channel->vibrato_intended_delay_duration;
    g_channel->vibrato_start_duration = g_channel->vibrato_intended_start_duration;

    if (g_channel->vibrato_intended_start_duration == 0)
    {
        g_channel->vibrato_depth = g_channel->vibrato_depth_target;
    }
    else
    {
        g_channel->vibrato_depth = 0;
    }

    g_channel->vibrato_counter = 0;
    g_channel->vibrato_oscillation = g_channel->vibrato_counter;
}

void effects_update_vibrato_on_tick()
{
    if (g_channel->vibrato_delay_duration == 0)
    {
        if (g_channel->vibrato_start_duration != 0)
        {
            g_channel->vibrato_start_duration--;

            if (g_channel->vibrato_start_duration == 0)
            {
                g_channel->vibrato_depth = g_channel->vibrato_depth_target;
            }
            else
            {
                g_channel->vibrato_depth += g_channel->vibrato_depth_change_amount;
            }
        }

        effects_update_vibrato();
    }
    else
    {
        g_channel->vibrato_delay_duration--;
    }
}

void effects_update_vibrato()
{
    if (g_channel->vibrato_delay_duration == 0)
    {
        g_channel->vibrato_counter += g_channel->vibrato_rate;

        if (g_channel->vibrato_counter >= 0x100)
        {
            g_channel->vibrato_counter &= 0xFF;

            g_channel->flags |= PITCH_IS_CHANGING;

            g_channel->vibrato_oscillation += g_channel->vibrato_oscillation_step_size;
            g_channel->vibrato_oscillation &= 0x3F;

            if (g_channel->vibrato_depth < 0x8000)
            {
                g_channel->vibrato_pitch = g_channel->vibrato_depth >> 7;
                g_channel->vibrato_pitch = (g_vibrato_table[g_channel->vibrato_oscillation & 0x1F] * g_channel->vibrato_pitch) >> 8;
            }
            else
            {
                g_channel->vibrato_pitch = (g_channel->vibrato_depth >> 8) & 0x7F;
                g_channel->vibrato_pitch = g_channel->vibrato_pitch + 2;
                g_channel->vibrato_pitch = (g_vibrato_table[g_channel->vibrato_oscillation & 0x1F] * g_channel->vibrato_pitch) >> 1;
            }

            if (g_channel->vibrato_oscillation >= 0x20)
            {
                g_channel->vibrato_pitch = -g_channel->vibrato_pitch;
            }
        }
    }
}

void effects_set_vibrato_change()
{
    g_channel->vibrato_intended_start_duration = *g_sequence_data_needle;
    g_sequence_data_needle++;

    if (g_channel->vibrato_intended_start_duration != 0)
    {
        g_channel->vibrato_depth_change_amount = g_channel->vibrato_depth_target / g_channel->vibrato_intended_start_duration;
    }

}

void effects_set_random()
{
    g_channel->random_rate = *g_sequence_data_needle;
    g_sequence_data_needle++;

    g_channel->random_pitch_mask = *g_sequence_data_needle << 8;
    g_sequence_data_needle++;

    g_channel->random_pitch_mask += *g_sequence_data_needle;
    g_sequence_data_needle++;

    g_channel->random_index = 0;
    g_channel->random_counter = g_channel->random_index;

    if (g_channel->random_rate == 0)
    {
        g_channel->random_pitch = 0;
    }
}

void effects_update_random()
{
    g_channel->random_counter += g_channel->random_rate;

    if (g_channel->random_counter >= 0x100)
    {
        g_channel->random_counter &= 0xFF;

        g_channel->flags |= PITCH_IS_CHANGING;

        g_channel->random_pitch = g_random_table[g_channel->random_index];

        g_channel->random_index++;
        g_channel->random_index &= 0x7F;

        g_channel->random_pitch += g_random_table[g_channel->random_index] << 8;
        g_channel->random_pitch &= g_channel->random_pitch_mask;
    }
}

void effects_set_pitch_bend_dummy()
{
    g_sequence_data_needle += 3;
}

void effects_calculate_pitch_change_amount()
{
    s32 pitch_change_delta;
    s32 pitch_change_duration;

    pitch_change_delta = g_channel->pitch_change_target - g_channel->pitch;
    pitch_change_duration = ((((g_channel->pitch_change_duration << 16) / g_channel->tempo) & 0xFFFF) + 1) & 0xFFFF;

    g_channel->pitch_change_amount = pitch_change_delta / pitch_change_duration;

    if (g_channel->pitch_change_amount == 0)
    {
        if (pitch_change_delta < 0)
        {
            g_channel->pitch_change_amount = -1;
        }
        else
        {
            g_channel->pitch_change_amount = 1;
        }
    }
}

void effects_set_pitch_bend()
{
    u8 *next_sequence_data_needle;
    u8 pitch_target;

    next_sequence_data_needle = g_sequence_data_needle + 1;

    if (*g_sequence_data_needle == 0xE4)
    {
        if (g_channel->pitch_change_next_note_parameter == 0)
        {
            g_sequence_data_needle++;

            g_channel->pitch_change_type = FIXED_LENGTH;

            g_channel->pitch_change_delay_duration = *g_sequence_data_needle;
            g_sequence_data_needle++;

            g_channel->pitch_change_duration = *g_sequence_data_needle;
            g_sequence_data_needle++;

            pitch_target = *g_sequence_data_needle;
            g_sequence_data_needle++;

            g_channel->pitch_change_target = g_channel->pitch + ((pitch_target - g_channel->note_pitch) << 8);

            effects_calculate_pitch_change_amount();
        }
        else
        {
            g_sequence_data_needle += 4;
        }

        g_channel->sequence_data_needle = g_sequence_data_needle;
        next_sequence_data_needle = g_sequence_data_needle;
    }

    g_sequence_data_needle = next_sequence_data_needle;
}

void effects_update_pitch_change_on_tick()
{
    if (g_channel->pitch_change_type == FIXED_LENGTH)
    {
        if (g_channel->pitch_change_delay_duration == 0)
        {
            g_channel->flags |= PITCH_IS_CHANGING;

            g_channel->pitch_change_duration--;
            if (g_channel->pitch_change_duration == 0)
            {
                g_channel->pitch = g_channel->pitch_change_target;
            }
            else
            {
                g_channel->pitch += g_channel->pitch_change_amount;
            }
        }
        else
        {
            g_channel->pitch_change_delay_duration--;
        }
    }
    else
    {
        effects_update_portamento();
    }
}

void effects_update_pitch_change_off_tick()
{
    if (g_channel->pitch_change_type == FIXED_LENGTH)
    {
        if (g_channel->pitch_change_delay_duration == 0)
        {
            g_channel->flags |= PITCH_IS_CHANGING;

            g_channel->pitch += g_channel->pitch_change_amount;
        }
    }
    else
    {
        effects_update_portamento();
    }
}

void effects_set_pitch_bend_start()
{
    g_channel->pitch_change_type = FIXED_LENGTH;

    g_channel->pitch_change_next_note_delay_duration = *g_sequence_data_needle;
    g_sequence_data_needle++;

    g_channel->pitch_change_next_note_duration = *g_sequence_data_needle;
    g_sequence_data_needle++;

    g_channel->pitch_change_next_note_parameter = (*g_sequence_data_needle << 8);
    g_sequence_data_needle++;
}

void effects_calculate_pitch_change_amount_this_note()
{
    if (g_channel->pitch_change_type == FIXED_LENGTH)
    {
        g_channel->pitch_change_delay_duration = g_channel->pitch_change_next_note_delay_duration;
        g_channel->pitch_change_duration = g_channel->pitch_change_next_note_duration;
        g_channel->pitch_change_target = g_channel->pitch_change_next_note_parameter;
        g_channel->pitch -= g_channel->pitch_change_next_note_parameter;
        effects_calculate_pitch_change_amount();
    }
    else
    {
        g_channel->pitch_change_target = g_channel->pitch;
        g_channel->pitch = g_channel->pitch_change_amount;
        g_channel->pitch_change_duration = 1;
    }
}

void effects_set_portamento()
{
    g_channel->pitch_change_type = VARIABLE_LENGTH;

    g_channel->pitch_change_next_note_parameter = *g_sequence_data_needle;
    g_sequence_data_needle++;
}

void effects_update_portamento()
{
    s32 pitch_delta;
    s32 pitch_delta_absolute;
    s32 pitch_change_amount;

    g_channel->flags |= PITCH_IS_CHANGING;

    pitch_delta = g_channel->pitch_change_target - g_channel->pitch;
    pitch_delta_absolute = pitch_delta;
    if (pitch_delta < 0)
    {
        pitch_delta_absolute = -pitch_delta;
    }

    pitch_change_amount = (g_channel->pitch_change_next_note_parameter * pitch_delta_absolute) >> 8;
    if (pitch_change_amount == 0)
    {
        pitch_change_amount = 1;
    }

    if (pitch_delta < 0)
    {
        g_channel->pitch = g_channel->pitch - pitch_change_amount;

        if (g_channel->pitch < g_channel->pitch_change_target)
        {
            g_channel->pitch = g_channel->pitch_change_target;
        }
    }
    else
    {
        g_channel->pitch = g_channel->pitch + pitch_change_amount;

        if (g_channel->pitch > g_channel->pitch_change_target)
        {
            g_channel->pitch = g_channel->pitch_change_target;
        }
    }

    if (g_channel->pitch == g_channel->pitch_change_target)
    {
        g_channel->pitch_change_duration = 0;
    }
}

void effects_set_marker()
{
    g_channel->marker_loop_counter = 0;
    g_channel->marker_needle = g_sequence_data_needle;
    g_channel->marker_volume = 0;
    g_channel->marker_transpose = g_channel->marker_volume;
}

void effects_go_to_marker()
{
    u8 count;

    count = *g_sequence_data_needle;
    g_sequence_data_needle++;

    g_channel->marker_loop_counter++;

    if ((count != 0) && (g_channel->marker_loop_counter == count))
    {
        g_channel->marker_volume = 0;
        g_channel->marker_transpose = g_channel->marker_volume;

        g_sequence_data_needle += 2;
    }
    else
    {
        g_channel->marker_volume += *g_sequence_data_needle;
        g_sequence_data_needle++;

        g_channel->marker_transpose += (*g_sequence_data_needle << 3);
        g_sequence_data_needle++;

        g_sequence_data_needle = g_channel->marker_needle;
    }
}

void effects_set_nested_marker()
{
    g_channel->nested_marker_loop_counter = 0;
    g_channel->nested_marker_needle = g_sequence_data_needle;
    g_channel->nested_marker_volume = 0;
    g_channel->nested_marker_transpose = g_channel->nested_marker_volume;
}

void effects_go_to_nested_marker()
{
    u8 count;

    count = *g_sequence_data_needle;
    g_sequence_data_needle++;

    g_channel->nested_marker_loop_counter++;

    if ((count != 0) && (g_channel->nested_marker_loop_counter == count))
    {
        g_channel->nested_marker_volume = 0;
        g_channel->nested_marker_transpose = g_channel->nested_marker_volume;

        g_sequence_data_needle += 2;
    }
    else
    {
        g_channel->nested_marker_volume += *g_sequence_data_needle;
        g_sequence_data_needle++;

        g_channel->nested_marker_transpose += (*g_sequence_data_needle << 3);
        g_sequence_data_needle++;

        g_sequence_data_needle = g_channel->nested_marker_needle;
    }
}

void effects_set_master_marker()
{
    g_channel->master_marker_needle = g_sequence_data_needle;
}

void effects_go_to_master_marker()
{
    g_sequence_data_needle = g_channel->master_marker_needle;
}

void effects_set_conditional_marker()
{
    // TODO
}

void effects_go_to_conditional_marker()
{
    // TODO
}

void effects_set_echo()
{
    u8 dry_wet_mix;

    dry_wet_mix = *g_sequence_data_needle;
    g_sequence_data_needle++;

    if (g_channel->dry_wet_mix != dry_wet_mix)
    {
        g_channel->dry_wet_mix = dry_wet_mix;
        alSynSetFXMix(&alGlobals->drvr, &g_voices[g_channel_index], dry_wet_mix);
    }
}

void effects_set_bank()
{
    g_channel->bank = *g_sequence_data_needle;
    g_sequence_data_needle++;
}

void effects_set_nothing()
{
#define DO_VARIATIONS

#ifdef DO_VARIATIONS
    g_sequence_data_needle++;
#endif
}

void effects_rest()
{
    alSynSetVol(&alGlobals->drvr, &g_voices[g_channel_index], 0, CALLBACK_TIME);

    g_channel->voice_status = 1;

    g_channel->note_duration = *g_sequence_data_needle;
    g_sequence_data_needle++;

    g_channel->note_hold_ratio = 1;

    g_channel->sequence_data_needle = g_sequence_data_needle;
}

void effects_tie()
{
    g_channel->note_duration = *g_sequence_data_needle;
    g_sequence_data_needle++;

    g_channel->note_hold_ratio = *g_sequence_data_needle;
    g_sequence_data_needle++;

    if (g_channel->note_hold_ratio == 0)
    {
        g_channel->note_hold_duration = 0;
    }
    else
    {
        g_channel->note_hold_duration = (g_channel->note_duration * g_channel->note_hold_ratio) / 128;

        if (g_channel->note_hold_duration == 0)
        {
            g_channel->note_hold_duration = 1;
        }
    }

    g_channel->sequence_data_needle = g_sequence_data_needle;

    if (g_channel->envelope_phase_duration == 0)
    {
        player_set_volume(); // is actually player_fdsdfsfsdf
    }
    else
    {
        player_update_envelope_phase();

        if (g_channel->envelope_phase_duration != 0)
        {
            player_set_volume();
        }
    }
}

void effects_end()
{
    alSynSetVol(&alGlobals->drvr, &g_voices[g_channel_index], 0, CALLBACK_TIME);

    g_channel->voice_status = 1;
    g_channel->sequence_id = 0;
    g_channel->priority = g_channel->sequence_id;
}
