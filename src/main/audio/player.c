#include "audio.h"

#include <ultra64.h>

/* ----- externs ----- */
extern u8 g_paused;
extern u8 g_tempo_increase_amount;
extern u8 g_fade_amount;

/* ----- .data ----- */
void (*g_command_table[])() =
{
    effects_set_tempo, // 0xD0
    effects_set_tempo_change, // 0xD1
    effects_set_sound, // 0xD2
    effects_set_volume_sound, // 0xD3
    effects_set_volume_sound_pan, // 0xD4
    effects_set_volume, // 0xD5
    effects_set_volume_change, // 0xD6
    effects_set_attack_decay, // 0xD7
    effects_set_sustain, // 0xD8
    effects_set_release, // 0xD9
    effects_set_reverb, // 0xDA
    effects_end, // 0xDB
    effects_end, // 0xDC
    effects_set_pan, // 0xDD
    effects_set_pan_change, // 0xDE
    effects_set_transpose, // 0xDF
    effects_set_detune, // 0xE0
    effects_set_vibrato, // 0xE1
    effects_set_vibrato_change, // 0xE2
    effects_set_random, // 0xE3
    effects_set_pitch_bend_dummy, // 0xE4
    effects_set_pitch_bend_start, // 0xE5
    effects_set_portamento, // 0xE6
    effects_set_marker, // 0xE7
    effects_go_to_marker, // 0xE8
    effects_set_nested_marker, // 0xE9
    effects_go_to_nested_marker, // 0xEA
    effects_set_master_marker, // 0xEB
    effects_go_to_master_marker, // 0xEC
    effects_set_conditional_marker, // 0xED
    effects_go_to_conditional_marker, // 0xEE
    effects_set_echo, // 0xEF
    effects_set_bank, // 0xF0
    effects_set_nothing, // 0xF1
    effects_rest, // 0xF2
    effects_tie, // 0xF3
    effects_end, // 0xF4
    effects_end, // 0xF5
    effects_end, // 0xF6
    effects_end, // 0xF7
    effects_end, // 0xF8
    effects_end, // 0xF9
    effects_end, // 0xFA
    effects_end, // 0xFB
    effects_end, // 0xFC
    effects_end, // 0xFD
    effects_end, // 0xFE
    effects_end, // 0xFF
};

f32 g_pitch_table[] = 
{
    0.03125,
    0.03310822,
    0.035076939,
    0.037162717,
    0.03937253,
    0.041713718,
    0.044194154,
    0.046822064,
    0.049606249,
    0.052555967,
    0.055681124,
    0.058992095,
    0.0625,
    0.066216439,
    0.070153877,
    0.074325435,
    0.07874506,
    0.083427437,
    0.088388309,
    0.093644127,
    0.099212497,
    0.10511193,
    0.11136225,
    0.11798419,
    0.125,
    0.13243288,
    0.14030775,
    0.14865087,
    0.15749012,
    0.16685487,
    0.17677662,
    0.18728825,
    0.19842499,
    0.21022387,
    0.2227245,
    0.23596838,
    0.25,
    0.26486576,
    0.2806155,
    0.29730174,
    0.31498024,
    0.33370975,
    0.35355324,
    0.3745765,
    0.39684999,
    0.42044774,
    0.44544899,
    0.47193676,
    0.5,
    0.5297315,
    0.561231,
    0.59460348,
    0.62996048,
    0.66741949,
    0.70710647,
    0.749153,
    0.79369998,
    0.84089547,
    0.89089799,
    0.9438735, 
    1.0,
    1.059463,
    1.122462,
    1.189207,
    1.259921,
    1.334839,
    1.4142129,
    1.498306,
    1.5874,
    1.6817909,
    1.781796,
    1.887747,
    2.0,
    0.0,
};

/* ----- .bss ----- */
u8 g_channel_index;

ALVoice g_voices[CHANNEL_COUNT];

