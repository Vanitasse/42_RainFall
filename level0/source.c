#include <stdio.h>
#include <unistd.h>
#include <strings.h>
#include <sys/types.h>
#define _GNU_SOURCE


int main(int ac,char **av)

{
  int nb;
  char *cmd_args[2];
  __uid_t uid;
  __gid_t gid;
  
  nb = atoi(av[1]);
  if (nb == 423) {
    cmd_args[0] = strdup("/bin/sh");
    cmd_args[1] = NULL;
    gid = getegid();
    uid = geteuid();
    setresgid(gid,gid,gid);
    setresuid(uid,uid,uid);
    execv("/bin/sh", cmd_args);
  }
  else {
    fwrite("No !\n", 5, 1, stderr);
  }
  return 0;
}
