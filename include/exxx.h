#ifndef __exxx_h
#define __exxx_h

#include <stdio.h>

#include <CoreAudio/CoreAudio.h>

struct exxx_data_client {
  float* table_notes;
  unsigned int length_table_notes;
  float frequency;
  float value_phase;
  float amplitude;
  float volume;
  unsigned short int signal;
  unsigned long int frame;

  FILE* file;
  unsigned long int fileIndex;

  float* points;
  unsigned int length_points;
};

struct exxx_input_data {
  char* baseDir;

  char* boundary;
  char* parks;
  char* streams;
  char* contours;
  char* railroads;
  char* structures;
  char* lakes;
  char* roads;
  char* surfaces;
};

extern const char exxx_octave_maximum;
extern const char exxx_octave_minimum;

extern const unsigned int exxx_channel_left;
extern const unsigned int exxx_channel_right;

extern const float exxx_sample_rate;

extern struct exxx_input_data exxx_input_data;

int main(
  int,
  char**
);


void exxx_loop_input(
  struct exxx_data_client*,
  float*
);

struct exxx_data_client* exxx_data_client_initialize(
  float*,
  float*,
  unsigned int
);

AudioDeviceID exxx_device_default_get(void);

AudioDeviceIOProcID exxx_audio_initialize(
  AudioDeviceID,
  void*
);

OSStatus exxx_proc_io(
  AudioObjectID,
  const AudioTimeStamp*,
  const AudioBufferList*,
  const AudioTimeStamp*,
  AudioBufferList*,
  const AudioTimeStamp*,
  void*
);

unsigned int exxx_destroy(
  AudioDeviceID,
  AudioDeviceIOProcID
);

#endif