Channel g_channels[CHANNEL_COUNT];
Channel *g_channel;

u8 *g_sequence_data_needle;

void player_update_channels()
{
    // A bunch of modifier stuff... (Tempo/Fade/Volume)

    for (g_channel_index = 0; g_channel_index < CHANNEL_COUNT; g_channel_index++)
    {
        g_channel = &g_channels[g_channel_index];

        if (g_channels[g_channel_index].voice_status != 0)
        {
            if (--g_channels[g_channel_index].voice_status == 0)
            {
                alSynStopVoice(&alGlobals->drvr, &g_voices[g_channel_index]);
            }
        }

        if (g_channel->sequence_id != 0)
        {
            if (g_channel->initial_delay_counter == 0)
            {
                if (g_channel->sequence_id < 0x100)
                {
                    if (!g_paused)
                    {
                        g_channel->tempo += g_tempo_increase_amount;
                        player_update_channel();
                    }
                }
                else
                {
                    player_update_channel();
                }
            }
            else
            {
                g_channel->initial_delay_counter--;
            }
        }
    }
}

void player_update_channel()
{
    g_channel->tempo_counter = g_channel->tempo_counter + (g_channel->tempo >> 8);

    if (g_channel->tempo_counter < 0x100)
    {
        player_update_off_tick();
    }
    else
    {
        g_channel->tempo_counter &= 0xFF;
        g_channel->note_duration--;

        if (g_channel->note_duration == 0)
        {
            player_read_data();
            effects_set_pitch_bend();
        }
        else
        {
            player_update_on_tick();
        }

        if (g_channel->tempo_change_duration != 0)
        {
            effects_update_tempo_change();
        }

        if (g_channel->volume_change_duration != 0)
        {
            effects_update_volume_change();
        }

        if (g_channel->pan_change_duration != 0)
        {
            effects_update_pan_change();
        }
    }

    if (g_channel->random_rate != 0)
    {
        effects_update_random();
    }
}

