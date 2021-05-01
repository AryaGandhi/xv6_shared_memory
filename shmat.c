#include "types.h"
#include "fcntl.h"
#include "types.h"
#include "user.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"

int main(){
  /*int shmid = shmget(1000, 1000, 0444);
  if(shmid < 0){
    printf(1, "error\n");
    exit();
  }
  else
    printf(1, "%d\n", shmid);
  char * shm = shmat(shmid, (void *)0, 0);
  if(!shm){
    printf(1, "error\n");
    exit();
  }
  else
    printf(1, "%p\n", shm);
  char * s = shm;
  for (s = shm; *s != 0; s++)
    printf(1, "%c", *s);
  printf(1, "\n");
  int pid = fork();
  if(pid == 0){
    for (char c = 'A'; c <= 'Q'; c++)
      *s++ = c;
    *s = 0;
  }
  if(pid > 0){
    wait();
    for (s = shm; *s != 0; s++)
      printf(1, "%c", *s);
    printf(1, "\n");
  }*/
  /*for (char * s = shm; *s != 0; s++)
    printf(1, "%c", *s);
  printf(1, "\n");*/
  int shmid = shmget(101, 1000, 0666);
  if(shmid < 0){
    printf(1, "error\n");
    exit();
  }
  else
    printf(1, "%d\n", shmid);
  char * shm = shmat(shmid, (void *)0, 0);
  if(!shm){
    printf(1, "error\n");
    exit();
  }
  else
    printf(1, "%d\n", shm);
  for (char * s = shm; *s != 0; s++)
    printf(1, "%c", *s);
  printf(1, "\n");
  exit();
}
