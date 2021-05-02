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
    printf(1, "Memory segment of size 10000 and flags IPC_CREAT|0666 with previously existing key, returned shared memory segment identifier\n");
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
  int shmdet = shmdt(shm);
  if(shmdet == -1)
    printf(1, "error in shmdt\n");
  if(shmdet == 0)
    printf(1, "shmdt successful\n");
  struct shmid_ds shmid_ds, *buf;
  buf = &shmid_ds;
  int ctl = shmctl(shmid, IPC_RMID, buf);
  if(ctl < 0)
    printf(1, "error in shmctl with IPC_RMID\n");
  else
    printf(1, "IPC_RMID successful(shared memory segment has been destroyed)\n");
  int shmid2 = shmget(10, 10000, IPC_CREAT|IPC_EXCL|0666);
  if(shmid2 < 0)
    printf(1, "error in shmget\n");
  else
    printf(1, "Memory segment of size 10000 and flags IPC_CREAT|IPC_EXCL|0666 newly created\n\n");
  exit();
}
