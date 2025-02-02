#include <ultra64.h>

#include "main/main.h"

#include "audio.h"

#define ms * (((s32) ((f32) FREQUENCY / 1000)) & ~0x7)

/* ----- externs ----- */
extern u8 _midibankSegmentRomStart[];
extern u8 _midibankSegmentRomEnd[];
extern u8 _miditableSegmentRomStart[];

extern u8 _midibank2SegmentRomStart[];
extern u8 _midibank2SegmentRomEnd[];
extern u8 _miditable2SegmentRomStart[];

extern u8 _midibank3SegmentRomStart[];
extern u8 _midibank3SegmentRomEnd[];
extern u8 _miditable3SegmentRomStart[];

// CHANGE THIS!!!!
#include "includes/mnsg_sequences/68.kseq.h"

extern ALVoice g_voices[CHANNEL_COUNT];
extern Channel g_channels[CHANNEL_COUNT];
extern Channel *g_channel;

/* ----- .data ----- */
s32 g_custom_parameters[10] = {
    1, 300 ms,
    0, 179 ms, 12000, 0, 0x7FFF, 0, 0, 0
};

u8 *g_wave_table_offsets[BANK_COUNT] = {
    _miditableSegmentRomStart,
    _miditable2SegmentRomStart,
    _miditable3SegmentRomStart,
};

u8 *g_bank_file_start_offsets[BANK_COUNT] = {
    _midibankSegmentRomStart,
    _midibank2SegmentRomStart,
    _midibank3SegmentRomStart,
};

u8 *g_bank_file_end_offsets[BANK_COUNT] = {
    _midibankSegmentRomEnd,
    _midibank2SegmentRomEnd,
    _midibank3SegmentRomEnd,
};

/* ----- .bss ----- */
ALFxRef g_al_fx_ref;

ALPlayer g_al_player;

u8 g_audio_heap[AUDIO_HEAP_SIZE];

ALHeap g_al_heap;

ALBankFile *g_al_bank_files[BANK_COUNT];

u8 g_initial_delay_counter;
u8 g_disable_request_handling;
u16 g_counter;
u8 g_voice_index;
u8 g_paused;
u8 g_mono;
u8 g_fade_counter_reset_value;
u8 g_fade_counter;
u8 g_fade_amount;
u8 g_tempo_increase_amount;
u8 g_tempo_increase_reference;
// ...
// ...
// ... [1]
// ... [1]
// ... 
u8 g_request_count;
u32 g_requests[REQUESTS_COUNT];
u32 g_request;

void external_initialize() {
    s32 index;
    ALBankFile *bank_file_allocation;
    s32 bank_file_size;
    ALSynConfig al_syn_config;
    AudioConfig audio_config;

    alHeapInit(&g_al_heap, g_audio_heap, AUDIO_HEAP_SIZE);

    for (index = 0; index < BANK_COUNT; index++) {
        bank_file_size = g_bank_file_end_offsets[index] - g_bank_file_start_offsets[index];
        bank_file_allocation = (ALBankFile *) alHeapAlloc(&g_al_heap, 1, bank_file_size);
        g_al_bank_files[index] = bank_file_allocation;

        dma_cartridge_to_ram((u32) g_bank_file_start_offsets[index], g_al_bank_files[index], bank_file_size);
    }

    for (index = 0; index < BANK_COUNT; index++) {
        alBnkfNew(g_al_bank_files[index], g_wave_table_offsets[index]);
    }

    al_syn_config.maxVVoices = 16;
    al_syn_config.maxPVoices = 16;
    al_syn_config.maxUpdates = 160;
    al_syn_config.dmaproc = NULL;
    al_syn_config.outputRate = 0;
    al_syn_config.heap = &g_al_heap;
    al_syn_config.fxType = AL_FX_CUSTOM;
    al_syn_config.params = g_custom_parameters;

    audio_config.frequency = FREQUENCY;
    audio_config.samples_per_frame_coefficient = 1;
    audio_config.audio_list_size = 0x1000;

    thread_create(&al_syn_config, 12, &audio_config);
    external_initialize_player(&g_al_player);
}

void external_initialize_player(ALPlayer *player) {
    g_initial_delay_counter = INITIAL_DELAY;
    // ...
    g_paused = FALSE;
    g_mono = FALSE;
    // ...
    g_counter = 0;

    player->next = NULL;
    player->handler = &external_player_handler;
    player->clientData = player;

    alSynAddPlayer(&alGlobals->drvr, player);
}

