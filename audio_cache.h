#ifndef AUDIO_CACHE_H_
#define AUDIO_CACHE_H_

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct AudioCacheT AudioCache;

#ifdef __cplusplus
extern "C" {
#endif

// Creates an instance to the Audio Cache structure.
AudioCache *AudioCache_Create(void);

// Frees the dynamic memory of a specified Audio Cache instance.
//
// - handle [i] : Pointer to Audio Cache instance that should be freed.
void AudioCache_Free(AudioCache *handle);


// Clear the data memory of a specified Audio Cache instance.
//
// - handle [i] : Pointer to Audio Cache instance that should be freed.
void AudioCache_Clear(AudioCache *handle);

//  Determine whether the Audio Cache is empty 
//
// - handle       [i/o] : Audio Cache Instance. Needs to be initialized by
//                        AudioCache_Create() before call.
//
// returns              : 1 - (is empty),
//                        0 - (is not empty)
int AudioCache_IsEmpty(AudioCache *handle);

// Adding data to the cache queue
//
// - handle         [i/o] : Audio Cache Instance. Needs to be initialized by
//                        AudioCache_Create() before call.
// - data           [i/o] :  Audio frames data.
// - frame_length   [i] :  Audio frames data length.
// - is_voice       [i] :  Audio frames is voice.
//
void AudioCache_Push(AudioCache *self, const int8_t* data, const size_t frame_length, int is_voice);

// Deleting one data from Audio Cache
//
// - handle       [i/o] : Audio Cache Instance. Needs to be initialized by
//                        AudioCache_Create() before call.
// returns              : Audio frames data.
//
void AudioCache_Pop(AudioCache *handle);

// Getting the length of Audio Cache
//
// - handle       [i/o] : Audio Cache Instance. Needs to be initialized by
//                        AudioCache_Create() before call.
// returns              : Audio Cache size.
//
int AudioCache_Length(AudioCache *self);

// Getting the length of Audio Cache Data size
//
// - handle       [i/o] : Audio Cache Instance. Needs to be initialized by
//                        AudioCache_Create() before call.
// returns              : Audio Cache size.
//
int AudioCache_DataSize(AudioCache *self);

// Getting add data of Audio Cache
//
// - handle       [i/o] : Audio Cache Instance. Needs to be initialized by
//                        AudioCache_Create() before call.
// returns              : All Audio Cache Data.
//
int8_t *AudioCache_Bytes(AudioCache *self);

// Judging the proportion of sound data
//
// - handle       [i/o] : Audio Cache Instance. Needs to be initialized by
//                        AudioCache_Create() before call.
// - scale          [f] : voice's scale
// returns              : 1 - (is voice),
//                        false - (is not voice)
//
int AudioCache_IsVoice(AudioCache *self, float scale);

// Judge the proportions of mute data
//
// - handle       [i/o] : Audio Cache Instance. Needs to be initialized by
//                        AudioCache_Create() before call.
// - scale          [f] : none voice's scale
// returns              : 1 - (is voice),
//                        false - (is not voice)
//
int AudioCache_IsNoneVoice(AudioCache *self, float scale);

#ifdef __cplusplus
}
#endif

#endif  // AUDIO_CACHE_H_