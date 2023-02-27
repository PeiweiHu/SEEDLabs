#include <stdio.h>
#include <stdlib.h>

int main(void) {
  char arr[64];
  char *ptr;

  ptr = getenv("PWD");
  if (ptr != NULL) {
    sprintf(arr, "Present working directory is: %s\n", ptr);
    printf("%s", arr);
  }
  return 0;
}
