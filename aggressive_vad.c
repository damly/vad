#include "aggressive_vad.h"
#include "webrtc/common_audio/vad/include/webrtc_vad.h"
#include "webrtc/modules/audio_processing/ns/noise_suppression_x.h"
#include "webrtc/modules/audio_processing/ns/noise_suppression.h"


struct AggressiveVadInstT {
    VadInst *vad;
    int fs;
    int mode;
    int frameSize;
    NsxHandle *nsx;
    int bands;
    short **nsxInData;
    short **nsxOutData;
    short *nsxTempData;
};

AggressiveVadInst* AggressiveVad_Create(int fs, int mode) {
    AggressiveVadInst* self = malloc(sizeof(AggressiveVadInst));
    self->vad = NULL;
    self->nsx = NULL;
    self->nsxInData = NULL;
    self->nsxOutData = NULL;
    self->nsxTempData = NULL;
    self->fs = fs;
    self->mode = mode;
    self->bands = 1;
    
    switch(fs) {
        case 8000: {
            self->frameSize = 80;
            break;
        }
        case 16000: {
            self->frameSize = 160;
            break;
        }    
        case 32000: {
            self->frameSize = 320;
            break;
        }
        default: {
            perror("Sampling frequency (Hz) must be in  8000, 16000, or 32000!"); 
            free(self);
            return NULL;
        }
    }

    self->nsxTempData = (short*)malloc(self->frameSize*sizeof(short));
    self->nsxInData = (short**)malloc(self->bands*sizeof(short*));
    self->nsxInData[0] = (short*)malloc(self->frameSize*sizeof(short));
    self->nsxOutData = (short**)malloc(self->bands*sizeof(short*));
    self->nsxOutData[0] = (short*)malloc(self->frameSize*sizeof(short));

    if ((self->vad = WebRtcVad_Create()) == NULL) {  
		perror("WebRtcVad_Create failed!");
        AggressiveVad_Free(self);
		return NULL;  
	}  
	
	if (WebRtcVad_Init(self->vad)){  
		perror("WebRtcVad_Init failed!");  
        AggressiveVad_Free(self);
		return NULL;  
	}  
	
	if (WebRtcVad_set_mode(self->vad, self->mode)) {  
		perror("WebRtcVad_set_mode failed!");  
        AggressiveVad_Free(self);
		return NULL;  
	}

    if ((self->nsx = WebRtcNsx_Create()) == NULL) {  
        perror("WebRtcNsx_Create failed!"); 
        AggressiveVad_Free(self);
        return NULL;
    }
   
    if(WebRtcNsx_Init(self->nsx, fs) == -1){
        perror("WebRtcNsx_Init failed!"); 
        AggressiveVad_Free(self);
        free(self);
        return NULL;
    }

    if(WebRtcNsx_set_policy(self->nsx, 2) == -1){
        perror("WebRtcNsx_Init failed!"); 
        AggressiveVad_Free(self);
        return NULL;
    }

    return self;
}

void AggressiveVad_Free(AggressiveVadInst* self) {
    if(self) {
        if(self->nsx) WebRtcNsx_Free(self->nsx);
        if(self->nsx) WebRtcVad_Free(self->vad); 
        if(self->nsxTempData) free(self->nsxTempData);
        if(self->nsxInData) {
            free(self->nsxInData[0]);
            free(self->nsxInData);
        }
        if(self->nsxOutData) {
            free(self->nsxOutData[0]);
            free(self->nsxOutData);
        }
      
        free(self);
    }
}

int AggressiveVad_Process0(AggressiveVadInst* self, const int16_t* audio_frame,  size_t frame_length) {
    
    if(frame_length != self->frameSize) {
        perror("frame_length is invalid!");
        return -1;
    }

    int frameLen = self->frameSize*sizeof(short);

    memset(self->nsxTempData, 0, frameLen);
  
    memcpy(self->nsxInData[0], audio_frame, frameLen);

    WebRtcNsx_Process(self->nsx, self->nsxInData, self->bands, self->nsxOutData);

    memcpy(self->nsxTempData, self->nsxOutData[0], frameLen);
    
    return WebRtcVad_Process(self->vad, self->fs, (const int16_t *)self->nsxTempData, frame_length);
}

int AggressiveVad_Process(AggressiveVadInst* self, const int16_t* audio_frames,  size_t frames_length, float scale) {
    
    if(!self) {
        perror("AggressiveVadInst is NULL!");
        return -1;
    }

    int frameLen = self->frameSize*sizeof(short);
    int count = 0;
    int size = frames_length / self->frameSize;
    if(size == 0) {
        return -1;
    }
    int ret = 0;
    for(int i=0; i<size; i++) {
        if(AggressiveVad_Process0(self, audio_frames+i*frameLen, self->frameSize) == 1) {
            count++;
        }
    }

    if(count >= size*scale) {
        return 1;
    } else {
        return 0;
    }
}