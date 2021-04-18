#include "types.h"
#include "fcntl.h"
#include "types.h"
#include "user.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"

int main(){
  int shmid = shmget(100, 10000, IPC_CREAT|IPC_EXCL|0666);
  if(shmid < 0){
    printf(1, "error\n");
    exit();
  }
  else
    printf(1, "%d\n", shmid);
  char * shm = shmat(shmid, (void *)0, 0);
  if(shm < 0)
    printf(1, "error\n");
  else
    printf(1, "%d\n", shm);
  char * s = shm;
  for (char c = 'A'; c <= 'Z'; c++)
    *s++ = c;
  *s = 0;
  for (s = shm; *s != 0; s++)
    printf(1, "%c", *s);
  printf(1, "\n");
  exit();
}