void player_read_data()
{
    u8 note_is_still_playing = FALSE;
    u8 sequence_data;
    u32 command;

    if (g_channel->note_hold_ratio == 0)
    {
        note_is_still_playing = TRUE;
    }
    else if (g_channel->voice_status != 0)
    {
        g_channel->voice_status = 0;

        alSynStopVoice(&alGlobals->drvr, &g_voices[g_channel_index]);
    }

    g_sequence_data_needle = g_channel->sequence_data_needle;
    sequence_data = *g_sequence_data_needle;

    while (TRUE)
    {
        command = sequence_data;
        g_sequence_data_needle++;

        if (command < 0xD0)
        {
            // Set the previous pitch for the portamento effect.
            g_channel->pitch_change_amount = g_channel->pitch;

            if (command < 0x68)
            {
                g_channel->note_pitch = sequence_data;
            }
            else
            {
                g_channel->note_pitch = sequence_data - 0x68;
            }

            if (g_channel->note_pitch < 0x48)
            {
                g_channel->flags &= ~IS_DRUM;
            }
            else
            {
                if (g_channel->note_pitch == 0x67)
                {
                    g_channel->note_pitch = *g_sequence_data_needle;
                    g_sequence_data_needle++;
                }

                g_channel->flags |= IS_DRUM;

                effects_load_drum_sound();

                g_channel->note_pitch = 0x3C;
            }

            g_channel->pitch = g_channel->note_pitch << 8;
            g_channel->pitch += g_channel->sound_transpose_detune;
            g_channel->pitch += g_channel->effect_transpose;
            g_channel->pitch += g_channel->effect_detune;
            g_channel->pitch += g_channel->marker_transpose;
            g_channel->pitch += g_channel->nested_marker_transpose;

            if (command < 0x68)
            {
                g_channel->previous_note_duration = *g_sequence_data_needle;
                g_sequence_data_needle++;
            }

            g_channel->note_duration = g_channel->previous_note_duration;

            sequence_data = *g_sequence_data_needle;
            g_sequence_data_needle++;

            if (sequence_data < 0x80)
            {
                g_channel->previous_note_hold_ratio = sequence_data;

                sequence_data = *g_sequence_data_needle;
                g_sequence_data_needle++;
            }

            g_channel->note_hold_ratio = g_channel->previous_note_hold_ratio;
            if (g_channel->note_hold_ratio == 0)
            {
                g_channel->note_hold_duration = 0;
                g_channel->note_velocity = sequence_data & 0x7F;
            }
            else
            {
                // Casting might have issues here.
                g_channel->note_hold_duration = (g_channel->note_duration * g_channel->note_hold_ratio) / 128;

                if (g_channel->note_hold_duration == 0)
                {
                    g_channel->note_hold_duration = 1;
                }

                g_channel->note_velocity = sequence_data & 0x7F;
            }

            g_channel->sequence_data_needle = g_sequence_data_needle;

            if (g_channel->pitch_change_next_note_parameter == 0) 
            {
                g_channel->pitch_change_duration = 0;
            }
            else
            {
                effects_calculate_pitch_change_amount_this_note();
            }

            g_channel->vibrato_pitch = 0;
            if (g_channel->vibrato_depth_target != 0)
            {
                effects_initialize_vibrato();
            }

            player_set_pitch();

            if (note_is_still_playing)
            {
                if (g_channel->envelope_phase_duration == 0) 
                {
                    player_set_volume();
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
            else
            {
                player_start_envelope_attack_phase();
                
                alSynStartVoice(&alGlobals->drvr, &g_voices[g_channel_index], g_channel->wave_table);
            }
            return;
        }
        else
        {
            g_command_table[command - 0xD0]();
        }

        if (command >= 0xF2)
        {
            return;
        }

        sequence_data = *g_sequence_data_needle;
    }
}

void player_update_on_tick()
{
    if (g_channel->pitch_change_duration != 0)
    {
        effects_update_pitch_change_on_tick();
    }

    if (g_channel->vibrato_depth_target != 0)
    {
        effects_update_vibrato_on_tick();
    }

    if ((g_channel->flags & PITCH_IS_CHANGING) != 0)
    {
        player_set_pitch();
    }

    if (g_channel->note_hold_ratio == 0 || g_channel->note_duration != 1) 
    {
        if (g_channel->note_hold_duration != 0 && --g_channel->note_hold_duration == 0)
        {
            player_start_envelope_release_phase();
        }

        if (g_channel->envelope_phase_duration != 0)
        {
            player_update_envelope_phase();
        }

        if ((g_channel->flags & VOLUME_IS_CHANGING) == 0)
        {
            player_set_volume(); // is actually player_fdsdfsfsdf
        }
        else
        {
            player_set_volume();
        }
    }
    else
    {
        player_start_envelope_cutoff_phase();
    }
}

void player_fdsdfsfsdf()
{

}

void player_update_off_tick() 
{
    if (g_channel->pitch_change_duration != 0)
    {
        effects_update_pitch_change_off_tick();
    }

    if (g_channel->vibrato_depth_target != 0)
    {
        effects_update_vibrato();
    }

    if ((g_channel->flags & PITCH_IS_CHANGING) != 0)
    {
        player_set_pitch();
    }

    if (g_channel->envelope_phase_duration != 0) 
    {
        player_update_envelope_phase();
    }

    if (g_channel->flags & VOLUME_IS_CHANGING)
    {
        player_set_volume();
    }
}

void player_set_pitch()
{
    u32 intended_pitch;

    g_channel->flags &= ~PITCH_IS_CHANGING;

    intended_pitch = g_channel->pitch + g_channel->random_pitch + g_channel->vibrato_pitch;

    //DIFFERENCE: In Mahjong Master this normalizes the pitch by repeadetly adding a value in a for loop, MNSG and after just do this.
    if (intended_pitch < 0x0000)
    {
        intended_pitch = 0x0000;
    }
    else if (intended_pitch > 0x4800)
    {
        intended_pitch = 0x4800;
    }

    //DIFFERENCE: In MNSG, it checks if the previous pitch is equal to this, whereas Mahjong Master doesn't even set a previous pitch.
    alSynSetPitch(&alGlobals->drvr, &g_voices[g_channel_index], ((f32) (intended_pitch & 0xFF) * (g_pitch_table[(intended_pitch >> 8) + 1] - g_pitch_table[intended_pitch >> 8])) / 256.0 + g_pitch_table[intended_pitch >> 8]);
}

void player_set_volume()
{
    s32 intended_volume;

    g_channel->flags &= ~VOLUME_IS_CHANGING;

    intended_volume = (g_channel->note_velocity + g_channel->marker_volume + g_channel->nested_marker_volume) * g_channel->volume;

    if (intended_volume < 0x800000)
    {
        intended_volume = intended_volume >> 8;

        if (intended_volume < 0x0000)
        {
            intended_volume = 0x0000;
        }
    }
    else
    {
        intended_volume = 0x7FFF;
    }

    intended_volume = (g_channel->envelope_volume_target * intended_volume) >> 8;

    if (g_channel->sequence_id < 0x100) 
    {
        if (g_fade_amount != 0)
        {
            intended_volume = (g_fade_amount * intended_volume) >> 8;
        }

        // Something else? Find out what it is.
    }

    // Also something else. What is it?
    alSynSetVol(&alGlobals->drvr, &g_voices[g_channel_index], intended_volume, g_channel->envelope_phase_duration * CALLBACK_TIME);
}

void player_start_envelope_attack_phase()
{
    g_channel->envelope_phase_duration = 0;

    if (g_channel->envelope_attack_duration == 0)
    {
        g_channel->envelope_volume_target = 0x100;
        player_set_volume();

        player_start_envelope_decay_phase();
    }
    else
    {
        g_channel->envelope_volume_target = 0x20;
        player_set_volume();

        g_channel->envelope_phase = ATTACK;
        g_channel->envelope_volume_target = 0x100;
        g_channel->envelope_phase_duration = g_channel->envelope_attack_duration;
        player_set_volume();
    }
}

void player_start_envelope_decay_phase()
{
    if (g_channel->envelope_decay_volume == 0)
    {
        player_start_envelope_sustain_phase();
    }
    else
    {
        g_channel->envelope_phase = DECAY;
        g_channel->envelope_volume_target -= g_channel->envelope_decay_volume;
        g_channel->envelope_phase_duration = g_channel->envelope_decay_duration;
        player_set_volume();
    }
}

void player_start_envelope_sustain_phase()
{
    g_channel->envelope_phase = SUSTAIN;

    if (g_channel->envelope_sustain_duration != 0)
    {
        g_channel->envelope_volume_target = 0;
        g_channel->envelope_phase_duration = g_channel->envelope_sustain_duration;
        player_set_volume();
    }
}

void player_start_envelope_release_phase()
{
    g_channel->envelope_phase = RELEASE;
    g_channel->envelope_volume_target = 0;
    g_channel->envelope_phase_duration = g_channel->envelope_release_duration;
    player_set_volume();
}

void player_start_envelope_cutoff_phase()
{
    g_channel->voice_status = 1;
    g_channel->envelope_phase = RELEASE;
    g_channel->envelope_volume_target = 0;
    g_channel->envelope_phase_duration = 1;
    player_set_volume();
}

void player_update_envelope_phase()
{
    g_channel->envelope_phase_duration--;
    if (g_channel->envelope_phase_duration == 0)
    {
        if (g_channel->envelope_phase == ATTACK)
        {
            player_start_envelope_decay_phase();
        }

        if (g_channel->envelope_phase == DECAY)
        {
            player_start_envelope_sustain_phase();
        }
    }
}
