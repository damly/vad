#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdio.h>  
#include<stdlib.h>  
#include<string.h>  
#include <time.h> 
#include "webrtc/common_audio/vad/include/webrtc_vad.h"
#include "webrtc/modules/audio_processing/ns/noise_suppression_x.h"
#include "webrtc/modules/audio_processing/ns/noise_suppression.h"
#include "aggressive_vad.h"
#include "audio_cache.h"

int TestNs(char *filename, char *outfilename, int mode)
{
    //alloc
    FILE *infp=fopen(filename,"r");
    int nBands = 1;
    int frameSize = 160;//10ms对应于160个short
    short *temp = (short*)malloc(frameSize*sizeof(short));
    float **pData = (float**)malloc(nBands*sizeof(float*));
    pData[0] = (float*)malloc(frameSize*sizeof(float));

    float **pOutData = (float**)malloc(nBands*sizeof(float*));
    pOutData[0] = (float*)malloc(frameSize*sizeof(float));
    int offset = 0;

    //init
    NsHandle *handle = 0;
    int ret = 0;
    handle=WebRtcNs_Create();
    ret= WebRtcNs_Init(handle,16000);
    if(ret==-1){
        printf("%s","init failure!");
        return -1;
    }
    ret = WebRtcNs_set_policy(handle,mode);

    //process
    FILE *outfp = fopen(outfilename,"w");
    int len = frameSize;
    while(len > 0)
    {
        memset(temp, 0, frameSize*sizeof(short));
        len = fread(temp, sizeof(short), frameSize, infp);
        int i;
        for(i = 0; i < frameSize; i++)  {
            pData[0][i] = (float)temp[i];
        }
        WebRtcNs_AnalyzeCore(handle,pData[0]);
        WebRtcNs_Process(handle,pData,nBands,pOutData);
        for(i = 0; i < frameSize; i++) {
            temp[i] = (short)pOutData[0][i];
        }
        //write file
        len = fwrite(temp,sizeof(short),len,outfp);
    }
    fclose(infp);
    fclose(outfp);

    WebRtcNs_Free(handle);
    free(temp);
    free(pData[0]);
    free(pData);
    free(pOutData[0]);
    free(pOutData);
}

int TestNsx(char *filename, char *outfilename, int mode)
{
    //alloc
    FILE *infp=fopen(filename,"r");
    int nBands = 1;
    int frameSize = 160;//10ms对应于160个short
    short *temp = (short*)malloc(frameSize*sizeof(short));
    short **pData = (short**)malloc(nBands*sizeof(short*));
    pData[0] = (short*)malloc(frameSize*sizeof(short));

    short **pOutData = (short**)malloc(nBands*sizeof(short*));
    pOutData[0] = (short*)malloc(frameSize*sizeof(short));
    int offset = 0;

    //init
    NsxHandle *handle = 0;
    int ret = 0;
    handle=WebRtcNsx_Create();
    ret= WebRtcNsx_Init(handle,16000);
    if(ret==-1){
        printf("%s","init failure!");
        return -1;
    }
    ret = WebRtcNsx_set_policy(handle, mode);

    //process
    FILE *outfp = fopen(outfilename,"w");
    int len = frameSize;
    while(len > 0)
    {
        memset(temp, 0, frameSize*sizeof(short));
        len = fread(temp, sizeof(short), frameSize, infp);
        int i;

        for(i = 0; i < frameSize; i++)  {
            pData[0][i] = (short)temp[i];
        }
	
       // WebRtcNsx_AnalyzeCore(handle,pData[0]);
        WebRtcNsx_Process(handle,pData,nBands,pOutData);

        for(i = 0; i < frameSize; i++) {
            temp[i] = (short)pOutData[0][i];
        }
        //write file
        len = fwrite(temp,sizeof(short),len,outfp);
    }
    fclose(infp);
    fclose(outfp);

    WebRtcNsx_Free(handle);
    free(temp);
    free(pData[0]);
    free(pData);
    free(pOutData[0]);
    free(pOutData);
}

