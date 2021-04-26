#include "types.h"
#include "fcntl.h"
#include "types.h"
#include "user.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"

int main(){
  int shmid = shmget(101, 1000, 0666);
  if(shmid < 0){
    printf(1, "error\n");
    exit();
  }
  else
    printf(1, "%d\n", shmid);
  char * shm = shmat(shmid, (void *)0, 0);
  if(!shm)
    printf(1, "error\n");
  else
    printf(1, "%d\n", shm);
  for (char * s = shm; *s != 0; s++)
    printf(1, "%c", *s);
  printf(1, "\n");
  exit();
}
