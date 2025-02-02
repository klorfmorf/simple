#ifndef AUDIO_H
#define AUDIO_H

#include <ultra64.h>

#include "main/scheduler.h"

#define FREQUENCY 44100

#define AUDIO_LIST_COUNT 2

#define AUDIO_TASK_COUNT 3

#define RETRACE_MESSAGE_BUFFER_COUNT 64

#define TASK_MESSAGE_BUFFER_COUNT 64

#define DMA_MESSAGE_BUFFER_COUNT 64

#define DMA_IO_MESSAGE_BUFFER_COUNT 64

#define AUDIO_THREAD_STACK_COUNT 0x6000

#define JITTER_SAMPLES 0x100

#define DMA_BUFFER_COUNT 24

#define AUDIO_HEAP_SIZE 0x4E000

#define CALLBACK_TIME 5000

#define INITIAL_DELAY 6

#define BANK_COUNT 3

#define REQUESTS_COUNT 8

#define CHANNEL_COUNT 16

#define VOLUME_IS_CHANGING (1 << 0)
#define PITCH_IS_CHANGING (1 << 1)
#define IS_DRUM (1 << 2)

typedef enum EnvelopePhase
{
    ATTACK,
    DECAY,
    SUSTAIN,
    RELEASE
} EnvelopePhase;

typedef enum PitchChangeType
{
    FIXED_LENGTH,
    VARIABLE_LENGTH
} PitchChangeType;

typedef struct Channel
{
    u32 sequence_id;
    s8 priority;
    s8 initial_delay_counter;
    u8 flags;
    u8 unknown_4;
    u8 *sequence_data_needle;
    u8 bank;
    u8 sound;
    u8 voice_status;
    s8 dry_wet_mix;
    ALWaveTable *wave_table;
    u16 tempo;
    u16 tempo_counter;
    u8 tempo_change_target;
    u8 tempo_change_duration;
    s16 tempo_change_amount;
    u8 previous_note_duration;
    u8 note_duration;
    u8 note_hold_ratio;
    u8 previous_note_hold_ratio;
    s8 note_hold_duration;
    u8 note_pitch;
    u8 unknown_22;
    u8 unknown_23;
    u32 pitch;
    s16 sound_transpose_detune;
    s16 effect_transpose;
    s16 effect_detune;
    s16 marker_transpose;
    s16 nested_marker_transpose;
    s16 vibrato_pitch;
    s16 random_pitch;
    u16 volume;
    u8 unknown_33;
    u8 unknown_34;
    u8 unknown_35;
    u8 unknown_36;
    s16 marker_volume;
    s16 nested_marker_volume;
    s16 volume_change_amount;
    u8 volume_change_target;
    u8 volume_change_duration;
    u8 note_velocity;
    u8 envelope_phase;
    u16 envelope_volume_target;
    u16 envelope_phase_duration;
    u16 envelope_attack_duration;
    u16 envelope_decay_duration;
    u16 envelope_sustain_duration;
    s16 envelope_decay_volume;
    u16 envelope_release_duration;
    u16 pan;
    u8 pan_change_target;
    u8 pan_change_duration;
    s16 pan_change_amount;
    s8 marker_loop_counter;
    u8 unknown_56;
    u8 *marker_needle;
    s8 nested_marker_loop_counter;
    u8 unknown_59;
    u8 unknown_60;
    u8 unknown_61;
    u8 *nested_marker_needle;
    u8 *master_marker_needle;
    s8 unknown_64;
    u8 unknown_65;
    u8 unknown_66;
    u8 unknown_67;
    u8 *conditional_marker_needle;
    u8 *unknown_69;
    s8 pitch_change_delay_duration;
    u8 pitch_change_duration;
    u8 unknown_72;
    u8 unknown_73;
    u32 pitch_change_target;
    s32 pitch_change_amount;
    u8 pitch_change_next_note_delay_duration;
    u8 pitch_change_next_note_duration;
    s16 pitch_change_next_note_parameter;
    u8 pitch_change_type;
    u8 random_index;
    u16 random_pitch_mask;
    u16 random_counter;
    u8 random_rate;
    u8 vibrato_intended_delay_duration;
    s8 vibrato_delay_duration;
    u8 vibrato_rate;
    u16 vibrato_counter;
    u8 vibrato_oscillation;
    u8 vibrato_oscillation_step_size;
    u16 vibrato_depth_target;
    u16 vibrato_depth;
    u8 vibrato_intended_start_duration;
    s8 vibrato_start_duration;
    u16 vibrato_depth_change_amount;
    u8 unknown_95;
    u8 unknown_96;
} Channel;

