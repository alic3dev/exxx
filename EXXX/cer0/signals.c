//
//  signals.c
//  EXXX
//
//  Created by Alice Grace on 10/13/24.
//

#include "signals.h"

size_t SIGNAL_COUNT = 6;

float SignalSine(float phase) { return sin(phase); }

float SignalWhiteNoise(float phase) {
  return ((float)(arc4random_uniform(__UINT32_MAX__)) / (float)(UINT32_MAX)) *
             2.0f -
         1.0f;
}

float SignalSawtoothUp(float phase) {
  return 1.0f - 2.0f * (phase * (1.0f / TWO_PI));
}

float SignalSawtoothDown(float phase) {
  return (2.0f * (phase * (1.0f / TWO_PI))) - 1.0f;
}

float SignalSquare(float phase) {
  if (phase <= M_PI) {
    return 1.0f;
  } else {
    return -1.0f;
  }
}

float SignalTriangle(float phase) {
  float value = (2.0f * (phase * (1.0f / TWO_PI))) - 1.0f;

  if (value < 0.0f) {
    value = -value;
  }

  return 2.0f * (value - 0.5f);
}

char *SignalNameLookup[6] = {"Sine",        "Square",        "Triangle",
                             "Sawtooth Up", "Sawtooth Down", "White Noise"};

SignalFunction SignalFunctionLookup[6] = {SignalSine,         SignalSquare,
                                          SignalTriangle,     SignalSawtoothUp,
                                          SignalSawtoothDown, SignalWhiteNoise};
