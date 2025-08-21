#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *p(void)
{
  void *return_addr;
  char buffer[64];
  
  fflush(stdout);
  gets(buffer);
  return_addr = __builtin_return_address (0);
  if (((unsigned int)return_addr & 0xb0000000) == 0xb0000000) {
    printf("(%p)\n",return_addr);
    _exit(1);
  }
  puts(buffer);
  return (strdup(buffer));
}

int main(void)
{
  p();
  return 0;
}
