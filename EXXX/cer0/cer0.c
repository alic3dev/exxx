//
//  cer0.c
//  EXXX
//
//  Created by Alice Grace on 10/13/24.
//

#include "cer0.h"

float *createNoteTable(unsigned char startingOctave, unsigned char endingOctave,
                       float frequency) {
  unsigned char octaveRange = endingOctave - startingOctave;

  static float *noteTable;

  noteTable = malloc(sizeof(float) * octaveRange * 12);

  for (unsigned char i = startingOctave; i <= endingOctave; ++i) {
    unsigned char octaveOffset = i * 12;

    for (signed char x = 0; x < 12; ++x) {
      float noteOffset = -57.0f + (float)(x);

      noteTable[octaveOffset + x] =
          pow(2.0f, (noteOffset + (float)(octaveOffset)) / 12.0f) * frequency;
    }
  }

  return noteTable;
}

size_t sizeofNoteTable(unsigned char startingOctave,
                       unsigned char endingOctave) {
  return (endingOctave - startingOctave) * 12;
}
