/*
 * FIG : Facility for Interactive Generation of figures
 * Copyright (c) 1995-2002 by T. Sato
 *
 * Any party obtaining a copy of these files is granted, free of charge, a
 * full and unrestricted irrevocable, world-wide, paid up, royalty-free,
 * nonexclusive right and license to deal in this software and documentation
 * files (the "Software"), including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense and/or sell copies of
 * the Software, and to permit persons who receive copies from any such
 * party to do so, with the only requirement being that the above copyright
 * and this permission notice remain intact.
 */

#include "fig.h"
#include "figx.h"
#include "resources.h"
#include "u_fonts.h"
#include "object.h"  /* for spell_canvas() */

/* replacement of Times_Roman_bits etc, for Japanese */
unsigned char Japanese_Times_Roman_bits[] = {
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0xfe,0x20,0xf8,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0xfe,0x67,0x00,0x00,0x00,0x00,0xc0,0x3f,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x7e,0x82,0xfe,0x8b,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x66,0x66,0x00,0x00,0x00,0x00,0x80,0x71,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x42,0x82,0x20,0x88,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x62,0x04,0x00,0x00,0x00,0x00,0x80,0x61,0x00,0x00,
   0x00,0x00,0x00,0x00,0x20,0x00,0x42,0x82,0xfc,0x89,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x60,0x00,0x00,0x00,0x00,0x00,0x80,0x61,0x00,
   0x00,0x00,0x00,0x00,0x00,0x20,0x00,0x42,0xfe,0x04,0xf9,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x60,0x70,0xce,0x18,0x3c,0x16,0x80,0x61,
   0x78,0x9c,0x31,0x3c,0xce,0x00,0x20,0x00,0x7e,0x82,0x04,0x89,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x60,0x60,0xec,0x3d,0x66,0x19,0x80,
   0x31,0xcc,0xd8,0x7b,0x26,0xec,0x01,0x20,0x00,0x42,0x82,0xfc,
   0x89,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x60,0x60,0x9c,0x33,0x43,0x13,
   0x80,0x1f,0x86,0x39,0x67,0x66,0x9c,0x01,0xfe,0x03,0x42,0x82,
   0x04,0x89,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x60,0x60,0x8c,0x31,0x7f,
   0x07,0x80,0x1d,0x86,0x19,0x63,0x70,0x8c,0x01,0x20,0x00,0x42,
   0xfe,0x04,0xf9,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x60,0x60,0x8c,0x31,
   0x03,0x8e,0x8f,0x19,0x86,0x19,0x63,0x6c,0x8c,0x01,0x20,0x00,
   0x42,0x82,0xfc,0x89,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x60,0x60,0x8c,
   0x31,0x03,0x1c,0x80,0x31,0x86,0x19,0x63,0x66,0x8c,0x01,0x20,
   0x00,0x7e,0x82,0x20,0x88,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x60,0x60,
   0x8c,0x31,0x03,0x19,0x80,0x61,0x86,0x19,0x63,0x66,0x8c,0x01,
   0x20,0x00,0x00,0x83,0xfe,0x8b,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x60,
   0x60,0x8c,0x31,0x66,0x13,0x80,0xc1,0xcc,0x18,0x63,0x7e,0x8c,
   0x01,0x00,0x00,0x00,0x81,0x20,0x8c,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0xf0,0xf0,0xde,0x7b,0x3c,0x0d,0xc0,0xc3,0x79,0xbc,0xf7,0xcc,
   0x9e,0x03,0x00,0x00,0x80,0x81,0x20,0x84,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0xe0,0xe0,0x20,0xc6,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

unsigned char Japanese_Roman_bits[] = {
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x01,0x00,0x80,0x7f,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0xf0,0x07,0xc1,0x07,0x00,0xe3,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0xf0,0x13,0xf4,0x5f,0x04,0x00,0xc3,
   0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x10,0x12,0x04,0x41,0x04,
   0x00,0xc3,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x10,0x12,0xe4,
   0x4f,0x04,0x00,0xc3,0xf0,0x38,0x63,0xf0,0x38,0x03,0x08,0x10,
   0xf2,0x27,0xc8,0x07,0x00,0x63,0x98,0xb1,0xf7,0x98,0xb1,0x07,
   0x08,0xf0,0x13,0x24,0x48,0x04,0x00,0x3f,0x0c,0x73,0xce,0x98,
   0x71,0x86,0xff,0x10,0x12,0xe4,0x4f,0x04,0x00,0x33,0x0c,0x33,
   0xc6,0xc0,0x31,0x06,0x08,0x10,0x12,0x24,0x48,0x04,0x00,0x63,
   0x0c,0x33,0xc6,0xb0,0x31,0x06,0x08,0x10,0xf2,0x27,0xc8,0x07,
   0x00,0xc3,0x0c,0x33,0xc6,0x98,0x31,0x06,0x08,0x10,0x12,0xe4,
   0x4f,0x04,0x00,0xc3,0x0c,0x33,0xc6,0x98,0x31,0x06,0x08,0xf0,
   0x13,0x04,0x41,0x04,0x00,0x83,0x99,0x31,0xc6,0xf8,0x31,0x06,
   0x00,0x00,0x18,0xf4,0x5f,0x04,0x80,0x87,0xf3,0x78,0xef,0x31,
   0x7b,0x0e,0x00,0x00,0x08,0x04,0x61,0x04,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x0c,0x04,0x21,0x04,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x07,0x31,0x06,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00};

unsigned char Japanese_Times_Bold_bits[] = {
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x0a,0x00,0x00,0x00,0x00,0x08,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x0a,0x00,0x00,0x00,0x00,0x08,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xf0,
   0x7f,0x0e,0x00,0x00,0x00,0x00,0xe0,0x0f,0x00,0x1e,0x78,0x00,
   0x00,0x00,0x00,0x00,0x03,0x00,0x00,0x00,0x08,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x30,0x67,0x0e,0x00,0x00,0x00,0x00,0xc0,0x39,0x00,0x1c,0x70,
   0x00,0x08,0xc0,0xff,0x03,0x06,0x00,0x00,0x00,0xfc,0x01,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x10,0x47,0x0e,0x00,0x00,0x00,0x00,0xc0,0x71,0x00,0x1c,
   0x70,0x00,0x08,0x00,0x00,0x02,0x04,0x00,0x00,0x00,0x04,0x01,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x07,0x00,0x00,0x00,0x00,0x00,0xc0,0x71,0x00,
   0x1c,0x70,0x00,0x08,0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x06,
   0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x07,0xef,0x9d,0x83,0xc7,0x07,0xc0,0x71,
   0x3c,0x1c,0x76,0x00,0x08,0x00,0x00,0xc2,0x00,0x04,0x04,0x00,
   0x83,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x07,0xce,0x7b,0xc7,0x6d,0x06,0xc0,
   0x39,0x66,0x1c,0x7f,0x80,0xff,0x00,0x00,0x82,0x01,0x86,0x0c,
   0x81,0x81,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xce,0x39,0xe7,0xec,0x04,
   0xc0,0x0f,0xe7,0x9c,0x73,0x00,0x08,0x00,0x00,0x02,0x01,0x83,
   0x09,0x01,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xce,0x39,0xe7,0xef,
   0xe1,0xc7,0x39,0xe7,0x9c,0x73,0x00,0x08,0x00,0x00,0x02,0x80,
   0x01,0x81,0x01,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xce,0x39,0xe7,
   0xc0,0xe3,0xc7,0x71,0xe7,0x9c,0x73,0x00,0x08,0x00,0x00,0x02,
   0xc0,0x00,0x80,0x00,0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xce,0x39,
   0xe7,0x80,0x07,0xc0,0x71,0xe7,0x9c,0x73,0x00,0x08,0x00,0x00,
   0x02,0x60,0x00,0xc0,0x00,0x30,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xce,
   0x39,0xe7,0x20,0x07,0xc0,0x71,0xe7,0x9c,0x73,0x00,0x00,0x00,
   0x00,0x02,0x38,0x00,0x60,0x00,0x18,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,
   0xce,0x39,0xc7,0x6d,0x06,0xc0,0x39,0x66,0x1c,0x77,0x00,0x00,
   0xc0,0xff,0x03,0x0e,0x00,0x30,0x00,0x0e,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,
   0x0f,0xff,0x7b,0x8f,0xe7,0x03,0xe0,0x1f,0x3c,0x3e,0xee,0x00,
   0x00,0x00,0x00,0xc0,0x03,0x00,0x18,0x80,0x03,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0e,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

unsigned char Japanese_Bold_bits[] = {
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40,
   0x01,0x00,0x00,0x00,0x00,0x01,0xf8,0x07,0x80,0x07,0x1e,0x00,
   0x00,0x40,0x01,0x00,0x00,0x00,0x00,0x01,0x70,0x0e,0x00,0x07,
   0x1c,0x00,0x00,0x00,0x60,0x00,0x00,0x00,0x00,0x01,0x70,0x1c,
   0x00,0x07,0x1c,0x10,0xf8,0x7f,0xc0,0x00,0x00,0x00,0x80,0x3f,
   0x70,0x1c,0x00,0x07,0x1c,0x10,0x00,0x40,0x80,0x00,0x00,0x00,
   0x80,0x20,0x70,0x1c,0x0f,0x87,0x1d,0x10,0x00,0x40,0x00,0x00,
   0x00,0x00,0xc0,0x20,0x70,0x8e,0x19,0xc7,0x1f,0x10,0x00,0x40,
   0x18,0x80,0x80,0x00,0x60,0x30,0xf0,0xc7,0x39,0xe7,0x1c,0xff,
   0x01,0x40,0x30,0xc0,0x90,0x21,0x30,0x10,0x70,0xdc,0x39,0xe7,
   0x1c,0x10,0x00,0x40,0x20,0x60,0x30,0x21,0x00,0x18,0x70,0xf8,
   0x39,0xe7,0x1c,0x10,0x00,0x40,0x00,0x30,0x20,0x30,0x00,0x08,
   0x70,0xf8,0x39,0xe7,0x1c,0x10,0x00,0x40,0x00,0x18,0x00,0x10,
   0x00,0x0c,0x70,0xf8,0x39,0xe7,0x1c,0x10,0x00,0x40,0x00,0x0c,
   0x00,0x18,0x00,0x06,0x70,0x9c,0x19,0xc7,0x1d,0x00,0x00,0x40,
   0x00,0x07,0x00,0x0c,0x00,0x03,0xf8,0x07,0x8f,0x8f,0x3b,0x00,
   0xf8,0x7f,0xc0,0x01,0x00,0x06,0xc0,0x01,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x78,0x00,0x00,0x03,0x70,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xc0,0x01,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00};

/* replacement of Times_Roman_bits etc, for Korean (from Jeon Hyoung-Jo) */
unsigned char Korean_Times_Roman_bits[] = {
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0xfc,0xcf,0x00,0x00,0x00,0x00,0x80,0x7f,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x60,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0xcc,0xcc,0x00,0x00,0x00,0x00,0x00,0xe3,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0xfc,0x61,0xe0,0x0f,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0xc4,0x08,0x00,0x00,0x00,0x00,0x00,0xc3,0x00,0x00,
   0x00,0x00,0x00,0x00,0x40,0x00,0x98,0x7f,0x00,0x07,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0xc3,0x00,
   0x00,0x00,0x00,0x00,0x00,0x40,0x00,0x98,0x61,0x80,0x01,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0xc0,0xe0,0x9c,0x31,0x78,0x2c,0x00,0xc3,
   0xf0,0x38,0x63,0x78,0x9c,0x01,0x40,0x00,0x98,0x7f,0xc0,0x1e,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0xc0,0xc0,0xd8,0x7b,0xcc,0x32,0x00,
   0x63,0x98,0xb1,0xf7,0x4c,0xd8,0x03,0x40,0x00,0xf8,0x61,0x60,
   0x38,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0xc0,0xc0,0x38,0x67,0x86,0x26,
   0x00,0x3f,0x0c,0x73,0xce,0xcc,0x38,0x03,0xfc,0x07,0x08,0x20,
   0x30,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xc0,0xc0,0x18,0x63,0xfe,
   0x0e,0x00,0x3b,0x0c,0x33,0xc6,0xe0,0x18,0x03,0x40,0x00,0x00,
   0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xc0,0xc0,0x18,0x63,
   0x06,0x1c,0x1f,0x33,0x0c,0x33,0xc6,0xd8,0x18,0x03,0x40,0x00,
   0x00,0x3e,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xc0,0xc0,0x18,
   0x63,0x06,0x38,0x00,0x63,0x0c,0x33,0xc6,0xcc,0x18,0x03,0x40,
   0x00,0x00,0x63,0x00,0x63,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xc0,0xc0,
   0x18,0x63,0x06,0x32,0x00,0xc3,0x0c,0x33,0xc6,0xcc,0x18,0x03,
   0x40,0x00,0x00,0x63,0xfc,0xff,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xc0,
   0xc0,0x18,0x63,0xcc,0x26,0x00,0x83,0x99,0x31,0xc6,0xfc,0x18,
   0x03,0x00,0x00,0x00,0x63,0x08,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0xe0,0xe1,0xbd,0xf7,0x78,0x1a,0x80,0x87,0xf3,0x78,0xef,0x99,
   0x3d,0x07,0x00,0x00,0x00,0x3e,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

unsigned char Korean_Roman_bits[] = {
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0xc0,0x00,0x00,0x00,0x00,0xc6,0x01,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x01,0x00,0x00,0x00,0x86,
   0x01,0x00,0x00,0x00,0x00,0x00,0x10,0xf0,0x87,0x81,0x3f,0x00,
   0x00,0x86,0x01,0x00,0x00,0x00,0x00,0x00,0x10,0x60,0xfe,0x01,
   0x1c,0x00,0x00,0x86,0xe1,0x71,0xc6,0xe0,0x71,0x06,0x10,0x60,
   0x86,0x01,0x06,0x00,0x00,0xc6,0x30,0x63,0xef,0x31,0x63,0x0f,
   0x10,0x60,0xfe,0x01,0x7b,0x00,0x00,0x7e,0x18,0xe6,0x9c,0x31,
   0xe3,0x0c,0xff,0xe1,0x87,0x81,0xe1,0x00,0x00,0x66,0x18,0x66,
   0x8c,0x81,0x63,0x0c,0x10,0x20,0x80,0x40,0x04,0x00,0x00,0xc6,
   0x18,0x66,0x8c,0x61,0x63,0x0c,0x10,0x00,0x00,0x00,0x04,0x00,
   0x00,0x86,0x19,0x66,0x8c,0x31,0x63,0x0c,0x10,0x00,0xf8,0x00,
   0x04,0x00,0x00,0x86,0x19,0x66,0x8c,0x31,0x63,0x0c,0x10,0x00,
   0x8c,0x01,0x8c,0x01,0x00,0x06,0x33,0x63,0x8c,0xf1,0x63,0x0c,
   0x00,0x00,0x8c,0xf1,0xff,0x03,0x00,0x0f,0xe7,0xf1,0xde,0x63,
   0xf6,0x1c,0x00,0x00,0x8c,0x21,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0xf8,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00};

unsigned char Korean_Times_Bold_bits[] = {
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0xfe,0xcf,0x01,0x00,0x00,0x00,0x00,0xfc,0x01,0xc0,0x03,
   0x0f,0x00,0x00,0x00,0x00,0xe0,0x0f,0x01,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0xe6,0xcc,0x01,0x00,0x00,0x00,0x00,0x38,0x07,0x80,
   0x03,0x0e,0x00,0x01,0xe0,0xff,0x20,0x00,0x01,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0xe2,0xc8,0x01,0x00,0x00,0x00,0x00,0x38,0x0e,
   0x80,0x03,0x0e,0x00,0x01,0x00,0x80,0x20,0x00,0x01,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0xe0,0x00,0x00,0x00,0x00,0x00,0x00,0x38,
   0x0e,0x80,0x03,0x0e,0x00,0x01,0x00,0x80,0x20,0x00,0x01,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0xe0,0xe0,0xbd,0x73,0xf0,0xf8,0x00,
   0x38,0x8e,0x87,0xc3,0x0e,0x00,0x01,0x00,0x80,0x20,0x00,0x01,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0xe0,0xc0,0x79,0xef,0xb8,0xcd,
   0x00,0x38,0xc7,0x8c,0xe3,0x0f,0xf0,0x1f,0x00,0x80,0xe0,0x0f,
   0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xe0,0xc0,0x39,0xe7,0x9c,
   0x9d,0x00,0xf8,0xe1,0x9c,0x73,0x0e,0x00,0x01,0x00,0x80,0x00,
   0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xe0,0xc0,0x39,0xe7,
   0xfc,0x3d,0xfc,0x38,0xe7,0x9c,0x73,0x0e,0x00,0x01,0x00,0x04,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xe0,0xc0,0x39,
   0xe7,0x1c,0x78,0xfc,0x38,0xee,0x9c,0x73,0x0e,0x00,0x01,0x00,
   0x04,0x80,0xff,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xe0,0xc0,
   0x39,0xe7,0x1c,0xf0,0x00,0x38,0xee,0x9c,0x73,0x0e,0x00,0x01,
   0x00,0x04,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xe0,
   0xc0,0x39,0xe7,0x1c,0xe4,0x00,0x38,0xee,0x9c,0x73,0x0e,0x00,
   0x00,0xf0,0xff,0x01,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0xe0,0xc0,0x39,0xe7,0xb8,0xcd,0x00,0x38,0xc7,0x8c,0xe3,0x0e,
   0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0xf0,0xe1,0x7f,0xef,0xf1,0x7c,0x00,0xfc,0x83,0xc7,0xc7,
   0x1d,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

unsigned char Korean_Bold_bits[] = {
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0x00,0xf0,0xc0,
   0x03,0x00,0x00,0x00,0x00,0x80,0x00,0x00,0x00,0x00,0xce,0x01,
   0xe0,0x80,0x03,0x00,0x00,0x00,0xf0,0x87,0x00,0x00,0x00,0x00,
   0x8e,0x03,0xe0,0x80,0x03,0x08,0xf0,0x7f,0x10,0x80,0x00,0x00,
   0x00,0x00,0x8e,0x03,0xe0,0x80,0x03,0x08,0x00,0x40,0x10,0x80,
   0x00,0x00,0x00,0x00,0x8e,0xe3,0xe1,0xb0,0x03,0x08,0x00,0x40,
   0x10,0x80,0x00,0x00,0x00,0x00,0xce,0x31,0xe3,0xf8,0x03,0x08,
   0x00,0x40,0x10,0x80,0x00,0x00,0x00,0x00,0xfe,0x38,0xe7,0x9c,
   0x83,0xff,0x00,0x40,0xf0,0x87,0x00,0x00,0x00,0x00,0x8e,0x3b,
   0xe7,0x9c,0x03,0x08,0x00,0x40,0x00,0x80,0x00,0x00,0x00,0x00,
   0x0e,0x3f,0xe7,0x9c,0x03,0x08,0x00,0x40,0x00,0x00,0x00,0x00,
   0x00,0x00,0x0e,0x3f,0xe7,0x9c,0x03,0x08,0x00,0x02,0xc0,0xff,
   0x00,0x00,0x00,0x00,0x0e,0x3f,0xe7,0x9c,0x03,0x08,0x00,0x02,
   0x00,0x80,0x00,0x00,0x00,0x00,0x8e,0x33,0xe3,0xb8,0x03,0x00,
   0x00,0x02,0x00,0x80,0x00,0x00,0x00,0x00,0xff,0xe0,0xf1,0x71,
   0x07,0x00,0xf8,0xff,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00};


/* check if str include i18n character */
static Boolean is_i18n_text(str)
     char *str;
{
  int i;
  if (appres.always_use_fontset) return TRUE;
  for (i = 0; str[i] != '\0'; i++) {
    if (str[i] & 0x80) return TRUE;
  }
  return FALSE;
}

/* get fontset and size corresponding to font specified by fid */
/* return FALSE if the font is not for i18n */
static Boolean seek_fontset(fid, fontset, size_ret)
     Font fid;
     XFontSet *fontset;
     int *size_ret;
{
  extern struct _xfstruct x_fontinfo[]; /* X11 fontnames */
  struct xfont *nf;
  int i;
  if (!appres.international) return FALSE;
  for (i = 0; i < NUM_FONTS; i++) {
    nf = x_fontinfo[i].xfontlist;
    while (nf != NULL && (nf->fset == NULL || nf->fstruct->fid != fid))
      nf = nf->next;
    if (nf != NULL && nf->fset != NULL) {
      *fontset=nf->fset;
      *size_ret = nf->size;
      return TRUE;
    }
  }
  return FALSE;
}

/* check if the font is for i18n */
Boolean is_i18n_font(font)
     XFontStruct *font;
{
  XFontSet fontset;
  int font_size;
  return seek_fontset(font->fid, &fontset, &font_size);
}

int
i18n_fontset_height(fontset)
     XFontSet fontset;
{
  XFontSetExtents *extents;
  extents = XExtentsOfFontSet(fontset);
  return extents->max_logical_extent.height;
}


/* get extents of the text */
void
i18n_text_extents(font, str, len, dir, asc, des, overall)
     XFontStruct *font;
     char *str;
     int len;
     int *dir, *asc, *des;  /* assume these return values are not used */
     XCharStruct *overall;
{
  XFontSet fontset;
  int font_size;
  XRectangle ink, logical;
  double scale;
  if (!appres.international || !is_i18n_text(str)
      || !seek_fontset(font->fid, &fontset, &font_size)) {
    XTextExtents(font, str, len, dir, asc, des, overall);
  } else {
    XmbTextExtents(fontset, str, len, &ink, &logical);
    if (appres.fontset_size <= 0)
      scale = (double)font_size / (double)i18n_fontset_height(fontset);
    else
      scale = (double)font_size / (double)appres.fontset_size;
    overall->width = max2(logical.width, ink.width + ink.x) * scale + 0.5;
    overall->lbearing = -ink.x * scale;
    overall->rbearing = overall->width;
    overall->ascent = -logical.y * scale;
    overall->descent = logical.height * scale - overall->ascent;
  }
}

/* functions for bitmap scaling (AllocTextBitmapAndScaleBuf, ScaleUp, */
/* and ScaleDown) are derived from tgif-2.13 */

static int text_bitmap_width = -1;
static int text_bitmap_height = -1;
static Pixmap text_bitmap = None;
static char *scale_buf = NULL;
static GC depth_one_gc = None;

static void AllocTextBitmapAndScaleBuf(display, d, width, height)
     Display *display;
     Drawable d;
     int width;
     int height;
{
  if (height > text_bitmap_height || width > text_bitmap_width) {
    if (0 < text_bitmap_height && 0 < text_bitmap_width) {
      XFreePixmap(display, text_bitmap);
      free(scale_buf);
    }

    if (height < 1) height = 1;
    if (width < 1) width = 1;
    text_bitmap_height = height;
    text_bitmap_width = width;
    text_bitmap = XCreatePixmap(display, d,
		       text_bitmap_width, text_bitmap_height, 1);
    scale_buf = (char *)malloc(text_bitmap_height * sizeof(char));
  }
}

static void ScaleUp(from_image, to_image, width, height, scale)
     XImage *from_image;
     XImage *to_image;
     int width;
     int height;
     double scale;
{
  int wd = width * scale;
  int ht = height * scale;
  register int i, j;
  register int x1, y1, x2, y2;

  i = -1;
  for (x2 = 0; x2 < wd; x2++) {
    x1 = x2 / scale;
    if (x1 != i) {
      j = -1;
      for (y2 = 0; y2 < ht; y2++) {
	y1 = y2 / scale;
	if (y1 != j) {
	  if (scale_buf[y2] = XGetPixel(from_image, x1, y1))
	    XPutPixel(to_image, x2, y2, 1);
	  j = y1;
	} else {
	  if (scale_buf[y2] = scale_buf[y2 - 1])
	    XPutPixel(to_image, x2, y2, 1);
	}
      }
      i = x1;
    } else {
      for (y2 = 0; y2 < ht; y2++) {
	if (scale_buf[y2] == 1)
	  XPutPixel(to_image, x2, y2, 1);
      }
    }
  }
}

static void ScaleDown(from_image, to_image, width, height, scale)
     XImage *from_image;
     XImage *to_image;
     int width;
     int height;
     double scale;
{
  int y1, y2;
  register int x1, x2, last_x2;

  for (y1 = 0; y1 < height; y1++) {
    y2 = y1 * scale;
    last_x2 = -1;
    for (x1 = 0; x1 < width; x1++) {
      x2 = x1 * scale;
      if (x2 != last_x2 && XGetPixel(from_image, x1, y1)) {
	XPutPixel(to_image, x2, y2, 1);
	last_x2 = x2;
      }
    }
  }
}

void i18n_draw_text();

/* draw text to drawable WITH background color */

void i18n_draw_image_string(dpy, drawable, gc, x, y, str, len)
     Display *dpy;
     Drawable drawable;
     GC gc;
     int x, y;
     char *str;
     int len;
{
  i18n_draw_text(dpy, drawable, gc, x, y, str, len, True);
}

/* draw text to drawable with no background */

void i18n_draw_string(dpy, drawable, gc, x, y, str, len)
     Display *dpy;
     Drawable drawable;
     GC gc;
     int x, y;
     char *str;
     int len;
{
  i18n_draw_text(dpy, drawable, gc, x, y, str, len, False);
}

void i18n_draw_text(dpy, drawable, gc, x, y, str, len, drawbg)
     Display	*dpy;
     Drawable	 drawable;
     GC		 gc;
     int	 x, y;
     char	 *str;
     int	 len;
     Boolean	 drawbg;
{
  static Pixmap paint_bitmap = None;
  static int paint_bitmap_width = -1;
  static int paint_bitmap_height = -1;
  static XImage *to_image = NULL;
  static char *to_image_data = NULL;
  static int to_image_size = -1;
  static GC my_gc = None;
  static Drawable last_drawable = None;
  static Font last_font = None;
  static char last_str[400] = "";
  static int width, lbearing, height, ascent;
  XImage *from_image;
  XFontSet fontset;
  int font_size;
  int base_width, base_height;
  XGCValues values;
  XRectangle ink, logical;
  double scale;

  XGetGCValues(dpy, gc, GCFont, &values);
  if (!appres.international || !is_i18n_text(str)
      || !seek_fontset(values.font, &fontset, &font_size)) {
    if (drawbg)
	XDrawImageString(dpy, drawable, gc, x, y, str, len);
    else
	XDrawString(dpy, drawable, gc, x, y, str, len);
  } else {
    if (drawable != last_drawable || values.font != last_font
	|| strncmp(str, last_str, sizeof(last_str)) != 0) {
      /* generate paint_bitmap only if it is needed */
      last_drawable = drawable;
      last_font = values.font;
      strncpy(last_str, str, sizeof(last_str));

      XmbTextExtents(fontset, str, len, &ink, &logical);
      base_width = ink.width + abs(ink.x) + 1;
      base_height = ink.height + 1;

      if (appres.fontset_size <= 0)
	scale = (double)font_size / (double)i18n_fontset_height(fontset);
      else
	scale = (double)font_size / (double)appres.fontset_size;
      width = base_width * scale + 0.5;
      lbearing = max2(-ink.x, 0) * scale + 0.5;
      height = base_height * scale + 0.5;
      ascent = -ink.y * scale + 0.5;
      AllocTextBitmapAndScaleBuf(dpy, drawable,
			 max2(base_width, width), max2(base_height, height));
      if (depth_one_gc == None) {
	depth_one_gc = XCreateGC(dpy, text_bitmap, (unsigned long)0, 0);
	XSetBackground(dpy, depth_one_gc, 0);
	XSetFillStyle(dpy, depth_one_gc, FillSolid);
	XSetFunction(dpy, depth_one_gc, GXcopy);
      }
      if (my_gc != None) XFreeGC(dpy, my_gc);
      my_gc = XCreateGC(dpy, drawable, (unsigned long)0, 0);
      if (to_image == NULL
	  || to_image->width < text_bitmap_width
	  || to_image->height < text_bitmap_height) {
	if (to_image != NULL) XDestroyImage(to_image);
	to_image_size = (unsigned)(text_bitmap_width/8 + 1) * text_bitmap_height;
	to_image_data = malloc(to_image_size);
	to_image = XCreateImage(dpy, DefaultVisual(dpy, DefaultScreen(dpy)),
				1, XYBitmap, 0, to_image_data,
				text_bitmap_width, text_bitmap_height, 8, 0);
      }
      if (paint_bitmap_width < width || paint_bitmap_height < height) {
	if (paint_bitmap != None) XFreePixmap(dpy, paint_bitmap);
	paint_bitmap = XCreatePixmap(dpy, drawable, width, height, 1);
	paint_bitmap_width = width;
	paint_bitmap_height = height;
      }

      XSetForeground(dpy, depth_one_gc, 0);
      XFillRectangle(dpy, text_bitmap, depth_one_gc, 0, 0,
		     text_bitmap_width, text_bitmap_height);
      XSetForeground(dpy, depth_one_gc, 1);
      if (drawbg)
          XmbDrawImageString(dpy, text_bitmap, fontset, depth_one_gc,
		    max2(-ink.x, 0), -ink.y, str, len);
      else
          XmbDrawString(dpy, text_bitmap, fontset, depth_one_gc,
		    max2(-ink.x, 0), -ink.y, str, len);

      from_image = XGetImage(dpy, text_bitmap, 0, 0,
		     text_bitmap_width, text_bitmap_height, 1, ZPixmap);

      bzero(to_image_data, to_image_size);  /* clear to_image */
      if (1.0 < scale) {  /* scale up */
	ScaleUp(from_image, to_image, base_width, base_height, scale);
      } else {  /* scale down */
	ScaleDown(from_image, to_image, base_width, base_height, scale);
      }
      XDestroyImage(from_image);

      XPutImage(dpy, paint_bitmap, depth_one_gc, to_image, 0, 0, 0, 0, width, height);
    }

    /* paint text using stipple technique */
    XCopyGC(dpy, gc, GCForeground|GCBackground|GCFunction|GCPlaneMask, my_gc);
    XSetFillStyle(dpy, my_gc, FillStippled);
    XSetStipple(dpy, my_gc, paint_bitmap);
    XSetTSOrigin(dpy, my_gc, x - lbearing, y - ascent);
    XFillRectangle(dpy, drawable, my_gc, x - lbearing, y - ascent, width, height);
  }
}

#ifndef HAVE_SETLOCALE

char *setlocale(category, locale)
     int category;
     const char *locale;
{
  static char old_locale[100] = "C";
  static char cur_locale[100] = "C";
  const char *s;
  if (locale == NULL) {
    return cur_locale;
  } else if (category == LC_ALL) {
    strcpy(old_locale, cur_locale);
    if (locale[0] == '\0') {
      s = getenv("LANG");
      if (s == NULL) s = "C";  /* LANG not defined */
    } else {
      s = locale;
    }
    strncpy(cur_locale, s, sizeof(cur_locale) - 1);
    return old_locale;
  } else {
    return cur_locale;
  }
}
#endif  /* HAVE_SETLOCALE */