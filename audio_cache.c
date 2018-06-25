#include <string.h>
#include "audio_cache.h"

//队列的结点结构
typedef struct Node{
    int8_t *data;
    size_t len;
    bool   isVoice;
    struct Node *next;
} Node;

//队列的结构，嵌套
struct AudioCacheT {
    Node *front;
    Node *rear;
    int size;
};

void free_node(Node *node) {
    if(node->data) free(node->data);
    free(node);
}

AudioCache *AudioCache_Create(void) {
    AudioCache *self = malloc(sizeof(struct AudioCacheT));
    Node *node = (Node *)malloc(sizeof(Node));
    node->data = NULL;

    self->front = self->rear = node;
    self->front->next = NULL;
    self->size = 0;

    return self;
}

void AudioCache_Free(AudioCache *self) {
    while (self->front != NULL) {
        self->rear = self->front->next;
        free_node(self->front);
        self->front = self->rear;
    }
    self->size = 0;
    free(self);
}

void AudioCache_Clear(AudioCache *self) {
    while (self->front != NULL) {
        self->rear = self->front->next;
        free_node(self->front);
        self->front = self->rear;
    }

    Node *node = (Node *)malloc(sizeof(Node));
    node->data = NULL;

    self->front = self->rear = node;
    self->front->next = NULL;
    self->size = 0;
}

int AudioCache_IsEmpty(AudioCache *self) {
    return self->rear == self->front ? 1 : 0;
}

void AudioCache_Push(AudioCache *self, const int8_t* data, const size_t frame_length, int is_voice) {
    Node *q = (Node *)malloc(sizeof(Node));
    
    int8_t *audio_data = malloc(frame_length);
    memcpy(audio_data, data, frame_length);
    //插入数据
    q->data = audio_data;
    q->len = frame_length;
    q->isVoice = is_voice;
    q->next = NULL;
    //rear 总是指向队尾元素
    self->rear->next = q;
    self->rear = q;
    self->size++;
}

//出队，需要判空
void AudioCache_Pop(AudioCache *self) {
    Node *q = NULL;
    
    if (!AudioCache_IsEmpty(self)) {
        q = self->front->next;
        self->front->next = q->next;
        //这句很关键，不能丢
        if (self->rear == q) {
            self->rear = self->front;
        }
        free_node(q);
        self->size--;
    }
}

int AudioCache_Length(AudioCache *self) {
    return self->size;
}

int AudioCache_DataSize(AudioCache *self) {
    int length =  0;
    Node *q = self->front->next;
    
    while (q != NULL) {
        length = length + q->len;
        q = q->next;
    }

    return length;
}

int8_t *AudioCache_Bytes(AudioCache *self) {
    int length = AudioCache_DataSize(self);
    int8_t *data = malloc(length);
    
    Node *q = self->front->next;
    int i = 0;
    while (q != NULL) {
        memcpy(data + i, q->data, q->len);
        i = i + q->len;
        q = q->next;
    }

    return data;
}

int AudioCache_IsVoice(AudioCache *self, float scale) {
    Node *q = self->front->next;
    int len = 0;
    int voice = 0;
    while (q != NULL) {
        if(q->isVoice == 1) {
            voice++;
        }
        q = q->next;
        len++;
    }

    if(voice > (len * scale)) {
        return 1;
    }

    return 0;
}

int AudioCache_IsNoneVoice(AudioCache *self, float scale) {
    Node *q = self->front->next;
    int len = 0;
    int noneVoice = 0;
    while (q != NULL) {
        if(q->isVoice == 0) {
            noneVoice++;
        }
        q = q->next;
        len++;
    }

    if(noneVoice > (len * scale)) {
        return 1;
    }

    return 0;
}