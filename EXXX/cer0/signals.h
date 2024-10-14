//
//  signals.h
//  EXXX
//
//  Created by Alice Grace on 10/13/24.
//

#ifndef signals_h
#define signals_h

#include <math.h>
#include <stdlib.h>

#include "constants.h"

extern size_t SIGNAL_COUNT;

typedef float (*SignalFunction)(float phase);

float SignalSine(float phase);
float SignalWhiteNoise(float phase);
float SignalSawtoothUp(float phase);
float SignalSawtoothDown(float phase);
float SignalSquare(float phase);
float SignalTriangle(float phase);

extern char *SignalNameLookup[6];
extern SignalFunction SignalFunctionLookup[6];

#endif /* signals_h */
