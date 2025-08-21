#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef void(*func_ptr)(void);

void n(void)
{
  system("/bin/cat /home/user/level7/.pass");
}

void m(void)
{
  puts("Nope");
}

int main(int ac, char** av)
{
  char *arg;
  func_ptr *func;
  
  arg = malloc(64);
  func = malloc(4);
  *func = (void *)m;
  strcpy(arg, av[1]);
  (**func)();
  return 0;
}