typedef struct Sound
{
    s8 transpose;
    s8 detune;
    s8 attack;
    u8 decay;
    u8 sustain;
    s8 release;
    s8 pan;
} Sound;

typedef struct DrumSound
{
    u8 id;
    Sound sound;
} DrumSound;

typedef struct AudioConfig
{
    u32 frequency;
    u32 samples_per_frame_coefficient;
    u32 audio_list_size;
} AudioConfig;

typedef struct DMABuffer
{
    ALLink node;
    int startAddr;
    u32 lastFrame;
    char *ptr;
} DMABuffer;

typedef struct DMAState
{
    u8 initialized;
    DMABuffer *firstUsed;
    DMABuffer *firstFree;
} DMAState;

typedef struct AudioTask
{
    s16 *sample_buffer;
    u16 sample_count;
    u16 unknown_1;
    OSScTask task;
    OSScMsg message;
} AudioTask;

typedef struct AudioMessage
{
    s16 type;
    s16 pad;
    AudioTask *completed_task;
} AudioMessage;

typedef struct SoundEffect
{
    u8 track_count_priority; // Upper 3 bits: track count, lower 5 bits: priority
    u8 channel_offset_bank; // Upper 4 bits: channel offset, lower 4 bits: bank
    u8 unknown[2];
    void *data; // Pointer to either an array of pointers to sequence data or a pointer to sequence data.
} SoundEffect;

/* ----- thread Function Prototypes ----- */

void thread_create(ALSynConfig *synthesizer_config, OSPri priority, AudioConfig *audio_config);
void thread_main(void *argument);
s32 thread_generate_audio_task(AudioTask *new_task, AudioTask *completed_task);
s32 dmaCallBack(s32 addr, s32 len, void *state);
ALDMAproc dmaNew(DMAState **state);
void CleanDMABuffs();

/* ----- external Function Prototypes ----- */

void external_initialize_player(ALPlayer *player);
ALMicroTime external_player_handler(void *player);
void external_initialize_voices();
void external_initialize_channels(); // TEMPORARY
void external_handle_requests();

/* ----- player Function Prototypes  ----- */
void player_update_channels();
void player_update_channel();
void player_read_data();
void player_update_on_tick();
void player_fdsdfsfsdf();
void player_update_off_tick();
void player_set_pitch();
void player_set_volume();
void player_start_envelope_attack_phase();
void player_start_envelope_decay_phase();
void player_start_envelope_sustain_phase();
void player_start_envelope_release_phase();
void player_start_envelope_cutoff_phase();
void player_update_envelope_phase();

/* ----- effects Function Prototypes  ----- */
void effects_set_tempo();
void effects_set_tempo_change();
void effects_update_tempo_change();
void effects_set_sound();
void effects_load_sound(Sound *sound);
void effects_load_drum_sound();
void effects_set_volume_sound();
void effects_set_volume_sound_pan();
void effects_set_volume();
void effects_set_volume_change();
void effects_update_volume_change();
void effects_set_attack_decay();
void effects_set_sustain();
void effects_set_release();
void effects_set_reverb();
void effects_set_pan();
void effects_set_pan_(); // Figure out a name for this.
void effects_set_pan_change();
void effects_update_pan_change();
void effects_set_transpose();
void effects_set_detune();
void effects_set_vibrato();
void effects_initialize_vibrato();
void effects_update_vibrato_on_tick();
void effects_update_vibrato();
void effects_set_vibrato_change();
void effects_set_random();
void effects_update_random();
void effects_set_pitch_bend_dummy();
void effects_calculate_pitch_change_amount();
void effects_set_pitch_bend();
void effects_update_pitch_change_on_tick();
void effects_update_pitch_change_off_tick();
void effects_set_pitch_bend_start();
void effects_calculate_pitch_change_amount_this_note(); // Maybe this needs a different name.
void effects_set_portamento();
void effects_update_portamento();
void effects_set_marker();
void effects_go_to_marker();
void effects_set_nested_marker();
void effects_go_to_nested_marker();
void effects_set_master_marker();
void effects_go_to_master_marker();
void effects_set_conditional_marker();
void effects_go_to_conditional_marker();
void effects_set_echo();
void effects_set_bank();
void effects_set_nothing();
void effects_rest();
void effects_tie();
void effects_end();

#endif // AUDIO_H
