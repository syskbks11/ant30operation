 /**
  * test.c
  *
  */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum {MESSAGE_SIZE = 256}; /* 表示メッセージの最大長 */

int main(int argc, char **argv) {
 const char *DEFAULT_MSG = "Hello World!";
   char msg[MESSAGE_SIZE];
   int count = 1;
   int i;
 
  strncpy(msg, DEFAULT_MSG, MESSAGE_SIZE);
 
  switch (argc) {
  case 3:
  count = atoi(argv[2]);
  /* FALL THROUGH */
  case 2:
  strncpy(msg, argv[1], MESSAGE_SIZE);
  break;
  default:
  /* DO NOTHING */
  break;
   }
  
  for (i = 0; i < count; i++) {
  printf("%s\n", msg);
  }
  
  return 0;
   } 


