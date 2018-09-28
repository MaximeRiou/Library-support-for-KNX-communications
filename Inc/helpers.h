//*****************************************************************************
// 
// Fichero: helpers.h
// Proposito:
//   Funciones auxiliares
// 
//*****************************************************************************

#ifndef __HELPERS_H
#define __HELPERS_H

#define FORMATTED_STRING_MAXLEN         32


char asHex(const char value);
unsigned int getStringLength(const char *str);
void copyString(const char *from, char buffer[], unsigned int buffer_length);
void copyBinString(const char *from, unsigned int length, char buffer[], unsigned int buffer_length);

char *formatUnsignedInt(char buffer[], unsigned int buffer_length, unsigned int value, unsigned int formatted_length, char padding);
char *formatInt(char buffer[], unsigned int buffer_length, int value, unsigned int formatted_length, char padding);

#endif // __HELPERS_H
