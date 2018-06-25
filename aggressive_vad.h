#ifndef AGGRESSIVE_VAD_H_
#define AGGRESSIVE_VAD_H_

#include <stddef.h>
#include <stdint.h>

typedef struct AggressiveVadInstT AggressiveVadInst;

#ifdef __cplusplus
extern "C" {
#endif

// Creates an instance to the VAD structure.
//
// - fs     [i]   : Sampling frequency (Hz): 8000, 16000, or 32000
// - mode   [i]   : Aggressiveness mode (0, 1, 2, or 3).
//
// returns        : NULL - (Default ampling frequency or Default mode could not be set).
AggressiveVadInst* AggressiveVad_Create(int fs, int mode);

// Frees the dynamic memory of a specified VAD instance.
//
// - handle [i] : Pointer to VAD instance that should be freed.
void AggressiveVad_Free(AggressiveVadInst* handle);

// Calculates a VAD decision for the |audio_frame|. For valid sampling rates
// frame lengths, see the description of WebRtcVad_ValidRatesAndFrameLengths().
//
// - handle       [i/o] : VAD Instance. Needs to be initialized by
//                        AggressiveVad_Init() before call.
// - audio_frame  [i]   : Audio frame buffer.
// - frame_length [i]   : Length of audio frame buffer in number of samples.
//
// returns              : 1 - (Active Voice),
//                        0 - (Non-active Voice),
//                       -1 - (Error)
int AggressiveVad_Process0(AggressiveVadInst* handle, const int16_t* audio_frame,
                      size_t frame_length);


// Calculates a VAD decision for the |audio_frame|. For valid sampling rates
// frame lengths must be (n x 10) ms frames and the rates 8000, 16000 and 32000 Hz.
//
// - handle       [i/o] : VAD Instance. Needs to be initialized by
//                        AggressiveVad_Init() before call.
// - audio_frame  [i]   : Audio frame buffer.
// - frame_length [i]   : Length of audio frames buffer in number of samples.
// - scale        [f]   : Scale of Active Voice frames buffer audio frames data.
//
// returns              : 1 - (Active Voice),
//                        0 - (Non-active Voice),
//                       -1 - (Error)
int AggressiveVad_Process(AggressiveVadInst* handle, const int16_t* audio_frames,
                      size_t frames_length, float scale);

// Checks for valid combinations of |rate| and |frame_length|. We support 10 ms frames 
//    and the rates 8000, 16000 and 32000 Hz.
//
// - rate         [i] : Sampling frequency (Hz).
// - frame_length [i] : Speech frame buffer length in number of samples.
//
// returns            : 0 - (valid combination), -1 - (invalid combination)
int AggressiveVad_ValidRateAndFrameLength(int rate, size_t frame_length);

#ifdef __cplusplus
}
#endif

#endif  // AGGRESSIVE_VAD_H_