void TestVAD(char* pAudioFile,char* pResFile,int nSample,int nMode)  
{  
	VadInst* pVad = NULL;  
	if ((pVad = WebRtcVad_Create()) == NULL) {  
		perror("WebRtcVad_Create failed!");  
		return;  
	}  
	
	if (WebRtcVad_Init(pVad)){  
		perror("WebRtcVad_Init failed!");  
		return;  
	}  
	
	if (WebRtcVad_set_mode(pVad, nMode)) {  
		perror("WebRtcVad_set_mode failed!");  
		return;  
	}  
	
	FILE* fp = NULL;  
	FILE* fpR = NULL;  
	fp = fopen(pAudioFile, "rb");  
	fpR = fopen(pResFile, "wb");  
	fseek(fp, 0, SEEK_END);  
	unsigned int nLen = ftell(fp);  
	fseek(fp, 0, SEEK_SET);  
	short shBufferIn[160] = { 0 };  
	while (1) {  
		if (160 != fread(shBufferIn, 2, 160, fp))  
			break;  
		int nRet = WebRtcVad_Process(pVad, 16000, shBufferIn, 160);   
		if (1 == nRet) {  
			fwrite(shBufferIn, 2, 160, fpR);  
		}  
	}  
	
	fclose(fpR);  
	fclose(fp);  
	WebRtcVad_Free(pVad);  
}  

void FlushToFile(char *name,int index, const int8_t*data, size_t length) {

	char pResFile[256] = {0};
    sprintf(pResFile, "./%s_%d_%d.pcm", name, index,length);  

	FILE* fpR = NULL; 
	fpR = fopen(pResFile, "wb"); 

	fwrite(data, 1, length, fpR); 
	fclose(fpR);
}

void TestAggressiveVAD0(char* pAudioFile,char* pResFile,int nSample,int nMode) {
	AggressiveVadInst* pVad = NULL;  
	if ((pVad = AggressiveVad_Create(nSample, nMode)) == NULL) {  
		perror("AggressiveVad_Create failed!");  
		return;  
	} 
	
	int sg = 0;
	AudioCache *pCache = AudioCache_Create();

	AudioCache *pTriggered = AudioCache_Create();

	AudioCache *pVoice = NULL;

	bool isTriggered = false;

	FILE* fp = NULL;  
	FILE* fpR = NULL;  
	FILE* fpC = NULL; 
	fp = fopen(pAudioFile, "rb");  
	fpR = fopen(pResFile, "wb"); 
	fpC = fopen("./cache.pcm", "wb");
	fseek(fp, 0, SEEK_END);  
	unsigned int nLen = ftell(fp);  
	fseek(fp, 0, SEEK_SET);  
	short shBufferIn[160] = { 0 };  
	while (1) {  
		if (160 != fread(shBufferIn, 2, 160, fp))  
			break;  

		int nRet = AggressiveVad_Process0(pVad, shBufferIn, 160);   
		AudioCache_Push(pCache, (const int8_t *)shBufferIn, 160*sizeof(short), nRet);

		printf("triggered len=%d  data:%d\n", AudioCache_Length(pTriggered), AudioCache_DataSize(pTriggered));

		AudioCache_Push(pTriggered, (const int8_t *)shBufferIn, 160*sizeof(short), nRet);
		if(pVoice) {
			AudioCache_Push(pVoice, (const int8_t *)shBufferIn, 160*sizeof(short), nRet);
		}
		if (1 == nRet) {  
			fwrite(shBufferIn, 2, 160, fpR);  
		}  

		if(AudioCache_Length(pTriggered) == 20) {
			if(isTriggered) {
				if(AudioCache_IsNoneVoice(pTriggered, 0.9f) == 1) {
					printf("is none voice\n");
					isTriggered = false;
					printf("pre voice len=%d\n", AudioCache_Length(pVoice));
					FlushToFile("sg", sg, AudioCache_Bytes(pVoice), AudioCache_DataSize(pVoice));
					AudioCache_Free(pVoice);
					pVoice = NULL;
					sg++;
				}
			}
			else if(AudioCache_IsVoice(pTriggered, 0.5) == 1) {
				printf("is voice\n");
				isTriggered = true;
				pVoice = pTriggered;
				pTriggered = AudioCache_Create();
			}
			else {
				printf("hh is none voice\n");
			}
			if(AudioCache_IsEmpty(pTriggered) == 0) {
				AudioCache_Clear(pTriggered);
			}
		}
	}
	
	printf("cache len=%d  data:%d\n", AudioCache_Length(pCache), AudioCache_DataSize(pCache));
	fwrite(AudioCache_Bytes(pCache), 1, AudioCache_DataSize(pCache), fpC); 

	while(AudioCache_Length(pCache) > 0) {
		AudioCache_Pop(pCache);
//		printf("pop len=%d  data:%d\n", AudioCache_length(pCache), AudioCache_dataSize(pCache));
	}

	if(pVoice) {
		FlushToFile("sg", sg, AudioCache_Bytes(pVoice), AudioCache_DataSize(pVoice));
		AudioCache_Free(pVoice);
	}
	fclose(fpR);  
	fclose(fp);  
	fclose(fpC);
	AggressiveVad_Free(pVad);
	AudioCache_Free(pCache);
	AudioCache_Free(pTriggered);
}

