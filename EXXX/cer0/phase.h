//
//  phase.h
//  EXXX
//
//  Created by Alice Grace on 10/13/24.
//

#ifndef phase_h
#define phase_h

#include <stdio.h>

#include "constants.h"

float PhaseGetIncrement(float sampleRate, float frequency);
float PhaseAdvance(float phaseValue, float phaseIncrement);

#endif /* phase_h */
