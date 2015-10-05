//
// alerr.c -- error handling for use within the ades library.

#include <stdio.h>
#include <string.h>

char errLine[512];

int error(char *msg)
{
  strcpy(errLine, msg);
  return -1;
}

int error1(char *msg, char *arg)
{
  snprintf(errLine, sizeof errLine, msg, arg);
  return -1;
}
