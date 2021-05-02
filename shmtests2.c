#include "types.h"
#include "fcntl.h"
#include "types.h"
#include "user.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"

int main(){
  int shmid = shmget(10, 10000, IPC_CREAT|0666);
  if(shmid < 0)
    printf(1, "error in shmget\n");
  else
    printf(1, "Memory segment of size 1000 and flags IPC_CREAT|0666 with previously existing key, returned shared memory segment identifier\n");
  void * shm = shmat(shmid, (void *)0, 0);
  if((int)shm < 0)
    printf(1, "error in shmget\n");
  else{
    printf(1, "Shared memory segment attached to address space of process\n");
    char * s = shm;
    printf(1, "Reading from shared memory segment in same process...\n");
    for (s = shm; *s != 0; s++)
      printf(1, "%c", *s);
    printf(1, "\n");
  }
  exit();
}