ALMicroTime external_player_handler(void *player) {
    if (g_initial_delay_counter == 0) {

        if ((g_counter & 1) == 0 && !g_disable_request_handling) 
        {
            // Handle requests here...
        }

        // Load sequences here...

        player_update_channels();

        g_counter++;
    }
    else {
        if (g_initial_delay_counter == INITIAL_DELAY) {
            external_initialize_voices();
            //TEMPORARY
            external_initialize_channels();
        }

        g_initial_delay_counter--;
    }

    return CALLBACK_TIME;
}

void external_initialize_voices() {
    Channel *channel;
    ALVoice *voice;
    ALVoiceConfig voice_config;

    voice = g_voices;
    channel = g_channels;
    
    do {
        voice_config.priority = 10;
        voice_config.fxBus = 0;
        voice_config.unityPitch = 0;

        alSynAllocVoice(&alGlobals->drvr, voice, &voice_config);

        g_channel = channel;

        //POTENTIALFIX: Why? Just use g_channel if you set it already...
        channel->sound = 0; 
        g_channel->bank = g_channel->sound;
        g_channel->wave_table = g_al_bank_files[g_channel->bank]->bankArray[0]->instArray[g_channel->sound]->soundArray[0]->wavetable;

        alSynStartVoiceParams(&alGlobals->drvr, voice, g_channel->wave_table, 0.0f, 0, 0, 64, 0);

        voice++;
        channel++;
    } while (channel != &g_channels[CHANNEL_COUNT]);

    g_al_fx_ref = alSynGetFXRef(&alGlobals->drvr, 0, 0);
}

void external_initialize_channels() {
    //TEMPORARY: Should not be here, do it properly later.
    u8 track_count = seq_tracks;
    u8 channel_offset = 0;
    u8 priority = 0x1E;
    u8 bank = 0;
    u8 initial_delay = 6;
    u8 *tracks_sequence_data[CHANNEL_COUNT];
    s32 index;

    for (index = 0; index < track_count; index++)
    {
        tracks_sequence_data[index] = seq_data + (seq_data[(index * sizeof(u16))] << 8 | seq_data[(index * sizeof(u16)) + 1]);
    }

    g_request = 1;
    
    if (track_count != 0)
    {
        for (index = 0; index < track_count; index++)
        {
            // if (g_channels[index + channel_offset].dry_wet_mix != 0)
            // {
                g_channels[index + channel_offset].dry_wet_mix = 0;
                alSynSetFXMix(&alGlobals->drvr, &g_voices[index + channel_offset], 0);
            // }

            if (g_request >= 0x100)
            {
                alSynSetVol(&alGlobals->drvr, &g_voices[index + channel_offset], 0, CALLBACK_TIME);
                g_channels[index + channel_offset].voice_status = 2;
            }

            g_channels[index + channel_offset].initial_delay_counter = initial_delay;
            g_channels[index + channel_offset].sequence_id = g_request;
            g_channels[index + channel_offset].priority = priority;
            g_channels[index + channel_offset].bank = bank;
            g_channels[index + channel_offset].random_pitch = 0;
            g_channels[index + channel_offset].effect_detune = 0;
            g_channels[index + channel_offset].nested_marker_transpose = 0;
            g_channels[index + channel_offset].nested_marker_volume = 0;
            g_channels[index + channel_offset].pitch_change_next_note_parameter = 0;
            g_channels[index + channel_offset].tempo = 0x8000;
            g_channels[index + channel_offset].tempo_counter = 0;
            g_channels[index + channel_offset].note_duration = 1;
            g_channels[index + channel_offset].note_hold_ratio = 1;
            g_channels[index + channel_offset].tempo_change_duration = 0;
            g_channels[index + channel_offset].volume_change_duration = 0;
            g_channels[index + channel_offset].pan_change_duration = 0;
            g_channels[index + channel_offset].pan = 0;
            g_channels[index + channel_offset].sequence_data_needle = tracks_sequence_data[index];
            g_channels[index + channel_offset].random_rate = 0; // is set to volume?
            g_channels[index + channel_offset].vibrato_depth_target = 0; // this too
            g_channels[index + channel_offset].effect_transpose = 0;
            g_channels[index + channel_offset].marker_transpose = 0;
            g_channels[index + channel_offset].marker_volume = 0;
            g_channels[index + channel_offset].pitch_change_duration = 0;
        }
    }
}

void external_handle_requests() {
    while (g_request_count != 0) 
    {
        g_request_count--;
        g_request = g_requests[g_request_count];

        if ((g_request & 0xFFFF) < 0x100) 
        {
            if (g_request < 0x10) 
            {

            }
            else
            {

            }
        }
        else if ((g_request & 0xFFFF) < 0x8000)
        {

        }
        else
        {

        }
    }
}