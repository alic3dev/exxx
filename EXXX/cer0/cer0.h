//
//  cer0.h
//  EXXX
//
//  Created by Alice Grace on 10/13/24.
//

#ifndef cer0_h
#define cer0_h

#include <math.h>
#include <stdlib.h>

#include "constants.h"
#include "phase.h"
#include "signals.h"

float *createNoteTable(unsigned char startingOctave, unsigned char endingOctave,
                       float frequency);

size_t sizeofNoteTable(unsigned char startingOctave,
                       unsigned char endingOctave);

#endif /* cer0_h */
