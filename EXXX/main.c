//
//  main.c
//  EXXX
//
//  Created by Alice Grace on 10/12/24.
//

#include <CoreAudio/CoreAudio.h>

#include <IOKit/IOKitLib.h>
#include <IOKit/hid/IOHIDKeys.h>
#include <IOKit/hid/IOHIDManager.h>
#include <IOKit/usb/USBSpec.h>

#include <sys/param.h>

#include <math.h>
#include <paths.h>
#include <stdio.h>

#include "cer0/cer0.h"

static OSStatus ioProc(AudioObjectID, const AudioTimeStamp *,
                       const AudioBufferList *, const AudioTimeStamp *,
                       AudioBufferList *, const AudioTimeStamp *,
                       void *__nullable);
static AudioDeviceID getDefaultDevice(void);
static UInt32 cleanup(AudioDeviceID, AudioDeviceIOProcID);

const UInt32 CHANNEL_LEFT = 0;
const UInt32 CHANNEL_RIGHT = 1;

const float SAMPLE_RATE = 44100.0f;

const size_t CLIENT_DATA_SIZE = 5;
const unsigned short int CLIENT_DATA_FREQUENCY = 0;
const unsigned short int CLIENT_DATA_PHASE_VALUE = 1;
const unsigned short int CLIENT_DATA_AMPLITUDE = 2;
const unsigned short int CLIENT_DATA_VOLUME = 3;
const unsigned short int CLIENT_DATA_SIGNAL = 4;

int main(void) {
  float *noteTable =
      createNoteTable(OCTAVE_MINIMUM, OCTAVE_MAXIMUM, FREQUENCY_ROOT_STANDARD);
  size_t noteTableSize = sizeofNoteTable(OCTAVE_MINIMUM, OCTAVE_MAXIMUM);

  AudioDeviceID defaultDevice = getDefaultDevice();
  AudioDeviceIOProcID ioProcID;

  OSStatus didStartDevice = AudioDeviceStart(defaultDevice, NULL);
  if (didStartDevice != 0) {
    printf("Failed to start device [%i]: %i\n", defaultDevice, didStartDevice);
    return 1;
  }

  float clientData[CLIENT_DATA_SIZE];
  clientData[CLIENT_DATA_FREQUENCY] = noteTable[33];
  clientData[CLIENT_DATA_PHASE_VALUE] = 0.0f;
  clientData[CLIENT_DATA_AMPLITUDE] = 1.0f;
  clientData[CLIENT_DATA_VOLUME] = 0.4f;

  printf("Frequency: %f\n", clientData[CLIENT_DATA_FREQUENCY]);
  printf("Phase Value: %f\n", clientData[CLIENT_DATA_PHASE_VALUE]);
  printf("Amplitude: %f\n", clientData[CLIENT_DATA_AMPLITUDE]);
  printf("Volume: %f\n", clientData[CLIENT_DATA_VOLUME]);

  OSStatus didCreateIOProcID = AudioDeviceCreateIOProcID(
      defaultDevice, ioProc, (void *)(clientData), &ioProcID);

  if (didCreateIOProcID != 0) {
    printf("Failed to create IO Proc ID");
    return 1;
  }

  OSStatus didStartDeviceIOProc = AudioDeviceStart(defaultDevice, ioProcID);
  if (didStartDeviceIOProc != 0) {
    printf("Failed to start device IO Proc [%i]: %i\n", defaultDevice,
           didStartDeviceIOProc);
    return 1;
  }

  char userInput[10] = "";

  while (strcmp(userInput, "q") != 0 && strcmp(userInput, "Q") != 0) {
    printf("(Q to quit)> ");
    scanf("%s", userInput);

    if (userInput[0] == 's' || userInput[0] == 'S') {
      char slicedInput[9] = "";

      for (unsigned short int i = 0; i < 9; i++) {
        slicedInput[i] = userInput[i + 1];
      }

      int newSignal = atoi(slicedInput);

      if ((newSignal > 0 && newSignal < SIGNAL_COUNT) ||
          (newSignal == 0 && strcmp(slicedInput, "0") == 0)) {
        clientData[CLIENT_DATA_SIGNAL] = (float)newSignal;

        printf("Signal: %s\n", SignalNameLookup[newSignal]);
      } else {
        printf("Invalid Signal: %s\n", slicedInput);
      }
    } else {
      double newClientData = atof(userInput);

      if (newClientData != 0.0f || strcmp(userInput, "0") == 0 ||
          strcmp(userInput, "0.0") == 0) {
        if (newClientData == floorf(newClientData) && newClientData >= 0 &&
            newClientData < noteTableSize) {
          clientData[CLIENT_DATA_FREQUENCY] = noteTable[(int)(newClientData)];

          printf("Frequency: %f\n", clientData[CLIENT_DATA_FREQUENCY]);
        }
      }
    }

    printf("\n");
  }

  cleanup(defaultDevice, ioProcID);

  return 0;
}

