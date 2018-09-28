//*****************************************************************************
// 
// Fichero: helpers.c
// Proposito:
//   Funciones auxiliares
// 
//*****************************************************************************
#include <stdlib.h>
#include "helpers.h"

static char *_formatUInt(char buffer[], unsigned int buffer_length, unsigned int value, unsigned int formatted_length, char padding, unsigned int *padded);

char asHex(const char value) {
  if (value >= 16) {
    return '.';
  }
  if (value >= 10) {
    return 'A' + (value-10);
  }
  return '0' + value;
}

unsigned int getStringLength(const char *str) {
  unsigned int i;
  const char *p;
  
  if (str == NULL) {
    return 0;
  }
  for (p=str, i=0; *p != '\0'; p++, i++) {
  }
  return i;
}

void copyString(const char *from, char buffer[], unsigned int buffer_length) {
  unsigned int i = buffer_length;
  const char *f = from;
  char *t = buffer;
  
  if ((f == NULL) || (t == NULL) || (i == 0)) {
    return;
  }
  while ((i>0) && (*f != '\0')) {
    *t++ = *f++;
    i--;
  }
  if (i == 0) {
    t--;
  }
  *t = '\0';
  return;
}

void copyBinString(const char *from, unsigned int length, char buffer[], unsigned int buffer_length) {
  const char *f = from;
  char *t = buffer;
  
  if ((f == NULL) || (length == 0) || (t == NULL) || (buffer_length < length)) {
    return;
  }
  while (length>0) {
    *t++ = *f++;
    length--;
  }
  return;
}


static char *_formatUInt(char buffer[], unsigned int buffer_length, unsigned int value, unsigned int formatted_length, char padding, unsigned int *padded) {
  char str[FORMATTED_STRING_MAXLEN];
  int i;
  char *from, *to;

  if (value == 0) {
      i = 0;
      str[i++] = '0';
  } else {
    for (i=0; (i < FORMATTED_STRING_MAXLEN) && (value != 0); i++) {
      str[i] = '0' + (value % 10);
      value = value/10;
    }
  }
  if (formatted_length > i) {
    if (formatted_length > FORMATTED_STRING_MAXLEN) {
      formatted_length = FORMATTED_STRING_MAXLEN;
    }
    if (padded != NULL) {
      *padded = formatted_length - i;
    }
    for (; i < formatted_length; i++) {
      str[i] = padding;
    }
  }
  if (i > buffer_length) {
    return NULL;
  }
  for (from = &str[i-1], to = buffer; i > 0; i--) {
    *to++ = *from--;
  }
  return to;
}

char *formatUnsignedInt(char buffer[], unsigned int buffer_length, unsigned int value, unsigned int formatted_length, char padding) {
  if ((buffer == NULL) || (buffer_length == 0)) {
    return NULL;
  }
  return _formatUInt(buffer, buffer_length, value, formatted_length, padding, NULL);
}

char *formatInt(char buffer[], unsigned int buffer_length, int value, unsigned int formatted_length, char padding) {
  unsigned int padded = 0;
  char *result;

  if ((buffer == NULL) || (buffer_length == 0)) {
    return NULL;
  }
  if (value < 0) {
    buffer[0] = '-';
    result = _formatUInt(&buffer[1], buffer_length-1, (unsigned int)(-value), formatted_length-1, padding, &padded);
    if (padded > 0) {
      buffer[0] = padding;
      buffer[padded] = '-';
    }
    return result;
  } else {
    return _formatUInt(buffer, buffer_length, (unsigned int)value, formatted_length, padding, NULL);
  }
}