void TestAggressiveVAD(char* pAudioFile, int nSample, int nMode, int frames) {
	AggressiveVadInst* pVad = NULL;  
	if ((pVad = AggressiveVad_Create(nSample, nMode)) == NULL) {  
		perror("AggressiveVad_Create failed!");  
		return;  
	} 
	
	int sg = 0;

	AudioCache *pVoice = AudioCache_Create();

	bool isTriggered = false;
	int framesLen = 16 * frames;
	FILE* fp = NULL;  
	fp = fopen(pAudioFile, "rb");  

	fseek(fp, 0, SEEK_END);  
	unsigned int nLen = ftell(fp);  
	fseek(fp, 0, SEEK_SET);  
	short *shBufferIn = malloc(framesLen*sizeof(short));  
	while (1) {  
		memset(shBufferIn, 0x00, framesLen);
		if (framesLen != fread(shBufferIn, 2, framesLen, fp))  
			break;  

		int nRet = AggressiveVad_Process(pVad, shBufferIn, framesLen, 0.8f);   
		if(nRet == 1) {
			if(isTriggered == false) {
				isTriggered = true;
				printf("www have voice vad start========\n");
			}
			AudioCache_Push(pVoice, (const int8_t *)shBufferIn, framesLen*sizeof(short), nRet);
			printf("www have voice\n");
		}
		else if(nRet == 0) {
			if(isTriggered) {
				FlushToFile("sg1", sg, AudioCache_Bytes(pVoice), AudioCache_DataSize(pVoice));
				AudioCache_Clear(pVoice);
				sg++;
				printf("www have none voice start+++++++++++++\n");
				isTriggered = false;
			}
			printf("www have none voice\n");	
		}
	}
	
	if(AudioCache_Length(pVoice) > 0) {
		FlushToFile("sg1", sg, AudioCache_Bytes(pVoice), AudioCache_DataSize(pVoice));
	}
	AudioCache_Free(pVoice);

	fclose(fp);
	AggressiveVad_Free(pVad);
}


int main(int argc, char **argv){
	
	// TestNs("./audio.pcm", "./noise_0.pcm", 0);
	// TestNs("./audio.pcm", "./noise_1.pcm", 1);
	// TestNs("./audio.pcm", "./noise_2.pcm", 2);
	// TestNsx("./audio.pcm", "./noisex_0.pcm", 0);
	// TestNsx("./audio.pcm", "./noisex_1.pcm", 1);
	// TestNsx("./audio.pcm", "./noisex_2.pcm", 2);

	// TestVAD("./noisex_2.pcm","./vad_0.pcm",16000, 0) ;
	// TestVAD("./noisex_2.pcm","./vad_1.pcm",16000, 1) ;
	// TestVAD("./noisex_2.pcm","./vad_2.pcm",16000, 2) ;
	// TestVAD("./noisex_2.pcm","./vad_3.pcm",16000, 3) ;

//	TestAggressiveVAD0("./audio.pcm","./aggressive_1.pcm",16000, 2);

	TestAggressiveVAD("./audio.pcm" ,16000, 2, 200);
	// TestAggressiveVAD("./audio.pcm","./aggressive_2.pcm",16000, 2);
	// TestAggressiveVAD("./audio.pcm","./aggressive_3.pcm",16000, 3);

	return 0;
}