static AudioDeviceID getDefaultDevice(void) {
  AudioObjectPropertyAddress aopa = {
      kAudioHardwarePropertyDefaultSystemOutputDevice,
      kAudioObjectPropertyScopeOutput, kAudioObjectPropertyElementMain};

  AudioDeviceID dev_id = kAudioDeviceUnknown;
  UInt32 dev_id_size = sizeof(dev_id);
  AudioObjectGetPropertyData(kAudioObjectSystemObject, &aopa, 0, NULL,
                             &dev_id_size, &dev_id);

  return dev_id;
}

static OSStatus ioProc(AudioObjectID inDevice, const AudioTimeStamp *inNow,
                       const AudioBufferList *inInputData,
                       const AudioTimeStamp *inInputTime,
                       AudioBufferList *outOutputData,
                       const AudioTimeStamp *inOutputTime,
                       void *__nullable inClientData) {
  float *clientData = inClientData;
  float frequency = clientData[CLIENT_DATA_FREQUENCY];
  float amplitude = clientData[CLIENT_DATA_AMPLITUDE];
  float volume = clientData[CLIENT_DATA_VOLUME];
  float phaseValue = clientData[CLIENT_DATA_PHASE_VALUE];
  float phaseIncrement = PhaseGetIncrement(SAMPLE_RATE, frequency);

  UInt32 bufferCount = outOutputData->mNumberBuffers;

  for (UInt32 bufferIndex = 0; bufferIndex < bufferCount; bufferIndex++) {
    AudioBuffer currentAudioBuffer = outOutputData->mBuffers[bufferIndex];

    float *outBuffer = currentAudioBuffer.mData;
    UInt32 outBufferSize = currentAudioBuffer.mDataByteSize / sizeof(float);

    UInt32 outChannelCount = currentAudioBuffer.mNumberChannels;

    for (UInt32 i = 0; i < outBufferSize; ++i) {
      UInt32 currentChannel = i % 2;

      float signalValue =
          SignalFunctionLookup[(int)clientData[CLIENT_DATA_SIGNAL]](phaseValue);

      outBuffer[i] = signalValue * amplitude * volume;

      if (currentChannel == outChannelCount - 1) {
        phaseValue = PhaseAdvance(phaseValue, phaseIncrement);

        clientData[CLIENT_DATA_PHASE_VALUE] = phaseValue;
      }
    }
  }

  return 0;
}

static UInt32 cleanup(AudioDeviceID device, AudioDeviceIOProcID ioProcID) {
  UInt32 result = 0;

  OSStatus didDestroyIOProcID = AudioDeviceDestroyIOProcID(device, ioProcID);
  if (didDestroyIOProcID != 0) {
    printf("Failed to destroy IO Proc ID: %i\n", didDestroyIOProcID);
    result++;
  }

  OSStatus didStopDeviceIOProc = AudioDeviceStop(device, ioProcID);
  if (didStopDeviceIOProc != 0) {
    printf("Failed to stop device IO Proc [%i]: %i\n", device,
           didStopDeviceIOProc);
    result++;
  }

  OSStatus didStopDevice = AudioDeviceStop(device, NULL);
  if (didStopDevice != 0) {
    printf("Failed to stop device [%i]: %i\n", device, didStopDevice);
    result++;
  }

  return result;
}
