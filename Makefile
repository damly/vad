CC = gcc
CPP = g++
LINK = g++

LIBS = -lpthread -lm
#must add -fPIC option
CCFLAGS = $(COMPILER_FLAGS) -c -g -fPIC -DWEBRTC_POSIX
CPPFLAGS = $(COMPILER_FLAGS) -c -g -fPIC -std=c++11 -DWEBRTC_POSIX

TARGET = VadTest
USBTARGET = UsbTest

INCLUDES = -I./ -I./webrtc -I/usr/include/

CFILES = webrtc/modules/audio_processing/ns/noise_suppression.c \
		webrtc/modules/audio_processing/ns/nsx_core_c.c \
		webrtc/modules/audio_processing/ns/nsx_core.c \
		webrtc/modules/audio_processing/ns/ns_core.c \
		webrtc/modules/audio_processing/ns/noise_suppression_x.c \
		webrtc/common_audio/fft4g.c \
		webrtc/common_audio/vad/vad_core.c \
		webrtc/common_audio/vad/vad_gmm.c \
		webrtc/common_audio/vad/vad_sp.c \
		webrtc/common_audio/vad/vad_filterbank.c \
		webrtc/common_audio/vad/webrtc_vad.c \
		webrtc/common_audio/signal_processing/resample_by_2_internal.c \
		webrtc/common_audio/signal_processing/complex_fft.c \
		webrtc/common_audio/signal_processing/resample_48khz.c \
		webrtc/common_audio/signal_processing/spl_sqrt_floor.c \
		webrtc/common_audio/signal_processing/real_fft.c \
		webrtc/common_audio/signal_processing/downsample_fast.c \
		webrtc/common_audio/signal_processing/get_scaling_square.c \
		webrtc/common_audio/signal_processing/vector_scaling_operations.c \
		webrtc/common_audio/signal_processing/min_max_operations.c \
		webrtc/common_audio/signal_processing/complex_bit_reverse.c \
		webrtc/common_audio/signal_processing/division_operations.c \
		webrtc/common_audio/signal_processing/cross_correlation.c \
		webrtc/common_audio/signal_processing/copy_set_operations.c \
		webrtc/common_audio/signal_processing/spl_init.c \
		webrtc/common_audio/signal_processing/energy.c \
		webrtc/common_audio/signal_processing/resample_fractional.c \
		aggressive_vad.c \
		audio_cache.c \
		main.c

OBJFILE = $(CFILES:.c=.o)

USBFILES = 

OBJUSB = $(USBFILES:.c=.o)

%.o:%.c
	$(CC) -o $@ $(CCFLAGS) $< $(INCLUDES)

all:$(TARGET) $(USBTARGET)

#$(TARGET): $(OBJFILE)
#	$(LINK) $^ $(LIBS) -Wall -fPIC -shared -o $@

$(TARGET): $(OBJFILE)
	$(LINK) $^ $(LIBS) -Wall -fPIC -o $@

$(USBTARGET): $(OBJUSB)
	$(LINK) $^ $(LIBS) -Wall -fPIC -o $@

install:
	tsxs -i -o $(TARGET)

clean:
	rm -rf $(TARGET) $(USBTARGET)
	rm -rf $(OBJFILE) $(OBJUSB)

